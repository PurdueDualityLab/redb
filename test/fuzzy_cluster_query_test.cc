//
// Created by charlie on 9/7/21.
//

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <db/cluster.h>
#include <query/base_cluster_query.h>
#include <query/fuzzy_cluster_query.h>

TEST(FuzzyClusterQueryTest, NegatesCluster) {
    std::unordered_set<std::string> positive = {"abc", "ABC", "asdfjasdlfjasdf"};
    std::unordered_set<std::string> negative = {"123", "456"};

    rereuse::query::FuzzyClusterQuery cluster_query(positive, negative);

    auto cluster = std::make_shared<rereuse::db::Cluster>();
    cluster->add_pattern("123");
    cluster->add_pattern("[0-9]+");
    cluster->compile();

    auto results = cluster_query.query(cluster);

    EXPECT_EQ(results.size(), 0);
}

TEST(FuzzyClusterQueryTest, MaybeCluster) {
    std::unordered_set<std::string> positive = {"abc", "ABC", "asdfjasdlfjasdf"};
    std::unordered_set<std::string> negative = {"123", "456"};

    rereuse::query::FuzzyClusterQuery cluster_query(positive, negative);

    auto cluster = std::make_shared<rereuse::db::Cluster>();
    cluster->add_pattern("123");
    cluster->add_pattern("[0-9]+");
    cluster->add_pattern("[a-zA-Z]+");
    cluster->compile();

    auto results = cluster_query.query(cluster);

    EXPECT_EQ(results.size(), 1);
}
