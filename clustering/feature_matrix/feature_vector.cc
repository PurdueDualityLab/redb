//
// Created by charlie on 4/4/22.
//

#include "feature_vector.h"

#include <utility>
#include "../ThreadPool.h"
#include "../program_options.h"

FeatureVector::FeatureVector(const std::string& pattern, std::string classification, const StringVector &string_vec)
: classification(std::move(classification)) {
    // Build the regex
    this->regex = std::make_shared<re2::RE2>(pattern);
    if (!this->regex->ok())
        throw std::runtime_error("Could not build regex");

    ThreadPool workers(ProgramOptions::instance().workers);

    auto strings = string_vec.create_vector();
    this->match_vector = std::vector<bool>(strings.size());

    std::vector<std::future<std::pair<size_t, bool>>> tasks;
    for (std::vector<std::string>::size_type i = 0; i < strings.size(); i++) {
        auto task = workers.enqueue([](const std::shared_ptr<re2::RE2>& matcher, const std::string& test_string, std::vector<std::string>::size_type index) {
            bool is_match = re2::RE2::FullMatch(test_string, *matcher);
            return std::pair<std::vector<std::string>::size_type, bool> { index, is_match };
        }, this->regex, strings[i], i);
        tasks.push_back(std::move(task));
    }

    // Collect everything
    for (auto &task : tasks) {
        task.wait();
        auto [index, matches] = task.get();
        this->match_vector[index] = matches;
    }
}
