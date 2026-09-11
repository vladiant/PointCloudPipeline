/// \file synthetic.cpp
/// \brief Deterministic synthetic point-cloud generator (see synthetic.hpp).
#include "pcp/core/synthetic.hpp"

#include <random>

namespace pcp {

PointCloud makeSyntheticCloud(std::size_t count, unsigned seed, Scalar extent) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-extent, extent);

    std::vector<Point3> pts;
    pts.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        pts.push_back(Point3{static_cast<Scalar>(dist(rng)),
                             static_cast<Scalar>(dist(rng)),
                             static_cast<Scalar>(dist(rng))});
    }
    return PointCloud(std::move(pts));
}

} // namespace pcp
