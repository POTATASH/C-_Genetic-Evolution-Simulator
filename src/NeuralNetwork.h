#pragma once

#include "Matrix.h"
#include <vector>
#include <cmath>   // std::tanh, std::sqrt

/**
 * @class NeuralNetwork
 * @brief A fully-connected feed-forward network with one hidden layer.
 *
 *  Architecture
 *  ─────────────────────────────────────────────────────────
 *  [INPUT_NEURONS] ──► (W1, b1) ──► tanh ──►
 *  [HIDDEN_NEURONS] ──► (W2, b2) ──► tanh ──►
 *  [OUTPUT_NEURONS]
 *
 *  Activation function: tanh  (outputs in [-1, 1] — ideal for velocity signals)
 *
 *  The Genome
 *  ─────────────────────────────────────────────────────────
 *  All learnable parameters are serialised to / from a flat
 *  std::vector<float> in this canonical order:
 *
 *    W1 (row-major, HIDDEN×INPUT)   →  HIDDEN*INPUT  values
 *    b1 (HIDDEN×1)                  →  HIDDEN        values
 *    W2 (row-major, OUTPUT×HIDDEN)  →  OUTPUT*HIDDEN values
 *    b2 (OUTPUT×1)                  →  OUTPUT        values
 *
 *  Total genome size = HIDDEN*(INPUT+1) + OUTPUT*(HIDDEN+1)
 *                    = 12*(3+1) + 2*(12+1)  =  74  genes  (with default Config)
 *
 *  Genome access is intentionally restricted to Agent so the Environment
 *  never touches raw weights directly — a deliberate OOP encapsulation choice.
 */
class NeuralNetwork {
public:
    // -------------------------------------------------------------------------
    //  Construction
    // -------------------------------------------------------------------------
    NeuralNetwork(int inputSize, int hiddenSize, int outputSize);

    // Explicitly default copy/move so callers can clone networks easily.
    NeuralNetwork(const NeuralNetwork&)            = default;
    NeuralNetwork& operator=(const NeuralNetwork&) = default;
    NeuralNetwork(NeuralNetwork&&)                 = default;
    NeuralNetwork& operator=(NeuralNetwork&&)      = default;

    // -------------------------------------------------------------------------
    //  Inference
    // -------------------------------------------------------------------------

    /**
     * @brief Execute a forward pass through the network.
     * @param inputs  Sensor readings — must be of size inputSize.
     * @return Output activations of size outputSize (values in [-1, 1]).
     */
    std::vector<float> feedForward(const std::vector<float>& inputs) const;

    // -------------------------------------------------------------------------
    //  Genome Interface   (used exclusively by Agent)
    // -------------------------------------------------------------------------

    /// @return Total number of learnable float parameters.
    int getGenomeSize() const noexcept;

    /// @return All weights and biases as a flat vector (canonical order above).
    std::vector<float> getGenome() const;

    /// @brief Load weights and biases from a flat vector of the correct length.
    void setGenome(const std::vector<float>& genome);

    /// @brief Xavier/Glorot uniform initialisation for all parameters.
    void randomizeWeights();

private:
    int m_inputSize;
    int m_hiddenSize;
    int m_outputSize;

    // Weight matrices and bias column-vectors
    Matrix<float> m_W1;  ///< [hiddenSize × inputSize]
    Matrix<float> m_b1;  ///< [hiddenSize × 1]
    Matrix<float> m_W2;  ///< [outputSize × hiddenSize]
    Matrix<float> m_b2;  ///< [outputSize × 1]

    // Activation function — static so it can be passed to Matrix::apply.
    static float tanhFn(float x) noexcept { return std::tanh(x); }
};
