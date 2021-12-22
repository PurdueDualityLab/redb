//
// Created by charlie on 9/6/21.
//

#include <unordered_set>
#include <memory>
#include <query/cluster_match_query.h>
#include <gtest/gtest.h>

TEST(ClusterMatchQuery, Query) {
    std::unordered_set<std::string> positive = {"abc", "ABC", "asdfkjslkjdflasdf"};
    std::unordered_set<std::string> negative = {"123", "67976", "4574573473"};
    auto query = std::make_shared<rereuse::query::ClusterMatchQuery>(positive, negative);

    auto cluster = std::make_shared<rereuse::db::Cluster>();
    cluster->add_pattern("[a-z]+");
    cluster->add_pattern("[a-zA-Z]+");
    cluster->add_pattern("[0-9]*");
    cluster->compile();

    std::unordered_set<std::string> results = query->query(cluster);

    EXPECT_THAT(results, testing::ElementsAre("[a-zA-Z]+"));
}
