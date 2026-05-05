#include "Environment.h"
#include <fstream>

#include <algorithm>   // std::sort, std::max
#include <numeric>     // std::accumulate
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <cmath>       // std::abs
// =============================================================================
//  Construction
// =============================================================================

Environment::Environment()
    : m_currentGeneration(0)
    // Seed the RNG from a hardware source for genuine randomness each run.
    , m_rng(static_cast<std::mt19937::result_type>(
          std::chrono::steady_clock::now().time_since_epoch().count()))
    , m_uniform01(0.0f, 1.0f)
{
    m_statsFile.open("evolution_log.csv"); // Opens the file
if (m_statsFile.is_open()) {
    m_statsFile << "Gen,BestFitness,AvgFitness,FoodEaten\n"; // Header
}
    // ── Build population ──────────────────────────────────────────────────
    m_population.reserve(Config::POPULATION_SIZE);
    for (int i = 0; i < Config::POPULATION_SIZE; ++i) {
        m_population.push_back(std::make_unique<Agent>(i));
    }

    // ── Seed food supply ──────────────────────────────────────────────────
    m_foods.resize(Config::FOOD_COUNT);
    for (std::size_t i = 0; i < m_foods.size(); ++i) {
        spawnFoodAt(i);
    }

    // Initialise all-time-best tracker.
    m_allTimeBest.bestFitness = -1.0f;
}

// =============================================================================
//  run   — public entry point
// =============================================================================

void Environment::run() {
    printHeader();

    for (int gen = 1; gen <= Config::NUM_GENERATIONS; ++gen) {
        m_currentGeneration = gen;

        // ── Reset epoch state ────────────────────────────────────────────
        //    Agents keep their genomes but lose their fitness / energy.
        for (auto& agent : m_population) {
            agent->resetForEpoch();
        }
        placeAgents();

        // Scatter a fresh batch of food pellets at the start of each epoch.
        for (std::size_t i = 0; i < m_foods.size(); ++i) {
            spawnFoodAt(i);
        }

        // ── Run epoch ────────────────────────────────────────────────────
        GenerationStats stats = runEpoch(gen);
        printStats(stats);
        if (m_statsFile.is_open()) {
    m_statsFile << stats.generation << "," 
                << stats.bestFitness << "," 
                << stats.avgFitness << "," 
                << stats.bestFoodEaten << "\n";
}

        // Update all-time-best record.
        if (stats.bestFitness > m_allTimeBest.bestFitness) {
            m_allTimeBest = stats;
        }

        // ── Evolve (not after the final generation) ──────────────────────
        if (gen < Config::NUM_GENERATIONS) {
            evolve();
        }
    }

    printFooter(m_allTimeBest);
}

// =============================================================================
//  runEpoch   — drives EPOCH_STEPS ticks, then collects statistics
// =============================================================================

GenerationStats Environment::runEpoch(int generationNumber) {
    for (int step = 0; step < Config::EPOCH_STEPS; ++step) {
        tick();

        // Early-exit optimisation: if every agent is dead, there is
        // nothing left to simulate for this epoch.
        bool anyAlive = false;
        for (const auto& a : m_population) {
            if (a->isAlive()) { anyAlive = true; break; }
        }
        if (!anyAlive) break;
    }

    // ── Collect statistics ────────────────────────────────────────────────
    GenerationStats stats;
    stats.generation = generationNumber;

    float totalFitness  = 0.0f;
    float totalFood     = 0.0f;
    float worstFitness  = std::numeric_limits<float>::max();
    int   alive         = 0;

    stats.bestFitness   = -1.0f;
    stats.bestFoodEaten = 0;

    for (const auto& agent : m_population) {
        float f  = agent->getFitness();
        int   fe = agent->getFoodEaten();

        totalFitness += f;
        totalFood    += static_cast<float>(fe);

        if (f > stats.bestFitness) {
            stats.bestFitness   = f;
            stats.bestFoodEaten = fe;
        }
        if (f < worstFitness) {
            worstFitness = f;
        }
        if (agent->isAlive()) ++alive;
    }

    int N              = Config::POPULATION_SIZE;
    stats.avgFitness   = totalFitness / static_cast<float>(N);
    stats.worstFitness = worstFitness;
    stats.avgFoodEaten = totalFood    / static_cast<float>(N);
    stats.aliveAtEnd   = alive;

    return stats;
}

