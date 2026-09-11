/// \file query_correctness_tests.cpp
/// \brief The heart of the suite: indexed results MUST match the brute-force
///        baseline within tolerance on randomized clouds (FR-4/FR-5/FR-6/NFR-5).
#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include "pcp/core/synthetic.hpp"
#include "pcp/core/types.hpp"
#include "pcp/index/brute_force_index.hpp"
#include "pcp/index/kd_tree_3d.hpp"
#include "pcp/query/queries.hpp"

namespace {

constexpr pcp::Scalar kTol = 1e-3f;

std::vector<pcp::Scalar> sortedDistances(std::vector<pcp::Neighbor> ns) {
    std::vector<pcp::Scalar> d;
    d.reserve(ns.size());
    for (const auto& n : ns) {
        d.push_back(n.squaredDist);
    }
    std::sort(d.begin(), d.end());
    return d;
}

void expectMatch(const std::vector<pcp::Neighbor>& a,
                 const std::vector<pcp::Neighbor>& b) {
    const auto da = sortedDistances(a);
    const auto db = sortedDistances(b);
    ASSERT_EQ(da.size(), db.size());
    for (std::size_t i = 0; i < da.size(); ++i) {
        EXPECT_NEAR(da[i], db[i], kTol);
    }
}

TEST(Correctness, KNearestMatchesBaselineManyQueries) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(2000, 42u);
    pcp::KdTree3d tree(cloud);
    pcp::BruteForceIndex bf(cloud);

    std::mt19937 rng(123);
    std::uniform_real_distribution<float> pos(-50.0f, 50.0f);
    for (int trial = 0; trial < 50; ++trial) {
        const pcp::Point3 q{pos(rng), pos(rng), pos(rng)};
        for (std::size_t k : {1u, 5u, 8u, 20u}) {
            expectMatch(tree.kNearest(q, k), bf.kNearest(q, k));
        }
    }
}

TEST(Correctness, WithinRadiusMatchesBaselineManyQueries) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(2000, 7u);
    pcp::KdTree3d tree(cloud);
    pcp::BruteForceIndex bf(cloud);

    std::mt19937 rng(456);
    std::uniform_real_distribution<float> pos(-50.0f, 50.0f);
    for (int trial = 0; trial < 50; ++trial) {
        const pcp::Point3 q{pos(rng), pos(rng), pos(rng)};
        for (pcp::Scalar r : {5.0f, 15.0f, 30.0f}) {
            expectMatch(tree.withinRadius(q, r), bf.withinRadius(q, r));
        }
    }
}

TEST(Correctness, ObstaclePresenceAgreesWithBaseline) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(1000, 99u);
    pcp::KdTree3d tree(cloud);
    pcp::BruteForceIndex bf(cloud);

    const pcp::Point3 q = cloud[0]; // guaranteed to have a neighbor at r=0
    const auto obstacles = pcp::query::obstaclesWithin(tree, q, 1e-3f);
    EXPECT_TRUE(obstacles.obstaclePresent);

    // Far-away probe with a tiny radius: no obstacle in either index.
    const pcp::Point3 farAway{1e6f, 1e6f, 1e6f};
    const auto none = pcp::query::obstaclesWithin(tree, farAway, 1.0f);
    const auto noneBf = bf.withinRadius(farAway, 1.0f);
    EXPECT_FALSE(none.obstaclePresent);
    EXPECT_TRUE(noneBf.empty());
}

TEST(Correctness, NearestNeighborsWrapperReturnsSortedK) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(500, 8u);
    pcp::KdTree3d tree(cloud);
    const auto res = pcp::query::nearestNeighbors(tree, pcp::Point3{0, 0, 0}, 7);
    ASSERT_EQ(res.neighbors.size(), 7u);
    for (std::size_t i = 1; i < res.neighbors.size(); ++i) {
        EXPECT_LE(res.neighbors[i - 1].squaredDist,
                  res.neighbors[i].squaredDist);
    }
}

} // namespace
