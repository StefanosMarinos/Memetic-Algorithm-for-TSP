# Genetic Algorithm for TSP (C++)

This project solves the **Travelling Salesman Problem (TSP)** using a **Genetic Algorithm** with local optimization.

## Features
- Tournament Selection
- Order Crossover (OX)
- Swap Mutation
- 2-opt Local Search
- Elitism

## Compile & Run

```bash
g++ MemeticAlgorithm.cpp -o tsp
./tsp
```

## Input
- Number of cities
- Symmetric distance matrix

## Flexibility
- Changeable popsize, generations, elitesize, rng probability for more percise rsults in cost of time!

## Notes
- Optimized for larger instances (`n > 10`)
- Suggests exact methods for small instances
