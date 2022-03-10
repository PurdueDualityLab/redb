//
// Created by charlie on 12/25/21.
//

#include "args_parser.h"

#include <iostream>
#include <getopt.h>

redb::server::ArgsParser::ArgsParser(int argc, char **argv)
: db_seed_path(getenv("CLUSTER_DB_PATH") ? getenv("CLUSTER_DB_PATH") : "")
, port(8080) {
    // setup the arguments
    struct option options[] = {
            { "port", required_argument, nullptr, 'p' },
            { "db-file", required_argument, nullptr, 'f' },
            { "help", no_argument, nullptr, 'h' },
            { nullptr, 0, nullptr, 0 }
    };

    int option_index = 0;
    int c;
    while (true) {
        c = getopt_long(argc, argv, "p:f:", options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'p':
                // port
                this->port = (short) std::stoi(std::string(optarg));
                break;

            case 'f':
                this->db_seed_path = std::string(optarg);
                break;

            case 'h':
                ArgsParser::print_help();
                exit(0);

            default:
                std::cerr << "Undefined program argument" << std::endl;
                break;
        }
    }
}

void redb::server::ArgsParser::print_help() {
    std::cout << "redb-server - Regular expression reuse API and Database\n";
    std::cout << "Usage: redb-server [options]\n";
    std::cout << '\n';
    std::cout << "Options:\n";
    std::cout << "-p, --port    : port to host the server on. Default is 8080\n";
    std::cout << "-f, --db-file : path to file that contains the regex database. Database should be clustered\n";
    std::cout << "-h, --help    : show this help text\n";
    std::cout << '\n';
    std::cout << "NOTE: a database file must be provided somehow. If -f is not specified, then the application\n"
                 "will use the environment variable CLUSTER_DB_PATH. One of these two must be specified\n";
    std::cout << std::endl;
}
