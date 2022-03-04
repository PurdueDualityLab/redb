//
// Created by charlie on 3/2/22.
//

#ifndef REDB_PROGRAM_ARGS_H
#define REDB_PROGRAM_ARGS_H

#include <string>
#include <optional>

struct ProgramArgs {
    ProgramArgs() = default;
    ProgramArgs(int argc, char **argv);

    static void print_help() noexcept;

    std::string clusters_file;
    std::string query_file;
    std::optional<std::string> output_file;
    unsigned int repetitions;
};


#endif //REDB_PROGRAM_ARGS_H
