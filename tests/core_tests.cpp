/// \file core_tests.cpp
/// \brief Unit tests for pcp_core geometry primitives, PointCloud, HardwareInfo
///        and the synthetic generator.
#include <gtest/gtest.h>

#include "pcp/core/hardware_info.hpp"
#include "pcp/core/synthetic.hpp"
#include "pcp/core/types.hpp"

namespace {

TEST(Point3, DimensionAccessorCyclesXYZ) {
    pcp::Point3 p{1.0f, 2.0f, 3.0f};
    EXPECT_FLOAT_EQ(p[0], 1.0f);
    EXPECT_FLOAT_EQ(p[1], 2.0f);
    EXPECT_FLOAT_EQ(p[2], 3.0f);
    EXPECT_FLOAT_EQ(p[3], 1.0f); // wraps via dim % 3
}

TEST(Distance, SquaredAndEuclidean) {
    pcp::Point3 a{0.0f, 0.0f, 0.0f};
    pcp::Point3 b{3.0f, 4.0f, 0.0f};
    EXPECT_FLOAT_EQ(pcp::squaredDistance(a, b), 25.0f);
    EXPECT_FLOAT_EQ(pcp::distance(a, b), 5.0f);
}

TEST(Distance, ZeroForIdenticalPoints) {
    pcp::Point3 a{-1.5f, 2.5f, 7.0f};
    EXPECT_FLOAT_EQ(pcp::squaredDistance(a, a), 0.0f);
}

TEST(PointCloud, PushReserveSizeEmpty) {
    pcp::PointCloud cloud;
    EXPECT_TRUE(cloud.empty());
    EXPECT_EQ(cloud.size(), 0u);
    cloud.reserve(2);
    cloud.push_back(pcp::Point3{1, 2, 3});
    cloud.push_back(pcp::Point3{4, 5, 6});
    EXPECT_FALSE(cloud.empty());
    ASSERT_EQ(cloud.size(), 2u);
    EXPECT_FLOAT_EQ(cloud[1].y, 5.0f);
    EXPECT_EQ(cloud.points().size(), 2u);
}

TEST(Aabb, ContainsBoundaryAndOutside) {
    pcp::Aabb box = pcp::Aabb::around(pcp::Point3{0, 0, 0}, 2.0f);
    EXPECT_TRUE(box.contains(pcp::Point3{0, 0, 0}));
    EXPECT_TRUE(box.contains(pcp::Point3{2, 2, 2})); // corner (edge case)
    EXPECT_FALSE(box.contains(pcp::Point3{2.01f, 0, 0}));
    EXPECT_FLOAT_EQ(box.minCoord(0), -2.0f);
    EXPECT_FLOAT_EQ(box.maxCoord(2), 2.0f);
}

TEST(Aabb, AroundHandlesNegativeRadius) {
    pcp::Aabb box = pcp::Aabb::around(pcp::Point3{1, 1, 1}, -3.0f);
    EXPECT_FLOAT_EQ(box.minCoord(0), -2.0f);
    EXPECT_FLOAT_EQ(box.maxCoord(0), 4.0f);
}

TEST(Synthetic, DeterministicForSameSeed) {
    pcp::PointCloud a = pcp::makeSyntheticCloud(100, 7u);
    pcp::PointCloud b = pcp::makeSyntheticCloud(100, 7u);
    ASSERT_EQ(a.size(), b.size());
    for (std::size_t i = 0; i < a.size(); ++i) {
        EXPECT_FLOAT_EQ(a[i].x, b[i].x);
        EXPECT_FLOAT_EQ(a[i].y, b[i].y);
        EXPECT_FLOAT_EQ(a[i].z, b[i].z);
    }
}

TEST(Synthetic, DiffersForDifferentSeed) {
    pcp::PointCloud a = pcp::makeSyntheticCloud(100, 1u);
    pcp::PointCloud b = pcp::makeSyntheticCloud(100, 2u);
    bool anyDifferent = false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i].x != b[i].x) {
            anyDifferent = true;
            break;
        }
    }
    EXPECT_TRUE(anyDifferent);
}

TEST(HardwareInfo, ReportsThreadsAndCompiler) {
    pcp::HardwareInfo info = pcp::queryHardwareInfo();
    EXPECT_GE(info.hardwareThreads, 1u);
    EXPECT_FALSE(info.compiler.empty());
    EXPECT_FALSE(info.buildType.empty());
}

} // namespace
