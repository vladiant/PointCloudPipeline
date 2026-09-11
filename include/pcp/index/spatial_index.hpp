/// \file spatial_index.hpp
/// \brief The dependency-inversion seam shared by every index implementation.
///
/// query/ and bench/ are written once against this interface, so they run
/// unchanged over both the k-d tree and the brute-force baseline (FR-6/FR-7).
#ifndef PCP_INDEX_SPATIAL_INDEX_HPP
#define PCP_INDEX_SPATIAL_INDEX_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "pcp/core/types.hpp"

namespace pcp {

/// A neighbor result: an index into the source PointCloud and its squared
/// distance to the query point.
struct Neighbor {
    std::size_t index{};
    Scalar squaredDist{};
};

/// Abstract spatial index over a PointCloud.
///
/// \note Implementations hold a non-owning reference to the source cloud; the
///       cloud must outlive the index (documented lifetime precondition).
class SpatialIndex {
public:
    virtual ~SpatialIndex() = default;

    /// FR-4: the \p k nearest neighbors of \p query, sorted ascending by
    /// distance. Returns at most \c cloud.size() results.
    [[nodiscard]] virtual std::vector<Neighbor>
    kNearest(const Point3& query, std::size_t k) const = 0;

    /// FR-5: all points within \p radius of \p query, sorted ascending by
    /// distance.
    [[nodiscard]] virtual std::vector<Neighbor>
    withinRadius(const Point3& query, Scalar radius) const = 0;

    /// Human-readable index name (used in benchmark reports).
    [[nodiscard]] virtual std::string name() const = 0;
};

} // namespace pcp

#endif // PCP_INDEX_SPATIAL_INDEX_HPP
