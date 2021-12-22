//
// Created by charlie on 11/17/21.
//

#include <util/stats.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(StatsMean, work_for_int_vector) {
    std::vector<int> nums = {1, 2, 3, 4, 5};
    double avg = rereuse::util::mean(nums.cbegin(), nums.cend(), nums.size());
    EXPECT_EQ(avg, 3);
}

TEST(StatsMean, work_for_int_double) {
    std::vector<double> nums = {8.4, 9.3, 1.2, 100.0, 88.2};
    double avg = rereuse::util::mean(nums.cbegin(), nums.cend(), nums.size());
    EXPECT_EQ(avg, 41.42);
}

TEST(StatsStdDev, work_for_int) {
    std::vector<int> nums = {1, 2, 3, 4, 5};
    double stddev = rereuse::util::stddev(nums.cbegin(), nums.cend(), nums.size());
    EXPECT_DOUBLE_EQ(stddev, 1.4142135623730951);
}

TEST(StatsMedian, work_for_odd_int) {
    std::vector<int> nums = {0, 1, 2, 3, 4, 5, 6};
    double median = rereuse::util::median(nums.cbegin(), nums.cend());
    EXPECT_EQ(median, 3);
}

TEST(StatsMedian, work_for_even_int) {
    std::vector<int> nums = {0, 1, 2, 3, 4, 5};
    double median = rereuse::util::median(nums.cbegin(), nums.cend());
    EXPECT_EQ(median, 2.5);
}

TEST(StatsQuartiles, work_for_odd_int) {
    std::vector<int> nums = {0, 1, 2, 3, 4, 5, 6};
    auto quarts = rereuse::util::quartiles(nums.cbegin(), nums.cend());
    EXPECT_EQ(quarts.first, 1);
    EXPECT_EQ(quarts.second, 5);
}
TEST(StatsQuartiles, work_for_even_int) {
    std::vector<int> nums = {0, 1, 2, 3, 4, 5};
    auto quarts = rereuse::util::quartiles(nums.cbegin(), nums.cend());
    EXPECT_EQ(quarts.first, 1);
    EXPECT_EQ(quarts.second, 4);
}

TEST(StatsOutliers, no_outliers) {
    std::vector<int> nums = {0, 1, 2, 3, 4, 5};
    auto outliers = rereuse::util::outliers(nums.cbegin(),  nums.cend());
    EXPECT_TRUE(outliers.empty());
}

TEST(StatsOutliers, has_outlier) {
    std::vector<int> nums = {0, 1, 2, 3, 4, 5, 1257275983};
    auto outliers = rereuse::util::outliers(nums.cbegin(),  nums.cend());
    EXPECT_GT(std::count(outliers.cbegin(), outliers.cend(), 1257275983), 0);
}

TEST(StatsRemovesOutliers, dont_remove_outliers) {
    std::vector<int> nums = {0, 1, 2, 3, 4, 5};
    rereuse::util::remove_outliers(nums);
    EXPECT_EQ(nums.size(), 6);
}

TEST(StatsRemovesOutliers, remove_outlier) {
    std::vector<int> nums = {0, 1, 2, 3, 4, 5, 1257275983};
    rereuse::util::remove_outliers(nums);
    EXPECT_EQ(std::count(nums.cbegin(), nums.cend(), 1257275983), 0);
}