// =============================================================================
//  tick   — single time-step for the entire simulation
// =============================================================================

void Environment::tick() {
    // Update all living agents (they sense, think, move internally).
    for (auto& agent : m_population) {
        if (agent->isAlive()) {
            agent->update(m_foods);
        }
    }

    // Collision detection runs after all agents have moved.
    checkFoodCollisions();
}

// =============================================================================
//  checkFoodCollisions
// =============================================================================
//
//  For each living agent, test against every food pellet.
//  Collision criterion: distance² ≤ (agentRadius + foodRadius)²
//
//  On collision:
//    • Agent gains energy and fitness.
//    • Food pellet teleports to a new random location (infinite food density).
//
void Environment::checkFoodCollisions() {
    const float collDist   = Config::AGENT_RADIUS + Config::FOOD_RADIUS;
    const float collDistSq = collDist * collDist;

    for (auto& agent : m_population) {
        if (!agent->isAlive()) continue;

        float ax = agent->getX();
        float ay = agent->getY();

        for (std::size_t fi = 0; fi < m_foods.size(); ++fi) {
            auto [fx, fy] = m_foods[fi];

            float dx = ax - fx;
            float dy = ay - fy;

            if ((dx * dx + dy * dy) <= collDistSq) {
                // Agent eats this pellet.
                agent->addEnergy(Config::FOOD_ENERGY_REWARD);
                agent->addFitness(Config::FITNESS_PER_FOOD);
                agent->incrementFoodCount();

                // Immediately respawn the pellet at a different location so
                // the food density stays constant throughout the epoch.
                spawnFoodAt(fi);
            }
        }
    }
}

// =============================================================================
//  spawnFoodAt
// =============================================================================

void Environment::spawnFoodAt(std::size_t index) {
    m_foods[index] = {
        randomFloat(0.0f, static_cast<float>(Config::WORLD_WIDTH)),
        randomFloat(0.0f, static_cast<float>(Config::WORLD_HEIGHT))
    };
}

// =============================================================================
//  placeAgents   — randomise positions and headings before an epoch
// =============================================================================

void Environment::placeAgents() {
    for (auto& agent : m_population) {
        agent->setPosition(
            randomFloat(0.0f, static_cast<float>(Config::WORLD_WIDTH)),
            randomFloat(0.0f, static_cast<float>(Config::WORLD_HEIGHT)));
        agent->setAngle(randomFloat(-3.14159f, 3.14159f));
    }
}

// =============================================================================
//  evolve   — the core Genetic Algorithm
// =============================================================================

void Environment::evolve() {
    // ── Step 1: Rank population by fitness (best first) ───────────────────
    std::sort(m_population.begin(), m_population.end(),
        [](const std::unique_ptr<Agent>& a, const std::unique_ptr<Agent>& b) {
            return a->getFitness() > b->getFitness();
        });

    const int popSize    = Config::POPULATION_SIZE;
    const int eliteCount = std::max(2, static_cast<int>(
        static_cast<float>(popSize) * Config::ELITISM_RATIO));

    // ── Step 2: Cache elite genomes ───────────────────────────────────────
    //    We cache them before writing any new genomes to avoid overwriting
    //    an elite's genome while it is still being used as a parent.
    std::vector<std::vector<float>> eliteGenomes;
    eliteGenomes.reserve(static_cast<std::size_t>(eliteCount));
    for (int i = 0; i < eliteCount; ++i) {
        eliteGenomes.push_back(m_population[i]->getGenome());
    }

    // ── Step 3: Reproduce — fill non-elite slots ──────────────────────────
    //
    //  Parent selection: uniform random from the elite pool.
    //  Two distinct parents are always chosen (when eliteCount > 1).
    //
    for (int i = eliteCount; i < popSize; ++i) {
        int p1 = randomInt(0, eliteCount - 1);
        int p2 = randomInt(0, eliteCount - 1);

        // Guarantee distinct parents to prevent trivial self-crossover.
        if (eliteCount > 1) {
            while (p2 == p1) p2 = randomInt(0, eliteCount - 1);
        }

        // Single-point crossover → child genome
        std::vector<float> child = crossover(eliteGenomes[p1], eliteGenomes[p2]);

        // Gaussian mutation
        mutate(child);

        // Load the new genome into this agent.
        m_population[i]->setGenome(child);
    }

    // ── Step 4: Restore elite genomes (unchanged) ─────────────────────────
    //    Elites survive to the next generation exactly as they are.
    for (int i = 0; i < eliteCount; ++i) {
        m_population[i]->setGenome(eliteGenomes[i]);
    }
}

