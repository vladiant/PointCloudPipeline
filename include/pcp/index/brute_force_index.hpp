/// \file brute_force_index.hpp
/// \brief Linear-scan baseline implementation of SpatialIndex (FR-6).
#ifndef PCP_INDEX_BRUTE_FORCE_INDEX_HPP
#define PCP_INDEX_BRUTE_FORCE_INDEX_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "pcp/core/types.hpp"
#include "pcp/index/spatial_index.hpp"

namespace pcp {

/// Exhaustive linear-scan index used as the correctness/speedup baseline.
class BruteForceIndex final : public SpatialIndex {
public:
    /// \note \p cloud must outlive this index.
    explicit BruteForceIndex(const PointCloud& cloud);

    [[nodiscard]] std::vector<Neighbor>
    kNearest(const Point3& query, std::size_t k) const override;

    [[nodiscard]] std::vector<Neighbor>
    withinRadius(const Point3& query, Scalar radius) const override;

    [[nodiscard]] std::string name() const override;

private:
    const PointCloud* mCloud; ///< non-owning; cloud outlives the index
};

} // namespace pcp

#endif // PCP_INDEX_BRUTE_FORCE_INDEX_HPP
