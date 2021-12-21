//
// Created by charlie on 9/13/21.
//

#include "set_comparison.h"

#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>

#include <query/match_query.h>
#include <query/cluster_match_query.h>
#include <query/fuzzy_cluster_query.h>
#include <db/regex_cluster_repository.h>
#include <db/regex_repository.h>

void run_set_comparison_benchmarking() {
    std::unordered_set<std::string> positive = {"abc", "dfasdf", "ABC"};
    std::unordered_set<std::string> negative = {"123", "68765435", "2"};
    auto *match_query = new rereuse::query::MatchQuery(positive, negative);
    auto cluster_match_query = new rereuse::query::ClusterMatchQuery(positive, negative);
    auto fuzzy_match_query = new rereuse::query::FuzzyClusterQuery(positive, negative);

    std::shared_ptr<rereuse::query::BaseRegexQuery> query(match_query);
    std::shared_ptr<rereuse::query::BaseClusterQuery> cluster_query(cluster_match_query);
    std::shared_ptr<rereuse::query::BaseClusterQuery> fuzzy_query(fuzzy_match_query);

    std::vector<long> single_times;
    std::vector<long> cluster1_times;
    std::vector<long> fuzzy1_times;
    std::vector<double> cluster1_skips_percentage;
    std::vector<long> cluster10_times;
    std::vector<long> fuzzy10_times;
    std::vector<double> cluster10_skips_percentage;

    std::filesystem::directory_iterator samples_iter("/home/charlie/Programming/redb/samples/20000");
    for (const auto &sample_db : samples_iter) {
        rereuse::db::RegexRepository single_repo(sample_db.path());
        rereuse::db::RegexClusterRepository cluster_repo_1pct(10, sample_db.path());
        rereuse::db::RegexClusterRepository cluster_repo_10pct( 100, sample_db.path());

#ifdef BENCH_SINGLE
        auto single_query_start = std::chrono::high_resolution_clock::now();
        std::unordered_set<std::string> results = single_repo.query(query);
        auto single_query_end = std::chrono::high_resolution_clock::now();
        auto single_query_duration = std::chrono::duration_cast<std::chrono::milliseconds>(single_query_end - single_query_start);
        std::cout << "Found " << results.size() << " results in " << single_query_duration.count() << "ms" << std::endl;
        single_times.push_back(single_query_duration.count());
        for (const auto &pattern : results) {
            std::cout << "\t/" << pattern << "/" << std::endl;
        }
#endif

        auto cluster1_query_start = std::chrono::high_resolution_clock::now();
        int skips = 0;
        std::unordered_set<std::string> cluster_results = cluster_repo_1pct.query(cluster_query, &skips);
        auto cluster1_query_end = std::chrono::high_resolution_clock::now();
        auto cluster1_query_duration = std::chrono::duration_cast<std::chrono::milliseconds>(cluster1_query_end - cluster1_query_start);
        std::cout << "Found " << cluster_results.size() << " results in " << cluster1_query_duration.count() << "ms" << std::endl;
        cluster1_times.push_back(cluster1_query_duration.count());
        cluster1_skips_percentage.push_back((double) skips / cluster_repo_1pct.cluster_count());
        for (const auto &pattern : cluster_results) {
            std::cout << "\t/" << pattern << "/" << std::endl;
        }

        auto cluster10_query_start = std::chrono::high_resolution_clock::now();
        int skips10 = 0;
        std::unordered_set<std::string> cluster_10_results = cluster_repo_10pct.query(cluster_query, &skips10);
        auto cluster10_query_end = std::chrono::high_resolution_clock::now();
        auto cluster10_query_duration = std::chrono::duration_cast<std::chrono::milliseconds>(cluster10_query_end - cluster10_query_start);
        std::cout << "Found " << cluster_10_results.size() << " results in " << cluster10_query_duration.count() << "ms" << std::endl;
        cluster10_times.push_back(cluster10_query_duration.count());
        cluster10_skips_percentage.push_back((double) skips10 / cluster_repo_10pct.cluster_count());
        for (const auto &pattern : cluster_results) {
            std::cout << "\t/" << pattern << "/" << std::endl;
        }
#ifdef BENCH_FUZZY
        auto fuzzy1_query_start = std::chrono::high_resolution_clock::now();
        std::unordered_set<std::string> fuzzy1_results = cluster_repo_1pct.query(fuzzy_query, nullptr);
        auto fuzzy1_query_end = std::chrono::high_resolution_clock::now();
        auto fuzzy1_query_duration = std::chrono::duration_cast<std::chrono::microseconds>(fuzzy1_query_end - fuzzy1_query_start);
        std::cout << "Found " << fuzzy1_results.size() << " results in " << fuzzy1_query_duration.count() << "us" << std::endl;
        fuzzy1_times.push_back(fuzzy1_query_duration.count());

        auto fuzzy10_query_start = std::chrono::high_resolution_clock::now();
        std::unordered_set<std::string> fuzzy10_results = cluster_repo_10pct.query(fuzzy_query, nullptr);
        auto fuzzy10_query_end = std::chrono::high_resolution_clock::now();
        auto fuzzy10_query_duration = std::chrono::duration_cast<std::chrono::microseconds>(fuzzy10_query_end - fuzzy10_query_start);
        std::cout << "Found " << fuzzy10_results.size() << " results in " << fuzzy10_query_duration.count() << "us" << std::endl;
        fuzzy10_times.push_back(fuzzy10_query_duration.count());
#endif
    }

#ifdef BENCH_SINGLE
    std::cout << "Single pattern search times:" << std::endl;
    for (const auto &time : single_times) {
        std::cout << time << std::endl;
    }
    std::cout << std::endl;
#endif

    std::cout << "1% cluster search times:" << std::endl;
    for (const auto &time : cluster1_times) {
        std::cout << time << std::endl;
    }
    std::cout << std::endl;

    std::cout << "1% cluster skip counts:" << std::endl;
    for (const auto &skip : cluster1_skips_percentage) {
        std::cout << skip << std::endl;
    }
    std::cout << std::endl;

#ifdef BENCH_FUZZY
    std::cout << "1% fuzzy search times:" << std::endl;
    for (const auto &time : fuzzy1_times) {
        std::cout << time << std::endl;
    }
    std::cout << std::endl;
#endif

    std::cout << "10% cluster search times:" << std::endl;
    for (const auto &time : cluster10_times) {
        std::cout << time << std::endl;
    }
    std::cout << std::endl;

    std::cout << "10% cluster skip counts:" << std::endl;
    for (const auto &skip : cluster10_skips_percentage) {
        std::cout << skip << std::endl;
    }
    std::cout << std::endl;

#ifdef BENCH_FUZZY
    std::cout << "10% fuzzy search times:" << std::endl;
    for (const auto &time : fuzzy10_times) {
        std::cout << time << std::endl;
    }
    std::cout << std::endl;
#endif

}