// =============================================================================
//  crossover   — single-point recombination
// =============================================================================

std::vector<float> Environment::crossover(const std::vector<float>& p1,
                                            const std::vector<float>& p2) {
    const int N = static_cast<int>(p1.size());

    // Crossover point in [1, N-1] so each parent contributes at least one gene.
    const int cp = 1 + randomInt(0, N - 2);

    std::vector<float> child(static_cast<std::size_t>(N));
    for (int i = 0; i < N; ++i) {
        child[i] = (i < cp) ? p1[i] : p2[i];
    }
    return child;
}

// =============================================================================
//  mutate   — per-gene Gaussian perturbation
// =============================================================================

void Environment::mutate(std::vector<float>& genome) {
    std::normal_distribution<float> gauss(0.0f, Config::MUTATION_STRENGTH);

    for (auto& gene : genome) {
        if (m_uniform01(m_rng) < Config::MUTATION_RATE) {
            gene += gauss(m_rng);
        }
    }
}

// =============================================================================
//  Console output
// =============================================================================

void Environment::printHeader() const {
    // Column widths
    const int wGen  = 6;
    const int wStat = 13;
    const int wFood = 11;
    const int wAlive= 8;
    const int total = wGen + wStat * 3 + wFood * 2 + wAlive;

    std::cout << "\n";
    std::cout << std::string(total, '=') << "\n";
    std::cout << "   GENETIC SIMULATION ENGINE  —  C++17 / Neural Evolution\n";
    std::cout << std::string(total, '=') << "\n";
    std::cout << "   Population : " << Config::POPULATION_SIZE
              << "  |  Genome : "
              << (Config::INPUT_NEURONS * Config::HIDDEN_NEURONS) + Config::HIDDEN_NEURONS
               + (Config::HIDDEN_NEURONS * Config::OUTPUT_NEURONS) + Config::OUTPUT_NEURONS
              << " genes"
              << "  |  Hidden neurons : " << Config::HIDDEN_NEURONS
              << "  |  Mutation : " << Config::MUTATION_RATE * 100.0f << "%\n";
    std::cout << std::string(total, '-') << "\n";

    std::cout << std::left
              << std::setw(wGen)  << "Gen"
              << std::setw(wStat) << "Best Fit"
              << std::setw(wStat) << "Avg Fit"
              << std::setw(wStat) << "Worst Fit"
              << std::setw(wFood) << "BestFood"
              << std::setw(wFood) << "AvgFood"
              << std::setw(wAlive)<< "Alive"
              << "\n";
    std::cout << std::string(total, '-') << "\n";
}

void Environment::printStats(const GenerationStats& s) const {
    const int wGen  = 6;
    const int wStat = 13;
    const int wFood = 11;
    const int wAlive= 8;

    // Highlight generations with notable jumps in best fitness.
    std::cout << std::left  << std::fixed << std::setprecision(2)
              << std::setw(wGen)  << s.generation
              << std::setw(wStat) << s.bestFitness
              << std::setw(wStat) << s.avgFitness
              << std::setw(wStat) << s.worstFitness
              << std::setw(wFood) << s.bestFoodEaten
              << std::setw(wFood) << s.avgFoodEaten
              << std::setw(wAlive)<< s.aliveAtEnd
              << "\n";
}

void Environment::printFooter(const GenerationStats& best) const {
    int total = 6 + 13 * 3 + 11 * 2 + 8;
    std::cout << std::string(total, '=') << "\n";
    std::cout << "  Simulation complete after " << Config::NUM_GENERATIONS
              << " generations.\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  All-time best — Generation " << best.generation
              << "  |  Fitness: " << best.bestFitness
              << "  |  Food eaten: " << best.bestFoodEaten << "\n";
    std::cout << std::string(total, '=') << "\n\n";
}

// =============================================================================
//  Random utilities
// =============================================================================

float Environment::randomFloat(float low, float high) {
    return low + m_uniform01(m_rng) * (high - low);
}

int Environment::randomInt(int low, int high) {
    // Inclusive [low, high]
    std::uniform_int_distribution<int> dist(low, high);
    return dist(m_rng);
}
Environment::~Environment() {
    if (m_statsFile.is_open()) m_statsFile.close();
}