//
// Created by charlie on 2/10/22.
//

#include "mcl_wrapper.h"

#include <iostream>
#include <utility>
#include <sstream>
#include <fstream>
#include <re2/re2.h>

MclWrapper::~MclWrapper() {
    if (this->has_temp_cluster_file)
        remove(this->temp_cluster_file.c_str());
}

std::vector<std::vector<unsigned long>>
MclWrapper::cluster(const std::string &abc_file, double inflation, double pruning) {
    this->temp_cluster_file = "/tmp/clusters_" + std::to_string(random()) + ".txt";
    this->has_temp_cluster_file = true;
    return this->cluster(abc_file, inflation, pruning, this->temp_cluster_file);
}

std::vector<std::vector<unsigned long>>
MclWrapper::cluster(const std::string &abc_file, double inflation, double pruning, const std::string &clusters_output_file) const {
    std::vector<std::string> cmd_fragments = {this->mcl_path, abc_file, "--abc", "-I", std::to_string(inflation)};
    // Conditionally add pruning
    if (pruning > 0) {
        cmd_fragments.emplace_back("-p");
        cmd_fragments.push_back(std::to_string(pruning));
    }
    // Add output file
    cmd_fragments.emplace_back("-o");
    cmd_fragments.push_back(clusters_output_file);

    // join cmd_fragments
    std::stringstream cmd;
    cmd.precision(3); // 0.00 fixed
    auto it = cmd_fragments.begin();
    for (; it != cmd_fragments.end() - 1; ++it) {
        cmd << *it << ' ';
    }
    cmd << *(it);

    // Execute the program
    std::cout << "Executing mcl: " << cmd.str() << std::endl;
    int ret = system(cmd.str().c_str());
    if (ret != 0)
        throw std::runtime_error("Failed to execute mcl. Exit with return value: " + std::to_string(ret));

    // Read the clusters from the output file
    std::ifstream clusters_output(clusters_output_file);
    std::vector<std::vector<unsigned long>> clusters;
    std::string line;
    re2::RE2 cluster_line_parser("(\\d+)\\s*");
    while (std::getline(clusters_output, line)) {
        // split the line
        std::vector<unsigned long> cluster;

        re2::StringPiece line_piece(line);
        unsigned long id;
        while (re2::RE2::FindAndConsume(&line_piece, cluster_line_parser, &id)) {
            cluster.push_back(id);
        }

        clusters.push_back(std::move(cluster));
    }

    return clusters;
}
