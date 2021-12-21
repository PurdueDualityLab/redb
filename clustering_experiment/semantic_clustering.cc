//
// Created by charlie on 10/23/21.
//

#include <fstream>
#include <memory>
#include <random>

#include <nlohmann/json.hpp>
#include <db/cluster.h>
#include <db/regex_cluster_repository.h>

#include "benchmarking.h"
#include <util/stats.h>

std::vector<std::unique_ptr<rereuse::db::Cluster>> read_semantic_clusters(const std::string &file_path) {
    std::ifstream seed_file(file_path);
    nlohmann::json cluster_array;
    if (seed_file.is_open()) {
        seed_file >> cluster_array;
        seed_file.close();
    } else {
        return {};
    }

    std::vector<std::unique_ptr<rereuse::db::Cluster>> clusters;

    // Iterate over clusters
    for (const auto &cluster_patterns : cluster_array) {
        std::unordered_set<std::string> patterns;
        for (const auto &cluster_pattern : cluster_patterns) {
            patterns.insert(cluster_pattern.get<std::string>());
        }

        auto cluster = std::make_unique<rereuse::db::Cluster>(patterns);
        clusters.push_back(std::move(cluster));
    }

    return clusters;
}

std::vector<std::unique_ptr<rereuse::db::Cluster>> randomize_clusters(const std::vector<std::unique_ptr<rereuse::db::Cluster>> &semantic_clusters) {
    // Combine all patterns into one list
    std::vector<std::string> all_patterns;
    for (const auto &cluster : semantic_clusters) {
        for (const auto &pattern : cluster->get_patterns()) {
            all_patterns.push_back(pattern);
        }
    }

    // Make a list of clusters
    std::vector<std::unique_ptr<rereuse::db::Cluster>> randomized_clusters(semantic_clusters.size());
    for (auto& randomized_cluster : randomized_clusters) {
        randomized_cluster = std::make_unique<rereuse::db::Cluster>();
    }

    // Add the patterns to the clusters
    int i = 0;
    for (const auto &pattern : all_patterns) {
        randomized_clusters[i++ % randomized_clusters.size()]->add_pattern(pattern);
    }

    return randomized_clusters;
}

std::vector<std::shared_ptr<rereuse::query::ClusterMatchQuery>> read_queries(const std::string &queries_file_path) {
    std::ifstream queries_file(queries_file_path);
    if (!queries_file.is_open())
        return {};

    nlohmann::json queries_list;
    queries_file >> queries_list;
    queries_file.close();

    std::vector<std::shared_ptr<rereuse::query::ClusterMatchQuery>> queries;

    for (const auto &query : queries_list) {
        auto positive_examples = query["positive"].get<std::unordered_set<std::string>>();
        auto negative_examples = query["negative"].get<std::unordered_set<std::string>>();
        queries.push_back(std::make_shared<rereuse::query::ClusterMatchQuery>(positive_examples, negative_examples));
    }

    return queries;
}

void display_query_results(const QueryResults &results, const std::string &label) {
    std::cout << "Results from " << label << ":" << std::endl;
    std::cout << "Query time: " << std::chrono::duration_cast<std::chrono::milliseconds>(results.total_time).count() << "ms" << std::endl;
    std::cout << "Query result count: " << results.query_results.size() << std::endl;
    std::cout << "Skipped clusters: " << results.skipped_clusters << std::endl;
    std::vector<long> test_times;
    std::transform(results.test_times.cbegin(),  results.test_times.cend(), std::back_inserter(test_times),
                   [&](const std::chrono::microseconds &duration) { return duration.count(); });
    std::vector<long> query_times;
    std::transform(results.query_times.cbegin(),  results.query_times.cend(), std::back_inserter(query_times),
                   [&](const std::chrono::microseconds &duration){ return duration.count(); });
    rereuse::util::remove_outliers(test_times);
    rereuse::util::remove_outliers(query_times);

    std::cout << "Average test time (w/o outliers): " << rereuse::util::mean(test_times.cbegin(),  test_times.cend()) << "us" << std::endl;
    std::cout << "Average query time (w/o outliers): " << rereuse::util::mean(query_times.cbegin(),  query_times.cend()) << "us" << std::endl;
}

