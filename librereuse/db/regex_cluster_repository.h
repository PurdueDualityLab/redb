//
// Created by charlie on 8/31/21.
//

#ifndef _REGEXCLUSTERREPOSITORY_H
#define _REGEXCLUSTERREPOSITORY_H

#include <iostream>
#include <memory>
#include <vector>
#include "cluster.h"
#include "../query/base_regex_query.h"
#include "../query/cluster_match_query.h"

namespace rereuse::db {
    /**
     * Represents a clustered regex repository
     */
    class RegexClusterRepository {
    public:
        explicit RegexClusterRepository();
        explicit RegexClusterRepository(int maxClusterSize, const std::unordered_set<std::string> &patterns);
        explicit RegexClusterRepository(int maxClusterSize, const std::string& path);

        int cluster_count() const { return this->clusters.size(); }
        int pattern_count() const;
        bool add_cluster(std::unique_ptr<Cluster> cluster);
        void shuffle_clusters();

        const std::vector<std::shared_ptr<Cluster>> &get_clusters();

        std::unordered_set<std::string>
        query(const std::shared_ptr<rereuse::query::BaseClusterQuery> &query, int *skipped_clusters = nullptr,
              std::vector<std::chrono::microseconds> *test_times = nullptr,
              std::vector<std::chrono::microseconds> *query_times = nullptr) const;

    private:
        int maxClusterSize;
        std::vector<std::shared_ptr<Cluster>> clusters;
    };
}

#endif //_REGEXCLUSTERREPOSITORY_H
