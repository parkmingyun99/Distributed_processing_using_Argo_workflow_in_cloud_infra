#include <iostream>
#include <chrono>

#include "sim_engine.h"
#include "coup_plant.h"
#include "csv_export.h"
#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"
#include <fstream>

std::deque<int> ConvertToJobs(const std::vector<int> &chromosome) {
    const std::vector<int> jobs = {1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4};
    assert(jobs.size() == chromosome.size());
    std::deque<int> cur_jobs;
    for (const auto &each_pos : chromosome) {
        cur_jobs.push_back(jobs.at(each_pos));
    }
    return cur_jobs;
}

int Evaluate(const std::vector<int> &chromosome) {
    const auto cur_jobs = ConvertToJobs(chromosome);
    auto *const coup_fab = new CoupPlant(cur_jobs);

    EngInstance().Reset(coup_fab);

    if (!EngInstance().Start()) {
        throw std::runtime_error(fmt::format(
            "[opt] sim failed, param = {}", fmt::join(chromosome, ",")));
    }

    const auto exec_cycle_time = EngInstance().GetCurClock();
    std::cout << fmt::format("jobs: {}, fitness = {}", fmt::join(cur_jobs, ","), exec_cycle_time) << std::endl;
    // for maximization
    return exec_cycle_time;
}

int main(const int argc, char **argv) {
    try {
        // 첫 번째 인수를 확인하고, 0 또는 1이 아니면 예외 발생
        if (argc != 2) {
            throw std::invalid_argument(
                "Invalid argument. The first argument must be container id, which starts 0 to n.");
        }

        int container_id = std::stoi(argv[1]);
        // Argument 값 출력
        std::cout << "container id: " << container_id << std::endl;

        std::string filename = fmt::format("/home/share/sub_population_{}.json", container_id);

        // JSON 파일 읽기
        nlohmann::json in_json_data;

        std::ifstream input_file(filename);
        if (!input_file.is_open()) {
            throw std::runtime_error(fmt::format("Failed to open the json file: {}", filename));
        }
        input_file >> in_json_data;
        // Get the "sub_population" item from JSON data
        std::vector<std::pair<std::vector<int>, int>> individual_fitness_pairs;

        if (in_json_data.find("sub_population") != in_json_data.end()) {
            std::vector<std::vector<int>> sub_population = in_json_data["sub_population"];

            std::cout << "Contents of 'sub_population':" << std::endl;
            for (const std::vector<int> &row : sub_population) {
                const auto exec_cycle_time = Evaluate(row);
//                std::cout << "fit value = " << exec_cycle_time<< std::endl;
                individual_fitness_pairs.push_back(std::make_pair(row, exec_cycle_time));
            }
        } else {
            throw std::runtime_error(fmt::format("The JSON data does not contain a 'sub_population' item."));
        }

        // Create a new JSON object
        nlohmann::json out_json_data;
        out_json_data["individual_fitness_pairs"] = individual_fitness_pairs;

        // Create a output file
        std::string out_file_name = fmt::format("/home/share/individual_fitness_pairs_{}.json", container_id);
        std::ofstream output_file(out_file_name);
        if (!output_file.is_open()) {
            throw std::runtime_error("Failed to open the output file: " + out_file_name);
        }

        output_file << out_json_data.dump(4); // 4-space indentation for pretty formatting
        std::cout << "Data written to file: " << out_file_name << std::endl;
        std::cout << "Success" << std::endl;
    }
    catch (const std::invalid_argument &e) {
        std::cerr << "오류: " << e.what() << std::endl;
    }
    catch (std::bad_alloc &e) {
        std::cout << "bad alloc: " << e.what() << std::endl;
    }
    catch (const std::out_of_range &e) {
        std::cerr << "Out of Range error: " << e.what() << std::endl;
    }
    catch (std::exception &e) {
        std::cout << "except: " << e.what() << std::endl;
    }

    CsvExport::Instance().Export();

    return 0;
}
