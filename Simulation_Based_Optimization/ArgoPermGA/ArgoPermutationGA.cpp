#include <fstream>
#include "ArgoPermutationGA.h"
#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"


void ArgoPermutationGA::evaluatePopulation() {

}
void ArgoPermutationGA::initializePopulation() {
    population_.clear();

    std::vector<int> jobs;
    jobs.reserve(20);
    for (int i = 0; i < 20; ++i) {
        jobs.push_back(i);
    }
//    for (int i = 0; i < 5; ++i) {
//        jobs.push_back(2);
//    }
//    for (int i = 0; i < 5; ++i) {
//        jobs.push_back(3);
//    }
//    for (int i = 0; i < 5; ++i) {
//        jobs.push_back(4);
//    }

    for (size_t i = 0; i < population_size_; ++i) {
        std::vector<int> chromosome(jobs);
        // Shuffle the elements of the vector
        std::shuffle(chromosome.begin(), chromosome.end(), mt_19937_);
        population_.push_back(chromosome);
    }
}

std::vector<int> ArgoPermutationGA::run() {
    initializePopulation();

    for (size_t generation = 0; generation < generations_; ++generation) {
        evaluatePopulation();
        selection();
        crossoverAndMutation();
    }

    return selectBestChromosome();
}
void ArgoPermutationGA::setNumSimContainer(const size_t &num_sim_container) {
    num_sim_container_ = num_sim_container;

    if (population_size_ % num_sim_container_ != 0) {
        throw std::runtime_error("population_size_ is not divisible by num_sim_container");
    }

    for (size_t i = 0; i < num_sim_container_; ++i) {
        std::string input_file_name = fmt::format("/home/share/individual_fitness_pairs_{}.json", i);
        std::string output_file_name = fmt::format("/home/share/sub_population_{}.json", i);;

        input_sim_result_file_names_.push_back(input_file_name);
        output_sim_candidate_file_names_.push_back(output_file_name);
    }
}
void ArgoPermutationGA::GetHalfElitesUsingJsonData() {
    population_.clear();

    std::vector<std::pair<std::vector<int>, int>> total_vectors;

    for (const auto &each_file_name : input_sim_result_file_names_) {
        std::ifstream in_file(each_file_name);

        if (!in_file.is_open()) {
            throw std::runtime_error(fmt::format("Failed to open {} for reading.", each_file_name));
        }

        nlohmann::json json_data;
        in_file >> json_data;

        // JSON 데이터에서 std::vector<std::vector<int>>와 int 데이터 복원
        std::vector<std::pair<std::vector<int>, int>> nestedVector = json_data["individual_fitness_pairs"];

        // total vector 수집
        total_vectors.insert(total_vectors.end(), nestedVector.begin(), nestedVector.end());
    }

    // second 값을 기준으로 오름차순으로 정렬합니다.
    std::sort(total_vectors.begin(), total_vectors.end(), [](const auto &lhs, const auto &rhs) {
        return lhs.second < rhs.second;
    });

    if (total_vectors.size() != population_size_) {
        throw std::runtime_error(fmt::format("collected population size {} does not match {}.",
                                             total_vectors.size(),
                                             population_size_));
    }

    total_vectors.resize(population_size_ / 2);

    // current best solution을 출력
    std::cout << "Current best solution (total_vectors):" << std::endl;
    std::cout << "Fitness: " << total_vectors.front().second << " | Individual: "<< fmt::format("{}", fmt::join(total_vectors.front().first, ",")) << std::endl;

    for (const auto &[individual, _] : total_vectors) {
        population_.push_back(individual);
    }
}
void ArgoPermutationGA::exportToJson() {
    assert(num_sim_container_ != 0);
    assert(population_.size() == population_size_);

    auto sub_population_size = population_size_ / num_sim_container_;

    auto file_name_iterator = output_sim_candidate_file_names_.begin();
    auto iter = population_.begin();

    for (size_t i = 0; i < num_sim_container_; ++i) {
        std::ofstream out_file(*file_name_iterator);

        std::cout << *file_name_iterator << std::endl;

        // set sub_population
        nlohmann::json json_data;
        auto next_iter = std::next(iter, sub_population_size);
        std::vector<std::vector<int>> sub_population(iter, next_iter);
        json_data["sub_population"] = sub_population;

        if (!out_file.is_open()) {
            throw std::runtime_error(fmt::format("Failed to open {} for writing.", *file_name_iterator));
        }

        // Write JSON data to the file
        out_file << json_data.dump(4); // 4-space indentation for pretty formatting

        // Close the file
        out_file.close();

        // update iter
        ++file_name_iterator;
        iter = next_iter;
    }

    // check iterator
    assert(file_name_iterator == output_sim_candidate_file_names_.end());
    assert(iter == population_.end());
}
