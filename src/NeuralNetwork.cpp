#include "NeuralNetwork.h"
#include <stdexcept>
#include <cmath>

// =============================================================================
//  Construction
// =============================================================================

NeuralNetwork::NeuralNetwork(int inputSize, int hiddenSize, int outputSize)
    : m_inputSize (inputSize)
    , m_hiddenSize(hiddenSize)
    , m_outputSize(outputSize)
    , m_W1(hiddenSize, inputSize)     // [H × I]
    , m_b1(hiddenSize, 1)             // [H × 1]
    , m_W2(outputSize, hiddenSize)    // [O × H]
    , m_b2(outputSize, 1)             // [O × 1]
{
    randomizeWeights();
}

// =============================================================================
//  feedForward
// =============================================================================

std::vector<float> NeuralNetwork::feedForward(const std::vector<float>& inputs) const {
    if (static_cast<int>(inputs.size()) != m_inputSize) {
        throw std::runtime_error(
            "NeuralNetwork::feedForward: expected " + std::to_string(m_inputSize)
            + " inputs, got " + std::to_string(inputs.size()));
    }

    // Fix: Explicitly specify <float> for the template type
    Matrix<float> x = Matrix<float>::fromVector(inputs);      // (INPUT × 1)
    Matrix<float> h = m_W1 * x + m_b1;                        // (HIDDEN × 1)
    h.apply(tanhFn);                                          // element-wise tanh

    Matrix<float> y = m_W2 * h + m_b2;                        // (OUTPUT × 1)
    y.apply(tanhFn);                                          // element-wise tanh

    return y.toVector();
}

// =============================================================================
//  getGenomeSize
// =============================================================================

int NeuralNetwork::getGenomeSize() const noexcept {
    return (m_hiddenSize * m_inputSize)  + m_hiddenSize   // W1 + b1
         + (m_outputSize * m_hiddenSize) + m_outputSize;  // W2 + b2
}

// =============================================================================
//  getGenome   (serialise all parameters → flat vector)
// =============================================================================

std::vector<float> NeuralNetwork::getGenome() const {
    std::vector<float> genome;
    genome.reserve(static_cast<std::size_t>(getGenomeSize()));

    // W1, b1, W2, b2 - Accessing m_W1 which is now a Matrix<float>
    for (int i = 0; i < m_hiddenSize; ++i)
        for (int j = 0; j < m_inputSize; ++j)
            genome.push_back(m_W1.at(i, j));

    for (int i = 0; i < m_hiddenSize; ++i)
        genome.push_back(m_b1.at(i, 0));

    for (int i = 0; i < m_outputSize; ++i)
        for (int j = 0; j < m_hiddenSize; ++j)
            genome.push_back(m_W2.at(i, j));

    for (int i = 0; i < m_outputSize; ++i)
        genome.push_back(m_b2.at(i, 0));

    return genome;
}

// =============================================================================
//  setGenome   (deserialise flat vector → matrices)
// =============================================================================

void NeuralNetwork::setGenome(const std::vector<float>& genome) {
    if (static_cast<int>(genome.size()) != getGenomeSize()) {
        throw std::runtime_error("NeuralNetwork::setGenome: genome length mismatch");
    }

    int idx = 0;

    for (int i = 0; i < m_hiddenSize; ++i)
        for (int j = 0; j < m_inputSize; ++j)
            m_W1.at(i, j) = genome[idx++];

    for (int i = 0; i < m_hiddenSize; ++i)
        m_b1.at(i, 0) = genome[idx++];

    for (int i = 0; i < m_outputSize; ++i)
        for (int j = 0; j < m_hiddenSize; ++j)
            m_W2.at(i, j) = genome[idx++];

    for (int i = 0; i < m_outputSize; ++i)
        m_b2.at(i, 0) = genome[idx++];
}

// =============================================================================
//  randomizeWeights   (Xavier / Glorot uniform)
// =============================================================================

void NeuralNetwork::randomizeWeights() {
    float lim1 = std::sqrt(6.0f / static_cast<float>(m_inputSize  + m_hiddenSize));
    float lim2 = std::sqrt(6.0f / static_cast<float>(m_hiddenSize + m_outputSize));

    // These calls require randomize() to be defined in Matrix.h[cite: 18]
    m_W1.randomize(-lim1, lim1);
    m_b1.randomize(-0.05f, 0.05f); 

    m_W2.randomize(-lim2, lim2);
    m_b2.randomize(-0.05f, 0.05f);
}