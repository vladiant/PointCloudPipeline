/// \file queries.cpp
/// \brief Implementation of the thin query layer (see queries.hpp).
#include "pcp/query/queries.hpp"

namespace pcp::query {

NearestResult nearestNeighbors(const SpatialIndex& idx, const Point3& q,
                               std::size_t k) {
    NearestResult result;
    result.neighbors = idx.kNearest(q, k);
    return result;
}

ObstacleResult obstaclesWithin(const SpatialIndex& idx, const Point3& probe,
                               Scalar radius) {
    ObstacleResult result;
    result.points = idx.withinRadius(probe, radius);
    result.obstaclePresent = !result.points.empty();
    return result;
}

} // namespace pcp::query
