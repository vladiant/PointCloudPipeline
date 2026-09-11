/// \file queries.hpp
/// \brief Problem-level query semantics on top of the generic SpatialIndex
///        primitives (FR-4, FR-5).
#ifndef PCP_QUERY_QUERIES_HPP
#define PCP_QUERY_QUERIES_HPP

#include <cstddef>
#include <vector>

#include "pcp/core/types.hpp"
#include "pcp/index/spatial_index.hpp"

namespace pcp::query {

/// Result of a nearest-neighbor query.
struct NearestResult {
    std::vector<Neighbor> neighbors; ///< up to k results, sorted ascending
};

/// FR-4: the \p k nearest neighbors of \p q using \p idx.
[[nodiscard]] NearestResult
nearestNeighbors(const SpatialIndex& idx, const Point3& q, std::size_t k);

/// Result of an obstacle-avoidance query.
struct ObstacleResult {
    bool obstaclePresent{};       ///< true if any point lies within radius
    std::vector<Neighbor> points; ///< the offending points, sorted ascending
};

/// FR-5: is anything within \p radius of \p probe? Returns the offending points.
[[nodiscard]] ObstacleResult
obstaclesWithin(const SpatialIndex& idx, const Point3& probe, Scalar radius);

} // namespace pcp::query

#endif // PCP_QUERY_QUERIES_HPP
