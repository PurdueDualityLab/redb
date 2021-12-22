//
// Created by charlie on 12/22/21.
//

#include <gtest/gtest.h>

#include <memory>
#include <db/parallel_regex_cluster_repository.h>

TEST(ParallelRegexClusterRepositoryTest, work_properly) {
    rereuse::db::ParallelRegexClusterRepository repo(2);
    repo.add_cluster(std::make_unique<rereuse::db::Cluster>(std::unordered_set<std::string> { "[a-z]+", "abc" }));
    auto results = repo.query(std::make_shared<rereuse::query::ClusterMatchQuery>(std::unordered_set<std::string> {"abc"}, std::unordered_set<std::string> {"012"}));

    EXPECT_EQ(results.size(), 2);
}
