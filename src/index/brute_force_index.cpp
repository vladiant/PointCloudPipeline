/// \file brute_force_index.cpp
/// \brief Implementation of the linear-scan baseline (see brute_force_index.hpp).
#include "pcp/index/brute_force_index.hpp"

#include <algorithm>

namespace pcp {

BruteForceIndex::BruteForceIndex(const PointCloud& cloud) : mCloud(&cloud) {}

std::vector<Neighbor> BruteForceIndex::kNearest(const Point3& query,
                                                std::size_t k) const {
    const std::size_t n = mCloud->size();
    std::vector<Neighbor> all;
    if (k == 0 || n == 0) {
        return all;
    }
    all.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        all.push_back(Neighbor{i, squaredDistance((*mCloud)[i], query)});
    }
    const std::size_t kept = std::min(k, n);
    std::partial_sort(all.begin(),
                      all.begin() + static_cast<std::ptrdiff_t>(kept),
                      all.end(), [](const Neighbor& a, const Neighbor& b) {
                          return a.squaredDist < b.squaredDist;
                      });
    all.resize(kept);
    return all;
}

std::vector<Neighbor> BruteForceIndex::withinRadius(const Point3& query,
                                                    Scalar radius) const {
    std::vector<Neighbor> out;
    if (radius < 0) {
        return out;
    }
    const Scalar r2 = radius * radius;
    const std::size_t n = mCloud->size();
    for (std::size_t i = 0; i < n; ++i) {
        const Scalar d = squaredDistance((*mCloud)[i], query);
        if (d <= r2) {
            out.push_back(Neighbor{i, d});
        }
    }
    std::sort(out.begin(), out.end(),
              [](const Neighbor& a, const Neighbor& b) {
                  return a.squaredDist < b.squaredDist;
              });
    return out;
}

std::string BruteForceIndex::name() const { return "BruteForce"; }

} // namespace pcp
