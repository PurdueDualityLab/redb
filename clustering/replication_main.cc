//
// Created by charlie on 2/9/22.
//

#include "similarity_table/similarity_table.h"
#include "mcl_wrapper.h"
#include "cluster_set.h"

#include <iostream>
#include <unordered_map>
#include <optional>
#include <getopt.h>
#include <re2/re2.h>

static int help;

static const struct option program_args[] = {
        { "inflation", required_argument, nullptr, 'i' },
        { "pruning", required_argument, nullptr, 'p' },
        { "graph-out", required_argument, nullptr, 'g' },
        { "cluster-out", required_argument, nullptr, 'o' },
        { "patterns-file", required_argument, nullptr, 'P' },
        { "parallel", required_argument, nullptr, 'j' },
        { "help", no_argument, nullptr, 'h' },
        { nullptr, 0, nullptr, 0 }
};

struct OptionValues {
public:
    OptionValues()
    : inflation(1.8)
    , pruning(0)
    // default to using all available cores. If running on a big computer, specify something else
    , workers(std::thread::hardware_concurrency())
    {  }

    double inflation;
    double pruning;
    std::optional<std::string> graph_out;
    std::optional<std::string> cluster_out;
    std::optional<std::string> patterns_file_out;
    std::string corpus_file;
    unsigned int workers;
};

static void display_help() {
    std::cout << "replication - replicate chapman and stolee clustering results\n";
    std::cout << "usage: replication [options] [corpus_file.txt]\n";
    std::cout << "\n";
    std::cout << "options:\n";
    std::cout << "-i, --inflation:    set the mcl inflation parameter (default is 1.8)\n";
    std::cout << "-p, --pruning:      set the mcl pruning parameter (default is off)\n";
    std::cout << "-g, --graph-out:    path to a file to write out the resulting similarity graph\n";
    std::cout << "-o, --cluster-out:  path to a file to write the resulting cluster with ids (default is clusters.txt)\n";
    std::cout << "-P, --patterns-out: file to write the resulting clusters to, ids mapped to patterns\n";
    std::cout << "-j, --parallel:     how many workers to work with (NOTE: default is all available cores in the computer)\n";
    std::cout << "-h, --help:         display this help screen" << std::endl;
}

static OptionValues read_program_opts(int argc, char **argv) {
    const char *getopt_str = "p:i:g:o:P:j:h";
    int c;
    int opt_index;
    OptionValues option_values;
    while ((c = getopt_long(argc, argv, getopt_str, program_args, &opt_index)) != -1) {
        switch (c) {
            case 'h':
                // help = 1;
                display_help();
                exit(0);

            case 'i': {
                float inflation_value = std::stof(std::string(optarg));
                option_values.inflation = static_cast<double>(inflation_value);
                break;
            }

            case 'p': {
                float pruning_value = std::stof(std::string(optarg));
                option_values.pruning = static_cast<double>(pruning_value);
                break;
            }

            case 'g':
                option_values.graph_out = std::string(optarg);
                break;

            case 'o':
                option_values.cluster_out = std::string(optarg);
                break;

            case 'P':
                option_values.patterns_file_out = std::string(optarg);
                break;

            case 'j':
                option_values.workers = std::stoi(std::string(optarg));
                break;

            default:
                throw std::runtime_error("Unexpected argument");
        }
    }

    // get the last positional argument
    option_values.corpus_file = std::string(argv[optind]);

    return option_values;
}

static std::unordered_map<unsigned long, std::string> read_patterns(const std::string& path) {
    std::ifstream pattern_file(path);
    re2::RE2 parser("^(\\d+)\\s+(.*)");
    std::string line;
    std::unordered_map<unsigned long, std::string> patterns;
    while (std::getline(pattern_file, line)) {
        unsigned long id;
        std::string pattern;
        if (re2::RE2::FullMatch(line, parser, &id, &pattern)) {
            patterns[id] = pattern;
        }
    }

    pattern_file.close();

    return patterns;
}

int main(int argc, char **argv) {

    // read arguments
    auto program_arguments = read_program_opts(argc, argv);

    if (help) {
        display_help();
        return 0;
    }

    // read in all the patterns
    auto patterns = read_patterns(program_arguments.corpus_file);

    // build the similarity table
    MclWrapper mcl_wrapper("/usr/local/bin/mcl");
    SimilarityTable table(patterns, program_arguments.workers);
    // table.prune(.25);
    table.to_similarity_graph();
    std::string abc_graph;
    if (program_arguments.graph_out)
         abc_graph = table.to_abc(*program_arguments.graph_out);
    else
        abc_graph = table.to_abc();

    std::vector<std::vector<unsigned long>> raw_clusters;
    if (program_arguments.cluster_out) {
        raw_clusters = mcl_wrapper.cluster(abc_graph, program_arguments.inflation, *program_arguments.cluster_out);
    } else {
        raw_clusters = mcl_wrapper.cluster(abc_graph, program_arguments.inflation);
    }

    ClusterSet cluster_set(raw_clusters);
    std::cout << cluster_set << std::endl;
    if (program_arguments.patterns_file_out) {
        std::ofstream patterns_file(*program_arguments.patterns_file_out);
        cluster_set.write_patterns(table, patterns_file);
        patterns_file.close();
    } else {
        std::cout << "\n\n\n" << std::endl;
        cluster_set.write_patterns(table, std::cout);
        std::cout << std::endl;
    }

    return 0;
}
