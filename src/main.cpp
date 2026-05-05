/**
 * @file   main.cpp
 * @brief  Entry point for the Genetic Simulation Engine.
 *
 *  This file is intentionally minimal.  All simulation logic lives in
 *  Environment / Agent / NeuralNetwork / Matrix.  main() constructs the
 *  Environment (which initialises the population and food) and calls run(),
 *  which drives the full evolutionary loop.
 *
 *  Build (GCC / Clang):
 *    g++ -std=c++17 -O2 -Wall -Wextra \
 *        main.cpp Matrix.cpp NeuralNetwork.cpp Agent.cpp Environment.cpp \
 *        -o genetic_sim
 *
 *  Build (CMake — preferred):
 *    cmake -B build && cmake --build build
 *    ./build/genetic_sim
 *
 *  Expected console output (example, values are stochastic):
 *    Gen   Best Fit     Avg Fit      Worst Fit    BestFood   AvgFood    Alive
 *    ───────────────────────────────────────────────────────────────────────
 *    1     121.45       14.72        9.90         1          0.12       23
 *    2     234.11       38.09        9.90         2          0.35       31
 *    ...   (fitness rises as the population learns to navigate toward food)
 *    60    1804.33      962.44       411.70       17         9.20       60
 *
 *  The rising Best/Avg Fitness and Food values confirm that the genetic
 *  algorithm is successfully driving evolution.
 */

#include "Environment.h"
#include "Config.h"

#include <iostream>
#include <memory>

// Helper: print startup banner with simulation parameters.
static void printStartupInfo() {
    // Compute genome size from Config constants (mirrors NeuralNetwork logic).
    const int genomeSize =
          (Config::INPUT_NEURONS  * Config::HIDDEN_NEURONS) + Config::HIDDEN_NEURONS
        + (Config::HIDDEN_NEURONS * Config::OUTPUT_NEURONS) + Config::OUTPUT_NEURONS;

    std::cout << "\n";
    std::cout << "  Initialising Genetic Simulation Engine...\n\n";
    std::cout << "  ┌─────────────────────────────────────────────┐\n";
    std::cout << "  │  World        : "
              << Config::WORLD_WIDTH << " × " << Config::WORLD_HEIGHT << " units"
              << std::string(17 - std::to_string(Config::WORLD_WIDTH).size()
                                - std::to_string(Config::WORLD_HEIGHT).size(), ' ')
              << "│\n";
    std::cout << "  │  Population   : " << Config::POPULATION_SIZE
              << std::string(28 - std::to_string(Config::POPULATION_SIZE).size(), ' ')
              << "│\n";
    std::cout << "  │  Food count   : " << Config::FOOD_COUNT
              << std::string(28 - std::to_string(Config::FOOD_COUNT).size(), ' ')
              << "│\n";
    std::cout << "  │  Generations  : " << Config::NUM_GENERATIONS
              << std::string(28 - std::to_string(Config::NUM_GENERATIONS).size(), ' ')
              << "│\n";
    std::cout << "  │  Epoch steps  : " << Config::EPOCH_STEPS
              << std::string(28 - std::to_string(Config::EPOCH_STEPS).size(), ' ')
              << "│\n";
    std::cout << "  │  NN layout    : "
              << Config::INPUT_NEURONS << " → "
              << Config::HIDDEN_NEURONS << " → "
              << Config::OUTPUT_NEURONS
              << "  (tanh activation)"
              << std::string(6, ' ') << "│\n";
    std::cout << "  │  Genome size  : " << genomeSize << " genes"
              << std::string(22 - std::to_string(genomeSize).size(), ' ')
              << "│\n";
    std::cout << "  │  Mutation     : "
              << static_cast<int>(Config::MUTATION_RATE * 100) << "% rate, "
              << Config::MUTATION_STRENGTH << " σ"
              << std::string(18, ' ') << "│\n";
    std::cout << "  │  Elitism      : "
              << static_cast<int>(Config::ELITISM_RATIO * 100) << "% preserved"
              << std::string(19, ' ') << "│\n";
    std::cout << "  └─────────────────────────────────────────────┘\n\n";
}

int main() {
    printStartupInfo();

    // Environment owns the entire simulation; smart pointer guarantees cleanup.
    auto env = std::make_unique<Environment>();
    env->run();

    return 0;
}
