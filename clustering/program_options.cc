//
// Created by charlie on 2/15/22.
//

#include "program_options.h"
#include <ostream>

ProgramOptions ProgramOptions::global_options_instance;

std::ostream &operator<<(std::ostream &os, const ProgramOptions &opts) {
    os << "OPTIONS\n";
    os << "Inflation: " << opts.inflation << '\n';
    os << "Pruning: " << opts.pruning << '\n';
    os << "Graph Out: ";
    if (opts.graph_out)
        os << *opts.graph_out;
    else
        os << "<none>";
    os << '\n';
    os << "Cluster Out: ";
    if (opts.cluster_out)
        os << *opts.cluster_out;
    else
        os << "<none>";
    os << '\n';
    os << "Patterns file out: ";
    if (opts.patterns_file_out)
        os << *opts.patterns_file_out;
    else
        os << "<none>";
    os << '\n';
    os << "Corpus file: " << opts.corpus_type << '\n';
    os << "Workers: " << opts.workers << '\n';
    os << "Corpus type: ";
    if (opts.corpus_type == CorpusType::CLUSTERS)
        os << "CLUSTERS";
    else if (opts.corpus_type == CorpusType::PAIRS)
        os << "PAIRS";
    else
        os << "OBJECTS";
    os << '\n';
    os << "Strict rex string checking: " << opts.strict_rex_string_checking << '\n';

    return os;
}
