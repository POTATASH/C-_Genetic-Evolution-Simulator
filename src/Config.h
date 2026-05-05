#pragma once

/**
 * @file Config.h
 * @brief Central configuration hub for ALL simulation constants.
 *
 *        Tune simulation behaviour here without touching any other source file.
 *        Every constant lives in the Config namespace to prevent global name
 *        pollution and to make usage sites self-documenting (Config::MUTATION_RATE).
 */

namespace Config {

    // =========================================================================
    //  World
    // =========================================================================
    constexpr int   WORLD_WIDTH         = 800;   ///< Simulation world width  (units)
    constexpr int   WORLD_HEIGHT        = 600;   ///< Simulation world height (units)

    // =========================================================================
    //  Simulation
    // =========================================================================
    constexpr int   POPULATION_SIZE     = 60;    ///< Agents alive per generation
    constexpr int   FOOD_COUNT          = 40;    ///< Food pellets in the world at once
    constexpr int   EPOCH_STEPS         = 1000;  ///< Simulation ticks per generation
    constexpr int   NUM_GENERATIONS     = 60;    ///< Total generations to evolve

    // =========================================================================
    //  Neural Network Architecture
    // =========================================================================
    /// Inputs:  [dist_to_food_norm, angle_to_food_norm, energy_norm]
    constexpr int   INPUT_NEURONS       = 3;
    /// Single hidden layer — increase for more expressive brains
    constexpr int   HIDDEN_NEURONS      = 12;
    /// Outputs: [linear_velocity, angular_velocity]
    constexpr int   OUTPUT_NEURONS      = 2;

    // =========================================================================
    //  Agent Physics & Biology
    // =========================================================================
    constexpr float INITIAL_ENERGY      = 200.0f; ///< Starting energy each epoch
    constexpr float ENERGY_DRAIN_RATE   = 0.12f;  ///< Passive energy loss per tick
    constexpr float MAX_LINEAR_SPEED    = 3.5f;   ///< Max forward speed (units/tick)
    constexpr float MAX_ANGULAR_SPEED   = 0.14f;  ///< Max rotation (radians/tick)
    constexpr float AGENT_RADIUS        = 8.0f;   ///< Collision radius for agent
    constexpr float FOOD_RADIUS         = 6.0f;   ///< Collision radius for food

    // =========================================================================
    //  Energy Economy
    // =========================================================================
    constexpr float FOOD_ENERGY_REWARD  = 55.0f;  ///< Energy gained from eating food
    constexpr float FITNESS_PER_FOOD    = 100.0f; ///< Fitness points per food eaten
    constexpr float FITNESS_PER_TICK    = 0.01f;  ///< Survival bonus per live tick

    // =========================================================================
    //  Genetic Algorithm
    // =========================================================================
    constexpr float MUTATION_RATE       = 0.08f;  ///< Probability of mutating each gene
    constexpr float MUTATION_STRENGTH   = 0.28f;  ///< Gaussian std-dev applied on mutation
    constexpr float ELITISM_RATIO       = 0.20f;  ///< Top fraction preserved unchanged

} // namespace Config
