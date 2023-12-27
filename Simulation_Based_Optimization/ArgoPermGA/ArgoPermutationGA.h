#pragma once
#include <algorithm>
#include <vector>
#include <random>
#include <numeric>
#include <iostream>
#include <cassert>
#include <set>

class ArgoPermutationGA {
 public:
    ArgoPermutationGA(const size_t &num_container, const bool &sim_result_loadable) {
        setNumSimContainer(num_container);
        
        //input argc 0
        if (!sim_result_loadable) {
            std::cout << "Initializing population..." << std::endl;
            initializePopulation();
        } else {
            // Read previous simulation results
            std::cout << "Reading existing simulation results..." << std::endl;
            GetHalfElitesUsingJsonData();
            assert(population_.size() * 2 == population_size_);
            crossoverAndMutation();
        }

    }
    ArgoPermutationGA(size_t populationSize, size_t chromosomeLength, size_t generations)
        : population_size_(populationSize),
          chromosome_length_(chromosomeLength),
          generations_(generations) {
    }

    std::vector<int> run();
    void exportToJson();

 private:
    size_t population_size_ = 48;
    size_t chromosome_length_ = 20;
    size_t generations_ = 50;

    const double mutationRate = 0.05; // 돌연변이 확률

    std::vector<std::vector<int>> population_;
    std::random_device rd_;
    std::mt19937 mt_19937_;

    // input and output json filename
    size_t num_sim_container_{0};
    std::vector<std::string> input_sim_result_file_names_;
    std::vector<std::string> output_sim_candidate_file_names_;

    void evaluatePopulation();

    void initializePopulation();

    void selection() {
        // Simply Select elites as many as the half of population_
        std::nth_element(population_.begin(), population_.begin() + population_size_ / 2, population_.end(),
                         [this](const std::vector<int> &a, const std::vector<int> &b) {
                             return fitnessFunction(a) > fitnessFunction(b);
                         });
        population_.resize(population_size_ / 2);
    }

    void crossoverAndMutation() {
        // 단순한 1점 교차를 수행합니다.
        std::uniform_int_distribution<> distribution(0, population_size_ / 2 - 1);
        std::uniform_real_distribution<> chance(0.0, 1.0);

        while (population_.size() < population_size_) {
            auto &parent1 = population_[distribution(mt_19937_)];
            auto &parent2 = population_[distribution(mt_19937_)];

            std::vector<int> child = orderBasedCrossover(parent1, parent2);

            // 돌연변이 확률을 적용합니다.
            if (chance(mt_19937_) < mutationRate) {
                swapMutation(child);
            }
#if !defined(NDEBUG)
            std::set<int> child_set(child.begin(), child.end());
            if(child_set.size() != child.size()){
                throw std::runtime_error("unknown exception");
            }

#endif

            population_.push_back(child);
        }
    }

    double fitnessFunction(const std::vector<int> &chromosome) {
        // 적합도 함수는 순열의 합을 계산합니다.
        return std::accumulate(chromosome.begin(), chromosome.end(), 0);
    }

    std::vector<int> orderBasedCrossover(const std::vector<int> &parent1, const std::vector<int> &parent2) {
        std::vector<int> child(parent1.size());
        std::uniform_int_distribution<> dist(0, parent1.size() - 1);

        int start = dist(mt_19937_);
        int end = dist(mt_19937_);
        if (start > end) {
            std::swap(start, end);
        }

        std::vector<bool> taken(parent1.size(), false);
        for (int i = start; i <= end; ++i) {
            child[i] = parent1[i];
            taken[parent1[i]] = true;
        }

        int idx = 0;
        for (int gene : parent2) {
            if (!taken[gene]) {
                if (idx == start) {
                    idx = end + 1;
                }
                child[idx++] = gene;
            }
        }

        return child;
    }

    void swapMutation(std::vector<int> &chromosome) {
        // 단순한 교환 돌연변이를 수행합니다.
        std::uniform_int_distribution<> distribution(0, chromosome_length_ - 1);
        int index1 = distribution(mt_19937_);
        int index2 = distribution(mt_19937_);
        std::swap(chromosome[index1], chromosome[index2]);
    }

    std::vector<int> selectBestChromosome() {
        // 가장 적합한 chromosome을 반환합니다.
        auto best = std::max_element(population_.begin(), population_.end(),
                                     [this](const std::vector<int> &a, const std::vector<int> &b) {
                                         return fitnessFunction(a) < fitnessFunction(b);
                                     });
        return *best;
    }

    // container 설정
    void setNumSimContainer(const size_t &num_sim_container);

    void GetHalfElitesUsingJsonData();
};
