//
// Created by charlie on 4/4/22.
//

#include "string_vector.h"
#include "../ThreadPool.h"
#include "../program_options.h"
#include <egret.h>

StringVector::StringVector(const std::vector<std::string> &patterns) {
    // Try out using the global workers variable
    ThreadPool workers(ProgramOptions::instance().workers);

    std::vector<std::future<std::vector<std::string>>> tasks;
    for (const auto &pattern : patterns) {
        auto fut = workers.enqueue([](const std::string &pattern) {
            try {
                return run_engine(pattern, "clustering");
            } catch (std::runtime_error &exe) {
                return std::vector<std::string> {};
            }
        }, pattern);

        tasks.push_back(std::move(fut));
    }

    // Collect all the strings, make sure only duplicates are kept
    for (auto &task : tasks) {
        task.wait();
        auto result = task.get();
        auto all_strings_inserter = std::inserter(this->all_strings, this->all_strings.end());
        std::move(result.begin(), result.end(), all_strings_inserter);
    }
}

std::vector<std::string> StringVector::create_vector() const {
    // Assuming that set gives an ordered list
    std::vector<std::string> string_vector;
    std::copy(this->all_strings.cbegin(), this->all_strings.cend(), std::back_inserter(string_vector));
    return string_vector;
}