void display_verbose_info(const std::vector<QueryResults> &semantic_results, const std::vector<QueryResults> &random_results) {
    std::vector<unsigned long> semantic_times;
    std::transform(semantic_results.cbegin(), semantic_results.cend(), std::back_inserter(semantic_times),
                   [&](const QueryResults &result) { return result.total_time.count() / 1000; });
    std::cout << "Semantic times (ms): [";
    for (const auto &time : semantic_times) {
        std::cout << time << ", ";
    }
    std::cout << "]" << std::endl;

    std::vector<unsigned long> random_times;
    std::transform(random_results.cbegin(), random_results.cend(), std::back_inserter(random_times),
                   [&](const QueryResults &result) { return result.total_time.count() / 1000; });
    std::cout << "Random times (ms): [";
    for (const auto &time : random_times) {
        std::cout << time << ", ";
    }
    std::cout << "]" << std::endl;

    std::cout << "-----------------------------" << std::endl;
    unsigned long queries = semantic_results.size() < random_results.size() ? semantic_results.size() : random_results.size();
    for (unsigned long i = 0; i < queries; i++) {
        std::cout << "---------Info for query " << i + 1 << "--------" << std::endl;
        auto semantic = semantic_results[i];
        auto random = random_results[i];

        std::vector<long> semantic_test_times;
        std::transform(semantic.test_times.cbegin(),  semantic.test_times.cend(), std::back_inserter(semantic_test_times),
                       [&](const auto &duration) { return duration.count(); });
        std::cout << "Semantic test times: [";
        for (const auto &time : semantic_test_times) {
            std::cout << time << ", ";
        }
        std::cout << ']' << std::endl;

        std::vector<long> random_test_times;
        std::transform(random.test_times.cbegin(),  random.test_times.cend(), std::back_inserter(random_test_times),
                       [&](const auto &duration) { return duration.count(); });
        std::cout << "Random test times: [";
        for (const auto &time : random_test_times) {
            std::cout << time << ", ";
        }
        std::cout << ']' << std::endl;

        std::vector<long> semantic_query_times;
        std::transform(semantic.query_times.cbegin(),  semantic.query_times.cend(), std::back_inserter(semantic_query_times),
                       [&](const auto &duration) { return duration.count(); });
        std::cout << "Semantic query times: [";
        for (const auto &time : semantic_query_times) {
            std::cout << time << ", ";
        }
        std::cout << ']' << std::endl;

        std::vector<long> random_query_times;
        std::transform(random.query_times.cbegin(),  random.query_times.cend(), std::back_inserter(random_query_times),
                       [&](const auto &duration) { return duration.count(); });
        std::cout << "Random query times: [";
        for (const auto &time : random_query_times) {
            std::cout << time << ", ";
        }
        std::cout << ']' << std::endl;
    }
}

