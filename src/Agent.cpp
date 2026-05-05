#include "Agent.h"
#include "Config.h"
#include <cmath>
#include <limits>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────────────
//  Constants & Pre-computations
// ─────────────────────────────────────────────────────────────────────────────

static constexpr float PI     = 3.14159265358979f;
static constexpr float TWO_PI = 2.0f * PI;

static const float WORLD_DIAGONAL = std::sqrt(
    static_cast<float>(Config::WORLD_WIDTH  * Config::WORLD_WIDTH) +
    static_cast<float>(Config::WORLD_HEIGHT * Config::WORLD_HEIGHT));

// =============================================================================
//  Construction
// =============================================================================

Agent::Agent(int id)
    : GameObject(0.0f, 0.0f) 
    , m_id         (id)
    , m_brain      (std::make_unique<NeuralNetwork>(
                        Config::INPUT_NEURONS,
                        Config::HIDDEN_NEURONS,
                        Config::OUTPUT_NEURONS))
    , m_x          (0.0f)
    , m_y          (0.0f)
    , m_angle      (0.0f)
    , m_velLinear  (0.0f)
    , m_velAngular (0.0f)
    , m_energy     (Config::INITIAL_ENERGY)
    , m_fitness    (0.0f)
    , m_isAlive    (true)
    , m_foodEaten  (0)
{}

// =============================================================================
//  Update Loop
// =============================================================================

void Agent::update(const FoodList& foods) {
    if (!m_isAlive) return;

    // 1. Gather environmental data
    std::vector<float> inputs = sense(foods);

    // 2. Process through the neural network
    std::vector<float> outputs = m_brain->feedForward(inputs);

    // 3. Rescale outputs to motor commands
    m_velLinear  = ((outputs[0] + 1.0f) * 0.5f) * Config::MAX_LINEAR_SPEED;
    m_velAngular = outputs[1] * Config::MAX_ANGULAR_SPEED;

    // 4. Update position and heading
    applyMovement();

    // 5. Apply metabolic costs and longevity rewards
    m_energy -= Config::ENERGY_DRAIN_RATE;
    m_fitness += Config::FITNESS_PER_TICK;

    // 6. Check for starvation
    if (m_energy <= 0.0f) {
        m_energy  = 0.0f;
        m_isAlive = false;
    }
}

// =============================================================================
//  Sensing Logic
// =============================================================================

std::vector<float> Agent::sense(const FoodList& foods) const {
    float nearestDist  = std::numeric_limits<float>::max();
    float nearestAngle = 0.0f;

    for (const auto& [fx, fy] : foods) {
        float d = distanceTo(fx, fy);
        if (d < nearestDist) {
            nearestDist  = d;
            nearestAngle = relativeAngleTo(fx, fy);
        }
    }

    float normDist   = (nearestDist / WORLD_DIAGONAL) * 2.0f - 1.0f;
    float normAngle  = nearestAngle / PI;
    float normEnergy = (m_energy / (Config::INITIAL_ENERGY * 2.0f)) * 2.0f - 1.0f;

    return { normDist, normAngle, normEnergy };
}

// =============================================================================
//  State Management
// =============================================================================

void Agent::resetForEpoch() {
    m_energy     = Config::INITIAL_ENERGY;
    m_fitness    = 0.0f;
    m_isAlive    = true;
    m_foodEaten  = 0;
    m_velLinear  = 0.0f;
    m_velAngular = 0.0f;
}

void Agent::addEnergy(float amount) {
    m_energy = std::min(m_energy + amount, Config::INITIAL_ENERGY * 2.0f);
}

void Agent::addFitness(float amount) {
    m_fitness += amount;
}

// =============================================================================
//  Genome Delegation
// =============================================================================

std::vector<float> Agent::getGenome() const        { return m_brain->getGenome(); }
void               Agent::setGenome(const std::vector<float>& g) { m_brain->setGenome(g); }
int                Agent::getGenomeSize() const    { return m_brain->getGenomeSize(); }

// =============================================================================
//  Physics / Movement (Private)
// =============================================================================

void Agent::applyMovement() {
    m_angle += m_velAngular;

    while (m_angle >  PI) m_angle -= TWO_PI;
    while (m_angle < -PI) m_angle += TWO_PI;

    m_x += std::cos(m_angle) * m_velLinear;
    m_y += std::sin(m_angle) * m_velLinear;

    wrapPosition();
}

float Agent::distanceTo(float fx, float fy) const noexcept {
    float dx = fx - m_x;
    float dy = fy - m_y;
    return std::sqrt(dx * dx + dy * dy);
}

float Agent::relativeAngleTo(float fx, float fy) const noexcept {
    float worldAngle = std::atan2(fy - m_y, fx - m_x);
    float relative   = worldAngle - m_angle;

    while (relative >  PI) relative -= TWO_PI;
    while (relative < -PI) relative += TWO_PI;

    return relative;
}

void Agent::wrapPosition() noexcept {
    const float W = static_cast<float>(Config::WORLD_WIDTH);
    const float H = static_cast<float>(Config::WORLD_HEIGHT);

    if (m_x <  0.0f) m_x += W;
    if (m_x >= W)    m_x -= W;
    if (m_y <  0.0f) m_y += H;
    if (m_y >= H)    m_y -= H;
}