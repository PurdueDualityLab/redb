//
// Created by charlie on 9/14/21.
//

#include "cluster_experiment.h"

#include <fstream>
#include <random>
#include <string>
#include <unordered_set>

#include <nlohmann/json.hpp>

#include <db/regex_cluster_repository.h>

static std::unordered_set<std::string> read_patterns_from_results_file(const std::string &path) {
    std::ifstream file(path);
    nlohmann::json file_obj;
    file >> file_obj; // Read the file ig

    std::unordered_set<std::string> patterns;
    auto pattern_objs_list = file_obj["regexes"];
    for (const auto &pattern_obj : pattern_objs_list) {
        patterns.insert(pattern_obj["pattern"].get<std::string>());
    }

    return patterns;
}

void perform_cluster_experiment() {
    int cluster_size = 50;
    std::unordered_set<std::string> email_patterns = read_patterns_from_results_file("/home/charlie/Programming/redb/loose-email.json");
    std::unordered_set<std::string> phone_patterns = read_patterns_from_results_file("/home/charlie/Programming/redb/loose-phone.json");

    // put everything together into a vector and shuffle the vector
    std::vector<std::string> combined_patterns;
    std::unordered_set<std::string> combined_patterns_set;
    auto combined_patterns_inserter = std::back_inserter(combined_patterns);
    for (auto &patterns_set : { email_patterns, phone_patterns }) {
        // This is causing a segfault
        std::copy(patterns_set.cbegin(), patterns_set.cend(), combined_patterns_inserter);
    }

    std::copy(combined_patterns.begin(), combined_patterns.end(), std::inserter(combined_patterns_set, combined_patterns_set.begin()));
    rereuse::db::RegexClusterRepository semantic_clusters(cluster_size, combined_patterns_set);
    semantic_clusters.shuffle_clusters();

    std::shuffle(combined_patterns.begin(), combined_patterns.end(), std::mt19937(std::random_device()()));
    combined_patterns_set.clear();
    std::copy(combined_patterns.begin(), combined_patterns.end(), std::inserter(combined_patterns_set, combined_patterns_set.begin()));


    rereuse::db::RegexClusterRepository random_clusters(cluster_size, combined_patterns_set);
    random_clusters.shuffle_clusters();

    // have semantic and random clusters. Time the queries against them
    std::cout << "Loaded " << semantic_clusters.pattern_count() << " patterns in " << semantic_clusters.cluster_count() << " semantic clusters" << std::endl;
    std::cout << "Loaded " << random_clusters.pattern_count() << " patterns in " << random_clusters.cluster_count() << " random clusters" << std::endl;

    // Email query
    std::unordered_set<std::string> positive = {"name@domain.com", "###/+-?^_`{|}~$$$***@weird.do", "1.2.3.4@crazy.domain.axes", "!@B.gone"};
    std::unordered_set<std::string> negative = {"@tweetybirdHandle", "www.website.com", "oneWord", "Look at that lightning storm - it's getting closer!"};

    auto query = std::make_shared<rereuse::query::ClusterMatchQuery>(positive, negative);

    for (int i = 0; i < 5; i++) {
        std::cout << "Iteration " << i << std::endl;
        std::cout << "Running semantic..." << std::endl;
        auto semantic_start = std::chrono::high_resolution_clock::now();
        int semantic_skipped = 0;
        auto semantic_results = semantic_clusters.query(query, &semantic_skipped);
        auto semantic_end = std::chrono::high_resolution_clock::now();
        auto semantic_duration = std::chrono::duration_cast<std::chrono::microseconds>(semantic_end - semantic_start);

        std::cout << "Semantic took " << semantic_duration.count() << "ms" << std::endl;
        std::cout << "Semantic skipped " << semantic_skipped << " clusters" << std::endl;
    }

    for (int i = 0; i < 5; i++) {
        std::cout << "Iteration " << i << std::endl;
        std::cout << "Running random..." << std::endl;
        auto random_start = std::chrono::high_resolution_clock::now();
        int random_skipped = 0;
        auto random_results = random_clusters.query(query, &random_skipped);
        auto random_end = std::chrono::high_resolution_clock::now();
        auto random_duration = std::chrono::duration_cast<std::chrono::microseconds>(random_end - random_start);

        std::cout << "Random took " << random_duration.count() << "ms" << std::endl;
        std::cout << "Random skipped " << random_skipped << " clusters" << std::endl;
    }
}
