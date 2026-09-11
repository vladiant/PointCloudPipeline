/// \file index_tests.cpp
/// \brief Unit tests for KdTree3d build integrity and query edge cases.
#include <gtest/gtest.h>

#include <cmath>
#include <set>

#include "pcp/core/synthetic.hpp"
#include "pcp/core/types.hpp"
#include "pcp/index/brute_force_index.hpp"
#include "pcp/index/kd_tree_3d.hpp"

namespace {

TEST(KdTree3d, ContainsEveryInputPoint) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(500, 11u);
    pcp::KdTree3d tree(cloud);
    // Each point must be found as its own exact nearest neighbor.
    for (std::size_t i = 0; i < cloud.size(); ++i) {
        const auto nn = tree.kNearest(cloud[i], 1);
        ASSERT_EQ(nn.size(), 1u);
        EXPECT_FLOAT_EQ(nn[0].squaredDist, 0.0f);
    }
}

TEST(KdTree3d, DepthIsLogarithmicForBulkBuild) {
    const std::size_t n = 4096;
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(n, 3u);
    pcp::KdTree3d tree(cloud);
    // Balanced median-split build: depth ~ ceil(log2(n)) = 12; allow slack.
    const int bound = static_cast<int>(std::ceil(std::log2(double(n)))) + 2;
    EXPECT_LE(tree.depth(), bound);
    EXPECT_GT(tree.depth(), 0);
}

TEST(KdTree3d, EmptyCloudYieldsEmptyResults) {
    pcp::PointCloud empty;
    pcp::KdTree3d tree(empty);
    EXPECT_EQ(tree.depth(), 0);
    EXPECT_TRUE(tree.kNearest(pcp::Point3{0, 0, 0}, 5).empty());
    EXPECT_TRUE(tree.withinRadius(pcp::Point3{0, 0, 0}, 10.0f).empty());
}

TEST(KdTree3d, KZeroReturnsEmpty) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(50, 5u);
    pcp::KdTree3d tree(cloud);
    EXPECT_TRUE(tree.kNearest(pcp::Point3{0, 0, 0}, 0).empty());
}

TEST(KdTree3d, KGreaterThanSizeReturnsAll) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(30, 9u);
    pcp::KdTree3d tree(cloud);
    const auto nn = tree.kNearest(pcp::Point3{0, 0, 0}, 1000);
    EXPECT_EQ(nn.size(), cloud.size());
}

TEST(KdTree3d, ResultsSortedAscending) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(200, 2u);
    pcp::KdTree3d tree(cloud);
    const auto nn = tree.kNearest(pcp::Point3{1, 2, 3}, 10);
    ASSERT_EQ(nn.size(), 10u);
    for (std::size_t i = 1; i < nn.size(); ++i) {
        EXPECT_LE(nn[i - 1].squaredDist, nn[i].squaredDist);
    }
}

TEST(BruteForceIndex, RadiusNegativeReturnsEmpty) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(20, 4u);
    pcp::BruteForceIndex bf(cloud);
    EXPECT_TRUE(bf.withinRadius(pcp::Point3{0, 0, 0}, -1.0f).empty());
}

TEST(Indices, NamesAreStable) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(10, 1u);
    pcp::KdTree3d tree(cloud);
    pcp::BruteForceIndex bf(cloud);
    EXPECT_EQ(tree.name(), "KdTree3d");
    EXPECT_EQ(bf.name(), "BruteForce");
}

} // namespace
