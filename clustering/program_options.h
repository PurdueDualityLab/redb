//
// Created by charlie on 2/15/22.
//

#ifndef _PROGRAM_OPTIONS_H
#define _PROGRAM_OPTIONS_H

#include <string>
#include <optional>
#include <thread>

enum CorpusType {
    OBJECTS, // json object per line, each object has a pattern key-value pair
    PAIRS, // Each line has an id, whitespace, and then patterns
    CLUSTERS // json array of arrays. Each subarray is a cluster
};

class ProgramOptions {
public:
    static const ProgramOptions &instance() {
        return ProgramOptions::global_options_instance;
    }

    static void set_instance(ProgramOptions &&options) {
        ProgramOptions::global_options_instance = std::move(options);
    }

    ProgramOptions()
            : inflation(1.8)
            , pruning(0)
            // default to using all available cores. If running on a big computer, specify something else
            , workers(std::thread::hardware_concurrency())
            , corpus_type(CorpusType::PAIRS)
            , strict_rex_string_checking(false)
    {  }

    friend std::ostream &operator<<(std::ostream &os, const ProgramOptions &opts);

    double inflation;
    double pruning;
    std::optional<std::string> graph_out;
    std::optional<std::string> cluster_out;
    std::optional<std::string> patterns_file_out;
    std::string corpus_file;
    unsigned int workers;
    CorpusType corpus_type;
    bool strict_rex_string_checking;

    static ProgramOptions global_options_instance;
};


#endif //_PROGRAM_OPTIONS_H
