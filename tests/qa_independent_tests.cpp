/// \file qa_independent_tests.cpp
/// \brief Independent QA checks added during the Testing stage.
///
/// The developer suite compares indexed-vs-baseline results by their sorted
/// squared distances only (see query_correctness_tests.cpp and harness.cpp).
/// That would pass even if the k-d tree returned wrong-but-equidistant points.
/// These tests close that gap by comparing the actual returned point *indices*
/// against the brute-force baseline, and by pinning k-NN indices on a cloud
/// engineered to have strictly distinct distances (no tie ambiguity).
#include <gtest/gtest.h>

#include <random>
#include <set>
#include <vector>

#include "pcp/core/synthetic.hpp"
#include "pcp/core/types.hpp"
#include "pcp/index/brute_force_index.hpp"
#include "pcp/index/kd_tree_3d.hpp"

namespace {

std::set<std::size_t> indexSet(const std::vector<pcp::Neighbor>& ns) {
    std::set<std::size_t> s;
    for (const auto& n : ns) {
        s.insert(n.index);
    }
    return s;
}

// withinRadius returns *every* point inside the radius, so the index set is
// unambiguous (unlike a k-NN boundary tie). The k-d tree must return the exact
// same set of point indices as the linear scan.
TEST(QAIndependent, WithinRadiusIndexSetEqualsBaseline) {
    pcp::PointCloud cloud = pcp::makeSyntheticCloud(3000, 21u);
    pcp::KdTree3d tree(cloud);
    pcp::BruteForceIndex bf(cloud);

    std::mt19937 rng(2024);
    std::uniform_real_distribution<float> pos(-60.0f, 60.0f);
    for (int trial = 0; trial < 40; ++trial) {
        const pcp::Point3 q{pos(rng), pos(rng), pos(rng)};
        for (pcp::Scalar r : {3.0f, 10.0f, 25.0f, 100.0f}) {
            const auto treeSet = indexSet(tree.withinRadius(q, r));
            const auto bfSet = indexSet(bf.withinRadius(q, r));
            EXPECT_EQ(treeSet, bfSet)
                << "radius=" << r << " query=(" << q.x << ',' << q.y << ','
                << q.z << ')';
        }
    }
}

// A grid with unit spacing so pairwise squared distances to an off-lattice
// probe are strictly distinct: the k-NN index set is then uniquely defined and
// must match the brute-force baseline exactly (not merely by distance).
TEST(QAIndependent, KNearestIndexSetOnDistinctDistanceCloud) {
    std::vector<pcp::Point3> pts;
    for (int x = 0; x < 12; ++x) {
        for (int y = 0; y < 12; ++y) {
            for (int z = 0; z < 12; ++z) {
                pts.push_back(pcp::Point3{static_cast<float>(x),
                                          static_cast<float>(y),
                                          static_cast<float>(z)});
            }
        }
    }
    pcp::PointCloud cloud(std::move(pts));
    pcp::KdTree3d tree(cloud);
    pcp::BruteForceIndex bf(cloud);

    // Irrational-ish offsets keep the probe off any symmetry plane so distances
    // to distinct lattice points do not collide.
    const pcp::Point3 q{5.3137f, 6.7191f, 4.1233f};
    for (std::size_t k : {1u, 4u, 8u, 16u, 32u}) {
        EXPECT_EQ(indexSet(tree.kNearest(q, k)), indexSet(bf.kNearest(q, k)))
            << "k=" << k;
    }
}

} // namespace
