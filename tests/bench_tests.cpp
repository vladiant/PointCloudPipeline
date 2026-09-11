/// \file bench_tests.cpp
/// \brief Unit tests for the benchmark harness, plus a fast benchmark smoke
///        test suitable for CI.
#include <gtest/gtest.h>

#include <thread>

#include "pcp/bench/harness.hpp"
#include "pcp/core/synthetic.hpp"
#include "pcp/core/types.hpp"
#include "pcp/index/brute_force_index.hpp"
#include "pcp/index/kd_tree_3d.hpp"

namespace {

TEST(ScopedTimer, RecordsNonNegativeElapsed) {
    pcp::bench::Timing t;
    {
        pcp::bench::ScopedTimer timer(t);
        volatile double x = 0.0;
        for (int i = 0; i < 100000; ++i) {
            x += i;
        }
        (void)x;
    }
    EXPECT_GE(t.microseconds, 0.0);
}

TEST(CompareKNearest, ResultsMatchAndSpeedupPositive) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(10000, 42u);
    pcp::KdTree3d tree(cloud);
    pcp::BruteForceIndex bf(cloud);

    const auto report = pcp::bench::compareKNearest(
        tree, bf, cloud, pcp::Point3{0, 0, 0}, 8, 1e-3f);
    EXPECT_TRUE(report.resultsMatch);
    EXPECT_EQ(report.pointCount, cloud.size());
    EXPECT_GT(report.throughputPointsPerSec, 0.0);
    EXPECT_GT(report.indexedQuery.microseconds, 0.0);
    EXPECT_GT(report.baselineQuery.microseconds, 0.0);
}

TEST(CompareRadius, ResultsMatch) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(10000, 7u);
    pcp::KdTree3d tree(cloud);
    pcp::BruteForceIndex bf(cloud);

    const auto report = pcp::bench::compareRadius(
        tree, bf, cloud, pcp::Point3{1, 1, 1}, 10.0f, 1e-3f);
    EXPECT_TRUE(report.resultsMatch);
    EXPECT_EQ(report.pointCount, cloud.size());
}

TEST(CompareKNearest, MismatchIsDetected) {
    // Two different clouds -> different neighbor distances -> FAIL expected.
    pcp::PointCloud a = pcp::makeSyntheticCloud(2000, 1u);
    pcp::PointCloud b = pcp::makeSyntheticCloud(2000, 2u);
    pcp::KdTree3d treeA(a);
    pcp::BruteForceIndex bfB(b);
    const auto report = pcp::bench::compareKNearest(
        treeA, bfB, a, pcp::Point3{3, 3, 3}, 8, 1e-6f);
    EXPECT_FALSE(report.resultsMatch);
}

// Fast benchmark smoke test (design section 5.2): guards the indexed-vs-baseline
// speedup on a small synthetic cloud without needing the multi-hundred-MB
// dataset in CI.
TEST(BenchSmoke, IndexedFasterThanBruteForceOnModerateCloud) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(50000, 42u);
    pcp::KdTree3d tree(cloud);
    pcp::BruteForceIndex bf(cloud);

    // Warm + measure a representative query.
    const auto report = pcp::bench::compareKNearest(
        tree, bf, cloud, pcp::Point3{5, -5, 10}, 8, 1e-3f);
    EXPECT_TRUE(report.resultsMatch);
    // On 50k points the k-d tree should comfortably beat linear scan.
    EXPECT_GT(report.speedup, 1.0);
}

} // namespace
