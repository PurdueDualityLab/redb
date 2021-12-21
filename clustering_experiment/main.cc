//
// Created by charlie on 8/30/21.
//

#include <iostream>

#include <nlohmann/json.hpp>

#include "cluster_experiment.h"
#include "semantic_clustering.h"

int main(int argc, char** argv) {

    std::cout << "Running experiment" << std::endl;
    // perform_cluster_experiment();
    perform_semantic_clustering();
    std::cout << "Complete" << std::endl;

    return 0;
}
