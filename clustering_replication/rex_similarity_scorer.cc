//
// Created by charlie on 2/10/22.
//

#include "rex_similarity_scorer.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <re2/re2.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

RexSimilarityScorer::RexSimilarityScorer(const std::string &pattern, unsigned long id, const RexWrapper &rex_wrapper)
: BaseSimilarityScorer(pattern, id) {

    // Generate strings for this pattern
    std::vector<std::string> strings;
    try {
        strings = rex_wrapper.generate_strings(pattern, 400);
    } catch (std::runtime_error &exe) {
        throw exe;
    }

    nlohmann::json strings_obj;
    for (auto &str : strings)
        strings_obj.push_back(str);

    // open a new file
    this->strings_file_path = "/tmp/rex_strings" + std::to_string(random()) + ".json";
    std::ofstream strings_file(this->strings_file_path);
    strings_file << strings_obj;

    // Build a pattern for this regex
    this->regex = std::make_unique<re2::RE2>(pattern);
    if (!this->regex->ok()) {
        throw std::runtime_error("Regex is not supported by re2");
    }
}

double RexSimilarityScorer::score(std::shared_ptr<BaseSimilarityScorer> other_scorer) {
    // TODO the smarter way to do this would be to have a regex pointer in the base scorer, but that's
    // not necessarily general
    auto rex_scorer = std::dynamic_pointer_cast<RexSimilarityScorer>(other_scorer);
    double hits = 0;
    auto strings = this->load_strings();
    for (const auto &str : strings) {
        if (rex_scorer->test_string(str)) {
            hits++;
        }
    }

    return hits / (double) strings.size();
}

std::vector<std::string> RexSimilarityScorer::load_strings() {
    int strings_file_fd = open(this->strings_file_path.c_str(), O_RDONLY);
    if (strings_file_fd < 0) {
        throw std::runtime_error("Could not read strings file");
    }

    struct stat strings_file_stats;
    if (fstat(strings_file_fd, &strings_file_stats) == -1) {
        throw std::runtime_error("Could not stat strings file");
    }

    size_t file_length = strings_file_stats.st_size;

    // Load the strings into data
    char *strings_buffer_raw = static_cast<char *>(mmap(nullptr, file_length, PROT_READ, MAP_PRIVATE, strings_file_fd, 0));
    std::string strings_buffer(strings_buffer_raw, file_length);

    // Parse the strings via json
    auto strings = nlohmann::json::parse(strings_buffer).get<std::vector<std::string>>();

    munmap(strings_buffer_raw, file_length);
    close(strings_file_fd);

    return strings;
}

RexSimilarityScorer::~RexSimilarityScorer() {
    remove(this->strings_file_path.c_str());
}

bool RexSimilarityScorer::test_string(const std::string &subject) const {
    return re2::RE2::PartialMatch(subject, *this->regex);
}
