//
// Created by charlie on 3/2/22.
//

#include "program_args.h"

#include <iostream>
#include <getopt.h>
#include <stdexcept>

const static struct option program_options[] = {
        { "output", required_argument, nullptr, 'o' },
        { "help", no_argument, nullptr, 'h' },
        { "repetitions", required_argument, nullptr, 'r' },
        { nullptr, 0, nullptr, 0 }
};

ProgramArgs::ProgramArgs(int argc, char **argv)
: repetitions(1) {

    const char *opt_str = "o:r:h";
    int c;
    while ((c = getopt_long(argc, argv, opt_str, program_options, nullptr)) != -1) {
        switch (c) {

            case 'h':
                ProgramArgs::print_help();
                exit(0);

            case 'o':
                this->output_file = std::string(optarg);
                break;

            case 'r':
                this->repetitions = std::stoul(std::string(optarg));
                break;

            default:
                abort();
        }
    }

    // Check if there are two arguments left
    int last_arg = argc - 1;

    if (argc - optind < 2) {
        // There aren't enough arguments left
        throw std::runtime_error("Did not specify enough positional arguments");
    }

    // second to last positional argument is the db file
    if (argv[last_arg - 1])
        this->clusters_file = std::string(argv[last_arg - 1]);
    else
        throw std::runtime_error("DB file positional argument not set");

    // last positional argument is the query file
    if (argv[last_arg - 1])
        this->query_file = std::string(argv[last_arg]);
    else
        throw std::runtime_error("Query file positional argument not set");
}

void ProgramArgs::print_help() noexcept {
    std::cout << "cluster_eval - tool for evaluating clusters\n";
    std::cout << "USAGE: cluster_eval [options] [db_file] [query_file]\n";
    std::cout << "\n";
    std::cout << "options:\n";
    std::cout << "-o, --output      : output all of the measurements to a file. Otherwise, goes to stdout\n";
    std::cout << "-r, --repetitions : repeat the query multiple times, average out all of the results\n";
    std::cout << "-h, --help:   display this help screen\n";
    std::cout << std::endl;
}
