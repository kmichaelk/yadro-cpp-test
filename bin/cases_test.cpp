#include <iostream>
#include <fstream>
#include <filesystem>
#include <optional>

#include "cclublogproc.h"

const std::string tests_directory = "../cases/";
const std::string solutions_directory = tests_directory + "solutions/";
const std::string file_extension = ".txt";

std::optional<std::string> run_test(const std::string &name) {
    std::ifstream in(tests_directory + name);
    std::ifstream solution(solutions_directory + name);

    std::stringstream out;

    if (!process_club_log(in, out, out)) {
        return "bad data";
    }

    std::stringstream ss_solution;
    ss_solution << solution.rdbuf();

    if (out.str() != ss_solution.str()) {
        return "wrong answer";
    }

    return std::nullopt;
}

int main() {
    for (auto &f : std::filesystem::directory_iterator(tests_directory)) {
        if (!f.is_regular_file() || f.path().extension().string() != file_extension) {
            continue;
        }

        const auto &name = f.path().filename().string();
        const auto result = run_test(name);

        std::cout << "Test " << std::quoted(name) << " ";
        if (result.has_value()) {
            std::cout << "failed: " << result.value();
        } else {
            std::cout << "passed";
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}