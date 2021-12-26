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
            { "db-path-file", required_argument, nullptr, 'f' },
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

            default:
                std::cerr << "Undefined program argument" << std::endl;
                break;
        }
    }
}
