#pragma once

#include "NeuralNetwork.h"
#include "Config.h"
#include "GameObject.h"

#include <vector>
#include <memory>
#include <utility>   // std::pair

/**
 * @class Agent
 * @brief Represents a single creature in the simulation.
 *
 *  Each Agent owns a NeuralNetwork (its "brain") whose weights ARE the genome.
 *  The Agent encapsulates everything about the creature:
 *
 *   • Sensing   — reads distance / angle to nearest food, normalised energy
 *   • Thinking  — runs the sensor data through its neural network
 *   • Acting    — translates network outputs into physical movement
 *   • Biology   — tracks energy, fitness, alive state, food count
 *
 *  Encapsulation Contract
 *  ─────────────────────────────────────────────────────────────────────────
 *  The Environment NEVER touches the Agent's internal NeuralNetwork directly.
 *  All weight access goes through getGenome() / setGenome(), preserving the
 *  "creature as black-box" invariant central to the design.
 *
 *  Memory
 *  ─────────────────────────────────────────────────────────────────────────
 *  The brain is stored as a std::unique_ptr<NeuralNetwork>; Agents themselves
 *  are owned by Environment via std::unique_ptr<Agent>.  No raw new/delete.
 */
class Agent : public GameObject {
public:
    void update() override { }
    /// Convenience alias for a list of (x, y) food positions.
    using FoodList = std::vector<std::pair<float, float>>;

    // -------------------------------------------------------------------------
    //  Construction
    // -------------------------------------------------------------------------

    /**
     * @param id  Unique identifier (used for debugging / logging).
     *
     *  Constructs the agent with a freshly randomised NeuralNetwork.
     *  Position and angle are set to zero; the Environment will call
     *  setPosition() and setAngle() before the first epoch.
     */
    explicit Agent(int id);

    // Rule of five: unique_ptr member requires explicit declarations.
    Agent(const Agent&)            = delete;   // Agents are not copyable (use cloneGenome)
    Agent& operator=(const Agent&) = delete;
    Agent(Agent&&)                 = default;
    Agent& operator=(Agent&&)      = default;
    ~Agent()                       = default;

    // -------------------------------------------------------------------------
    //  Per-Tick Update   (called by Environment::tick)
    // -------------------------------------------------------------------------

    /**
     * @brief Execute one simulation step: sense → think → act → drain energy.
     * @param foods  Current food positions (read-only).
     */
    void update(const FoodList& foods);

    // -------------------------------------------------------------------------
    //  Sensing   (also called by Environment for sensor debugging)
    // -------------------------------------------------------------------------

    /**
     * @brief Build the neural network input vector from world state.
     *
     *  Returns a 3-element vector:
     *    [0] Distance to nearest food, normalised to [-1, 1]
     *    [1] Relative angle to nearest food, normalised to [-1, 1]
     *    [2] Current energy, normalised to [-1, 1]
     */
    std::vector<float> sense(const FoodList& foods) const;

    // -------------------------------------------------------------------------
    //  Lifecycle Management   (called by Environment between epochs)
    // -------------------------------------------------------------------------

    /// @brief Reset energy, fitness, velocity and alive flag for a new epoch.
    ///        Genome (brain weights) are NOT touched here — evolution handles that.
    void resetForEpoch();

    // -------------------------------------------------------------------------
    //  Genome Interface   (used ONLY by the Genetic Algorithm in Environment)
    // -------------------------------------------------------------------------
    std::vector<float> getGenome()                           const;
    void               setGenome(const std::vector<float>&   g);
    int                getGenomeSize()                       const;

    // -------------------------------------------------------------------------
    //  State Mutators   (called by Environment on collision detection)
    // -------------------------------------------------------------------------
    void addEnergy(float amount);
    void addFitness(float amount);
    void incrementFoodCount() noexcept { ++m_foodEaten; }
    void kill()               noexcept { m_isAlive = false; }

    // -------------------------------------------------------------------------
    //  Placement   (called by Environment before each epoch)
    // -------------------------------------------------------------------------
    void setPosition(float x, float y) noexcept { m_x = x; m_y = y; }
    void setAngle   (float angle)      noexcept { m_angle = angle; }

    // -------------------------------------------------------------------------
    //  Read-only State Accessors
    // -------------------------------------------------------------------------
    float getX()         const noexcept { return m_x; }
    float getY()         const noexcept { return m_y; }
    float getAngle()     const noexcept { return m_angle; }
    float getEnergy()    const noexcept { return m_energy; }
    float getFitness()   const noexcept { return m_fitness; }
    bool  isAlive()      const noexcept { return m_isAlive; }
    int   getId()        const noexcept { return m_id; }
    int   getFoodEaten() const noexcept { return m_foodEaten; }

private:
    // ── Identity ─────────────────────────────────────────────────────────────
    int m_id;

    // ── Brain (owned) ────────────────────────────────────────────────────────
    std::unique_ptr<NeuralNetwork> m_brain;

    // ── Physics ──────────────────────────────────────────────────────────────
    float m_x;           ///< World X position
    float m_y;           ///< World Y position
    float m_angle;       ///< Heading in radians  (-π … +π)
    float m_velLinear;   ///< Current forward speed (units/tick)
    float m_velAngular;  ///< Current rotation speed (rad/tick)

    // ── Biology ──────────────────────────────────────────────────────────────
    float m_energy;
    float m_fitness;
    bool  m_isAlive;
    int   m_foodEaten;  ///< Pellets consumed this epoch

    // ── Private Helpers ──────────────────────────────────────────────────────
    void  applyMovement();
    float distanceTo(float fx, float fy) const noexcept;
    float relativeAngleTo(float fx, float fy) const noexcept;
    void  wrapPosition() noexcept;   ///< Toroidal (wrap-around) world boundary
};