void perform_semantic_clustering() {
    auto path = "/home/charlie/Programming/regex-reuse/data/clusterdb-v4.json";
    auto clusters = read_semantic_clusters(path);
    auto randomized_clusters = randomize_clusters(clusters);
    rereuse::db::RegexClusterRepository semantic_repo;
    rereuse::db::RegexClusterRepository random_repo;
    int clusters_added = 0;
    for (auto &cluster : clusters) {
        bool added = semantic_repo.add_cluster(std::move(cluster));
        if (!added) {
            std::cerr << "Failed to add cluster " << ++clusters_added << " to semantic database. Compiler ran out of memory" << std::endl;
            exit(1);
        } else {
            std::cout << "Added " << ++clusters_added << " cluster(s) to semantic" << std::endl;
        }
    }

    clusters_added = 0;
    for (auto &cluster : randomized_clusters) {
        bool added = random_repo.add_cluster(std::move(cluster));
        if (!added) {
            std::cerr << "Failed to add cluster " << ++clusters_added << " to random database. Compiler ran out of memory" << std::endl;
            exit(1);
        } else {
            std::cout << "Added " << ++clusters_added << " cluster(s) to random" << std::endl;
        }
    }

    std::cout << "semantic cluster: loaded " << semantic_repo.pattern_count() << " patterns into " << semantic_repo.cluster_count() << " clusters" << std::endl;
    std::cout << "random cluster: loaded " << random_repo.pattern_count() << " patterns into " << random_repo.cluster_count() << " clusters" << std::endl;

    auto queries = read_queries("/home/charlie/Programming/regex-reuse/data/cluster-queries-v4.1.json");
    std::vector<QueryResults> semantic_results_list;
    std::vector<QueryResults> random_results_list;

    int i = 0;
    for (const auto &query : queries) {
        std::cout << "--------Running query " << ++i << "-----------" << std::endl;
        std::cout << "Query has " << query->positive_examples_count() << " positive examples and " << query->negative_examples_count() << " negative examples" << std::endl;
        auto semantic_results = time_cluster_query(semantic_repo, query);
        auto random_results = time_cluster_query(random_repo, query);

        display_query_results(semantic_results, "semantic");
        display_query_results(random_results, "random");

        semantic_results_list.push_back(std::move(semantic_results));
        random_results_list.push_back(std::move(random_results));
        std::cout << "----------------------------------" << std::endl;
    }

    std::cout << "--------------------------------------" << std::endl;
    std::vector<double> semantic_times, random_times;
    std::transform(semantic_results_list.cbegin(),  semantic_results_list.cend(), std::back_inserter(semantic_times),
                   [&](const QueryResults& results){ return results.total_time.count(); });
    std::transform(random_results_list.cbegin(),  random_results_list.cend(), std::back_inserter(random_times),
                   [&](const QueryResults& results){ return results.total_time.count(); });
    double average_semantic_time = rereuse::util::mean(semantic_times.cbegin(),  semantic_times.cend());
    // double semantic_time_stddev = rereuse::util::stddev(semantic_times.cbegin(),  semantic_times.cend());
    double average_random_time = rereuse::util::mean(random_times.cbegin(),  random_times.cend());
    // double random_time_stddev = rereuse::util::stddev(random_times.cbegin(),  random_times.cend());
    std::cout << "Average semantic time: " << average_semantic_time / 1000 << " ms" << std::endl;
    std::cout << "Average random time: " << average_random_time / 1000 << " ms" << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    std::vector<std::chrono::microseconds> semantic_test_times, random_test_times;
    for (const auto &result : semantic_results_list) {
        semantic_test_times.insert(semantic_test_times.end(), result.test_times.cbegin(),  result.test_times.cend());
    }
    for (const auto &result : random_results_list) {
        random_test_times.insert(random_test_times.end(), result.test_times.cbegin(),  result.test_times.cend());
    }

    std::vector<long> semantic_test_times_count, random_test_times_count;
    for (const auto &duration : semantic_test_times) {
        semantic_test_times_count.push_back(duration.count());
    }
    for (const auto &duration : random_test_times) {
        random_test_times_count.push_back(duration.count());
    }

    rereuse::util::remove_outliers(semantic_test_times_count);
    rereuse::util::remove_outliers(random_test_times_count);

    std::cout << "Average semantic test time: " << rereuse::util::mean(semantic_test_times_count.cbegin(),  semantic_test_times_count.cend()) / 1000 << " ms" << std::endl;
    std::cout << "Average random test time: " << rereuse::util::mean(random_test_times_count.cbegin(),  random_test_times_count.cend()) / 1000 << " ms" << std::endl;

    std::cout << "--------------------------------------" << std::endl;

    std::vector<std::chrono::microseconds> semantic_query_times, random_query_times;
    for (const auto &result : semantic_results_list) {
        semantic_query_times.insert(semantic_query_times.end(), result.query_times.cbegin(),  result.query_times.cend());
    }
    for (const auto &result : random_results_list) {
        random_query_times.insert(random_query_times.end(), result.query_times.cbegin(),  result.query_times.cend());
    }

    std::vector<long> semantic_query_times_count, random_query_times_count;
    for (const auto &duration : semantic_query_times) {
        semantic_query_times_count.push_back(duration.count());
    }
    for (const auto &duration : random_query_times) {
        random_query_times_count.push_back(duration.count());
    }

    rereuse::util::remove_outliers(semantic_query_times_count);
    rereuse::util::remove_outliers(random_query_times_count);

    std::cout << "Average semantic query time: " << rereuse::util::mean(semantic_query_times_count.cbegin(),  semantic_query_times_count.cend()) / 1000 << " ms" << std::endl;
    std::cout << "Average random query time: " << rereuse::util::mean(random_query_times_count.cbegin(),  random_query_times_count.cend()) / 1000 << " ms" << std::endl;

    std::cout << "--------------------------------------" << std::endl;

    std::vector<int> semantic_skipped, random_skipped;
    for (const auto &result : semantic_results_list) {
        semantic_skipped.push_back(result.skipped_clusters);
    }
    for (const auto &result : random_results_list) {
        random_skipped.push_back(result.skipped_clusters);
    }

    std::cout << "Average semantic clusters skipped: " << rereuse::util::mean(semantic_skipped.cbegin(), semantic_skipped.cend()) << std::endl;
    std::cout << "Average random clusters skipped: " << rereuse::util::mean(random_skipped.cbegin(),  random_skipped.cend()) << std::endl;

    std::cout << "--------------------------------------" << std::endl;

    if (false) {
        display_verbose_info(semantic_results_list, random_results_list);
    }
}
