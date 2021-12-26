//
// Created by charlie on 12/25/21.
//

#ifndef _ARGS_PARSER_H
#define _ARGS_PARSER_H

#include <string>

namespace redb::server {
    class ArgsParser {
    public:
        ArgsParser(int argc, char **argv);

        unsigned short get_port() const {
            return port;
        }

        const std::string &get_db_seed_path() const {
            return db_seed_path;
        }

    private:
        unsigned short port;
        std::string db_seed_path;
    };
}

#endif //_ARGS_PARSER_H
