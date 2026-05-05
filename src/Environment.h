#pragma once

#include "Agent.h"
#include "Config.h"
#include <fstream>

#include <vector>
#include <memory>
#include <utility>    // std::pair
#include <random>

// =============================================================================
//  GenerationStats
// =============================================================================

/**
 * @struct GenerationStats
 * @brief  Snapshot of evolution progress for one completed generation.
 */
struct GenerationStats {
    int   generation    = 0;
    float bestFitness   = 0.0f;
    float avgFitness    = 0.0f;
    float worstFitness  = 0.0f;
    int   bestFoodEaten = 0;
    float avgFoodEaten  = 0.0f;
    int   aliveAtEnd    = 0;   ///< Agents still alive when the epoch ended
};

// =============================================================================
//  Environment
// =============================================================================

/**
 * @class Environment
 * @brief The simulation orchestrator.
 *
 *  Responsibilities
 *  ─────────────────────────────────────────────────────────────────────────
 *  • Owns the population  (std::vector<std::unique_ptr<Agent>>)
 *  • Owns the food supply (std::vector<std::pair<float,float>>)
 *  • Drives the per-tick update loop
 *  • Detects and handles agent–food collisions
 *  • Triggers the Genetic Algorithm at the end of each epoch
 *
 *  Encapsulation Policy
 *  ─────────────────────────────────────────────────────────────────────────
 *  Environment never reads or writes brain weights directly.
 *  It accesses genomes only through Agent::getGenome() / Agent::setGenome(),
 *  keeping internal neural network details fully hidden behind the Agent API.
 */
class Environment {
public:
    Environment();
    ~Environment();

    /**
     * @brief Run the full simulation: NUM_GENERATIONS epochs with GA between each.
     *        Prints a formatted progress table to stdout.
     */
    void run();

private:
    // =========================================================================
    //  Core Simulation
    // =========================================================================
    std::ofstream m_statsFile;
    /// @brief Execute one complete epoch (EPOCH_STEPS ticks).
    GenerationStats runEpoch(int generationNumber);

    /// @brief Advance the world by a single tick.
    void tick();

    /// @brief Detect agent–food overlaps; reward eating; respawn food.
    void checkFoodCollisions();

    /// @brief Place food pellet i at a fresh random location.
    void spawnFoodAt(std::size_t index);

    /// @brief Scatter all agents at random positions and headings.
    void placeAgents();

    // =========================================================================
    //  Genetic Algorithm
    // =========================================================================

    /**
     * @brief  Run one generation of evolution on the current population.
     *
     *  Algorithm
     *  ─────────
     *  1. Sort population by fitness (descending).
     *  2. Preserve the top ELITISM_RATIO fraction unchanged (elites).
     *  3. Fill remaining slots with children produced by single-point
     *     crossover from two elite parents, followed by Gaussian mutation.
     */
    void evolve();

    /**
     * @brief  Single-point crossover of two parent genomes.
     *
     *  A random crossover point cp ∈ [1, N-1] is chosen uniformly.
     *  child[i] = parent1[i]  for i < cp
     *  child[i] = parent2[i]  for i >= cp
     *
     * @param p1  First parent genome.
     * @param p2  Second parent genome.
     * @return    Child genome of the same length.
     */
    std::vector<float> crossover(const std::vector<float>& p1,
                                  const std::vector<float>& p2);

    /**
     * @brief  Apply Gaussian mutations to a genome in-place.
     *
     *  Each gene is independently mutated with probability MUTATION_RATE.
     *  When mutated, Gaussian noise N(0, MUTATION_STRENGTH²) is added.
     *
     * @param genome  Genome to mutate (modified in-place).
     */
    void mutate(std::vector<float>& genome);

    // =========================================================================
    //  Console Output
    // =========================================================================
    void printHeader()                       const;
    void printStats(const GenerationStats& s) const;
    void printFooter(const GenerationStats& best) const;

    // =========================================================================
    //  Random Utility
    // =========================================================================
    float randomFloat(float low, float high);
    int   randomInt  (int   low, int   high);  // inclusive [low, high]

    // =========================================================================
    //  Data Members
    // =========================================================================
    std::vector<std::unique_ptr<Agent>>   m_population; ///< Owned agents
    std::vector<std::pair<float, float>>  m_foods;      ///< (x, y) pellet positions

    int m_currentGeneration; ///< Generation counter (1-based)

    // Mersenne-Twister RNG — seeded once at construction, used for all
    // random decisions in the GA and world setup.
    std::mt19937                          m_rng;
    std::uniform_real_distribution<float> m_uniform01;  ///< [0, 1)

    // Tracking the all-time best for the final summary.
    GenerationStats m_allTimeBest;
};
