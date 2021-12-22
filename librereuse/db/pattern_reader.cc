//
// Created by charlie on 9/19/21.
//

#include "pattern_reader.h"

#include <fstream>
// #include "nlohmann/json.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;

std::vector<std::string> rereuse::db::read_patterns(std::istream &input_stream) {
    std::vector<std::string> patterns;
    std::string line;
    while (std::getline(input_stream, line)) {
        try {
            auto line_obj = json::parse(line);
            auto pattern = line_obj["pattern"];
            // TODO Filter out super long patterns maybe?
            patterns.push_back(std::move(pattern));
        } catch (json::parse_error &err) {
            // Just keep going
            continue;
        }
    }

    return patterns;
}

std::vector<std::string> rereuse::db::read_patterns_from_path(const std::string &path) {
    std::ifstream db_file(path);
    if (db_file.is_open()) {
        std::vector<std::string> patterns = rereuse::db::read_patterns(db_file);
        db_file.close();
        return patterns;
    } else {
        return std::vector<std::string>();
    }
}
