# C++_Genetic-Evolution-Simulator

This code implements a neuroevolutionary simulation where a population of "Agents" evolves over multiple generations to find food in a 2D world. It uses a Genetic Algorithm to optimize a Neural Network that acts as the agent's brain.  

How it Works:

Sensing & Thinking: Each Agent "sees" the distance and angle to the nearest food pellet and monitors its own energy. These inputs are fed into a 3-layer Neural Network.  
Acting: The Neural Network outputs motor commands—specifically linear and angular velocity—to move the agent through the environment.  
Survival & Physics: Agents lose energy over time (metabolism) and gain energy/fitness by "eating" food pellets. The world is toroidal, meaning if an agent goes off one edge, it wraps around to the other side.  
Evolution: At the end of each generation (epoch), the simulation performs three steps:
Selection: The highest-performing "elite" agents are preserved.  
Crossover: New agents are created by mixing the genetic code (neural network weights) of two successful parents.  
Mutation: Small random changes are applied to the new agents' genes to encourage discovery of new behaviors. 


Key Components:
->
Environment: Manages the loop, handles collisions, and runs the genetic algorithm.  
Agent: Encapsulates the physical state and the individual neural network.  
NeuralNetwork & Matrix: Provide the mathematical framework for the agent's decision-making.  
Config: Contains all tunable parameters, such as mutation rates and world size.


Educational Proof-of-Concept
->The project is a clean, academic implementation of Neuroevolution. It serves to demonstrate how a Genetic Algorithm can train a Neural Network without traditional backpropagation.  

Single-Threaded Logic
->The simulation runs on a single thread. While it uses efficient C++17 structures, it does not currently utilize multi-threading (like OpenMP or std::async) to process the population in parallel, nor does it use GPU acceleration (CUDA/OpenCL) for the matrix math.  

Lightweight Neural Architecture
->The "intelligence" is handled by a very small, 3-layer feed-forward network.  
Input Layer: 3 neurons.  
Hidden Layer: 12 neurons.  
Output Layer: 2 neurons.
With only 74 total genes (parameters), the computational load is minimal for any modern CPU.  

Simplified Physics
->The "Environment" uses basic 2D Euclidean distance for collision detection rather than a dedicated physics engine like Box2D. It uses a Toroidal (wrap-around) world, meaning there are no complex boundary calculations or spatial partitioning systems (like Quadtrees).  

Standard Linear Algebra
->The Matrix.h file provides a custom template for matrix operations. While functional, it does not use specialized SIMD (Single Instruction, Multiple Data) instructions or highly optimized libraries like Eigen or BLAS which are standard in industry-grade "high-performance" machine learning.



To build on linux->
go to project root and then 

cmake -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build

./build/bin/genetic_sim

To build on windows->
go to project directory then

cmake -B build

cmake --build build --config Release

.\build\bin\Release\genetic_sim.exe
