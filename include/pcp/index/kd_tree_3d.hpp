/// \file kd_tree_3d.hpp
/// \brief A balanced 3D k-d tree with k-NN and radius search.
///
/// Adapted from vlantonov/KDTree (MIT), extended from 2D to 3D and given a
/// bulk median-split builder plus a first-class k-NN query (neither existed
/// upstream). See NOTICES.md and design sections 6 / 7 (OQ-1, OQ-2).
#ifndef PCP_INDEX_KD_TREE_3D_HPP
#define PCP_INDEX_KD_TREE_3D_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "pcp/core/types.hpp"
#include "pcp/index/spatial_index.hpp"

namespace pcp {

/// Balanced k-d tree over a PointCloud, built once by median split.
class KdTree3d final : public SpatialIndex {
public:
    /// Bulk-builds a balanced tree via median split (O(N log N)).
    /// \note \p cloud must outlive this index.
    explicit KdTree3d(const PointCloud& cloud);

    [[nodiscard]] std::vector<Neighbor>
    kNearest(const Point3& query, std::size_t k) const override;

    [[nodiscard]] std::vector<Neighbor>
    withinRadius(const Point3& query, Scalar radius) const override;

    [[nodiscard]] std::string name() const override;

    /// Maximum depth of the built tree (0 for an empty cloud).
    [[nodiscard]] int depth() const noexcept;

private:
    struct Node {
        std::size_t point{};
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };

    std::unique_ptr<Node> buildRange(std::vector<std::size_t>& idx,
                                     std::size_t lo, std::size_t hi, int axis);
    void knnSearch(const Node* node, const Point3& query, std::size_t k,
                   int axis, std::vector<Neighbor>& heap) const;
    void radiusSearch(const Node* node, const Point3& query, Scalar radiusSq,
                      int axis, std::vector<Neighbor>& out) const;

    std::unique_ptr<Node> mRoot;
    const PointCloud* mCloud; ///< non-owning; cloud outlives the index
};

} // namespace pcp

#endif // PCP_INDEX_KD_TREE_3D_HPP
