#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <numeric>
#include <climits>

using namespace std;

// Compute total tour cost
int calculateFitness(const vector<int>& tour, const vector<vector<int>>& dist) {
    int total = 0;

    for (size_t i = 0; i < tour.size() - 1; i++)
        total += dist[tour[i]][tour[i + 1]];

    return total;
}

// 2-opt local search: improves solution by reversing segments
void localSearch(vector<int>& tour, const vector<vector<int>>& dist) {
    bool improved = true;

    while (improved) {
        improved = false;

        for (size_t i = 1; i < tour.size() - 2; i++) {
            for (size_t j = i + 1; j < tour.size() - 1; j++) {

                int oldCost = dist[tour[i - 1]][tour[i]] +
                    dist[tour[j]][tour[j + 1]];

                int newCost = dist[tour[i - 1]][tour[j]] +
                    dist[tour[i]][tour[j + 1]];

                if (newCost < oldCost) {
                    reverse(tour.begin() + i, tour.begin() + j + 1);
                    improved = true;
                }
            }
        }
    }
}

// Swap mutation: randomly swaps two cities
void mutate(vector<int>& tour, mt19937& rng, double mutationRate) {
    uniform_real_distribution<double> prob(0.0, 1.0);

    if (prob(rng) > mutationRate)
        return;

    uniform_int_distribution<int> dist(1, tour.size() - 2);

    int i = dist(rng);
    int j = dist(rng);

    swap(tour[i], tour[j]);
}

// Order Crossover (OX): preserves ordering of parent tours
vector<int> crossover(const vector<int>& p1, const vector<int>& p2, mt19937& rng) {
    int n = p1.size();

    vector<int> child(n, -1);
    child[0] = 0;
    child[n - 1] = 0;

    uniform_int_distribution<int> dist(1, n - 2);

    int start = dist(rng);
    int end = dist(rng);

    if (start > end)
        swap(start, end);

    vector<bool> used(n, false);
    used[0] = true;

    // Copy segment from parent 1
    for (int i = start; i <= end; i++) {
        child[i] = p1[i];
        used[p1[i]] = true;
    }

    // Fill remaining cities from parent 2
    int idx = 1;

    for (int i = 1; i < n - 1; i++) {
        if (!used[p2[i]]) {
            while (child[idx] != -1) idx++;
            child[idx] = p2[i];
            used[p2[i]] = true;
        }
    }

    return child;
}

// Tournament selection: picks best of two random individuals
vector<int> tournamentSelect(const vector<vector<int>>& pop, const vector<int>& fitness, mt19937& rng) {

    uniform_int_distribution<int> dist(0, pop.size() - 1);

    int a = dist(rng);
    int b = dist(rng);

    return (fitness[a] < fitness[b]) ? pop[a] : pop[b];
}

int main() {

    int n;
    cout << "Enter number of cities: ";
    cin >> n;

    if (n > 10) {
        vector<vector<int>> dist(n, vector<int>(n));
        char choice;
        cout << "Do you want to generate random distances? (y/n): ";
        cin >> choice;

        random_device rd;
        mt19937 rng(rd());
        // Ορίζουμε ένα εύρος τιμών για τις τυχαίες αποστάσεις, π.χ. 1-100
        uniform_int_distribution<int> uni(1, 100);

        // Γέμισμα πίνακα
        for (int i = 0; i < n; i++) {
            for (int j = i; j < n; j++) {
                if (i == j) {
                    dist[i][j] = 0;
                }
                else {
                    if (choice == 'y' || choice == 'Y') {
                        dist[i][j] = uni(rng);
                    }
                    else {
                        cout << "Distance " << i + 1 << " -> " << j + 1 << ": ";
                        cin >> dist[i][j];
                    }
                    dist[j][i] = dist[i][j]; // Συμμετρικότητα
                }
            }
        }

        // Εκτύπωση πίνακα για επιβεβαίωση (προαιρετικό)
        if (choice == 'y' || choice == 'Y') {
            cout << "\nGenerated Distance Matrix:\n";
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    cout << dist[i][j] << "\t";
                }
                cout << endl;
            }
        }

        int popSize = 100; // Population size, can be tuned based on problem size

        int generations = 300; // Number of generations, can be increased for better solutions at the cost of time
        int eliteSize = 10; // Number of top individuals to carry over

		vector<vector<int>> population; // The Tours
		vector<int> fitness; // Corresponding fitness values (total distance) for each tour

        // Initialize random population of valid tours
        for (int p = 0; p < popSize; p++) {

            vector<int> tour; //vector to hold the tour, starting with city 0

            for (int i = 1; i < n; i++) {
                tour.push_back(i);
            }
            shuffle(tour.begin(), tour.end(), rng);

            tour.insert(tour.begin(), 0);
            tour.push_back(0);

            population.push_back(tour);
            fitness.push_back(calculateFitness(tour, dist));
        }

        // Track global best solution
        int bestFitness = INT_MAX;
		vector<int> bestTour; // To store the best tours found

        // Main GA loop
        for (int gen = 0; gen < generations; gen++) {

            vector<vector<int>> newPop;
            vector<int> newFitness;

            // Sort population for elitism
            vector<int> idx(popSize);
            iota(idx.begin(), idx.end(), 0);

            sort(idx.begin(), idx.end(),[&](int a, int b) {return fitness[a] < fitness[b];});

            // Keep top elite individuals
            for (int i = 0; i < eliteSize; i++) {

				newPop.push_back(population[idx[i]]); //carry over best solutions
                newFitness.push_back(fitness[idx[i]]);
            }

			uniform_real_distribution<double> prob(0.0, 1.0); // 0 meaning no mutation, 1 meaning always mutate

            // Generate new population
            while (newPop.size() < popSize) {

                vector<int> p1 = tournamentSelect(population, fitness, rng);
                vector<int> p2 = tournamentSelect(population, fitness, rng);

                vector<int> child = crossover(p1, p2, rng);

                double mutationRate = 0.2 * (1.0 - double(gen) / generations);
                mutate(child, rng, mutationRate);

                // Occasional local improvement (reduces computation cost)
				if (prob(rng) < 0.3) //probability of applying local search to the child, it can be tuned based on problem size and desired balance between exploration and exploitation
                    localSearch(child, dist);

                int fit = calculateFitness(child, dist);

                newPop.push_back(child);
                newFitness.push_back(fit);

                // Update global best solution
                if (fit < bestFitness) {
                    bestFitness = fit;
                    bestTour = child;
                }
            }

            population = newPop;
            fitness = newFitness;
        }

        // Final result output
        cout << "\nBest Distance: " << bestFitness << endl;

        cout << "Best Path: ";
        for (int city : bestTour)
            cout << city + 1 << " ";

        cout << endl;
    }
    else {
        cout << "For small number of cities (<=10), consider using brute-force or dynamic programming approaches for exact solutions." << endl;
	}

    return 0;
}
