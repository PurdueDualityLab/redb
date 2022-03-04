//
// Created by charlie on 3/2/22.
//

#include "db/pattern_reader.h"
#include "query/cluster_match_query.h"

#include "program_args.h"
#include "db/regex_cluster_repository.h"
#include "util/time_task.h"
#include "util/stats.h"

#include <iostream>
#include <fstream>

template <typename DurationT>
static double average_times(const std::vector<DurationT> &durations) {
    // Map vector into counts
    std::vector<unsigned long> counts;
    std::transform(durations.cbegin(), durations.cend(), std::back_inserter(counts), [](const DurationT &duration) { return duration.count(); });

    // Remove any outliers because the first execution of re2 is super long, so test will always be much longer
    // than query because test always happens first
    rereuse::util::remove_outliers(counts);

    return rereuse::util::mean(counts.begin(), counts.end());
}

static void display_results(std::ostream &output, unsigned long report_id, size_t results, unsigned long duration,
                            int skipped, double average_test, double average_query) {
    output << "Report " << report_id << ":\n";
    output << "Num results: " << results << '\n';
    output << "Total duration: " << duration << "ms\n";
    output << "Skipped clusters: " << skipped << '\n';
    output << "Average test time: " << average_test << "us\n";
    output << "Average query time: " << average_query << "us\n";
    output << std::endl;
}

int main(int argc, char **argv) {

    // Parse args
    ProgramArgs args;
    try {
        ProgramArgs parsed_args(argc, argv);
        args = std::move(parsed_args);
    } catch (std::runtime_error &exe) {
        std::cerr << "Error while parsing args: " << exe.what() << std::endl;
        std::cerr << "Consult -h for help" << std::endl;
        return 1;
    }

    // Create a database out of a cluster file
    auto semantic_clusters = rereuse::db::read_semantic_clusters(args.clusters_file);
    rereuse::db::RegexClusterRepository repo;
    for (auto &cluster : semantic_clusters)
        repo.add_cluster(std::move(cluster));

    // Create a query out of the query def
    auto cluster_query_def = rereuse::query::ClusterMatchQuery::from_file(args.query_file);
    auto cluster_query = std::shared_ptr<rereuse::query::BaseClusterQuery>(new rereuse::query::ClusterMatchQuery(std::move(cluster_query_def)));

    // Run the query
    std::function<std::unordered_set<std::string>(int*, std::vector<std::chrono::microseconds>*, std::vector<std::chrono::microseconds>*)>
            query_task = [&repo, &cluster_query](int *skipped, std::vector<std::chrono::microseconds> *test, std::vector<std::chrono::microseconds> *query) {
        return repo.query(cluster_query, skipped, test, query);
    };

    if (args.output_file) {
        std::ofstream output_file(*args.output_file);
    }

    // Measure how long it took to evaluate the query...
    // ...and how many hits
    for (unsigned int rep = 0; rep < args.repetitions; rep++) {
        int skipped = 0;
        std::vector<std::chrono::microseconds> test_times, query_times;
        auto[results, duration] = rereuse::util::time_task<std::chrono::milliseconds>(query_task, &skipped, &test_times, &query_times);

        double average_test_time = average_times(test_times);
        double average_query_time = average_times(query_times);

        // Report measurements to user, optionally write to file
        display_results(std::cout, rep, results.size(), duration.count(), skipped, average_test_time, average_query_time);
    }

    return 0;
}
