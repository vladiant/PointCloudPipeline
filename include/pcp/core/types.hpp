/// \file types.hpp
/// \brief Fundamental value types for PointCloudPipeline: scalar, point, cloud
///        and axis-aligned bounding box.
///
/// Adapted from vlantonov/KDTree (MIT): the point/dimension-accessor idiom is
/// reused and extended from 2D to 3D (\c dim % 3). See NOTICES.md.
#ifndef PCP_CORE_TYPES_HPP
#define PCP_CORE_TYPES_HPP

#include <cmath>
#include <cstddef>
#include <vector>

namespace pcp {

/// Coordinate precision for the whole pipeline (design OQ-5: float chosen).
/// Centralized so it can be flipped to double in one place.
using Scalar = float;

/// A single 3D point. Trivial, contiguous, and cache-friendly at 1M+ points.
struct Point3 {
    Scalar x{};
    Scalar y{};
    Scalar z{};

    /// Dimension accessor compatible with the upstream k-d tree style,
    /// extended from 2 to 3 dimensions. \p dim is taken modulo 3.
    [[nodiscard]] Scalar operator[](std::size_t dim) const noexcept {
        switch (dim % 3u) {
        case 0u:
            return x;
        case 1u:
            return y;
        default:
            return z;
        }
    }
};

/// Squared Euclidean distance (avoids the sqrt on hot paths).
[[nodiscard]] inline Scalar squaredDistance(const Point3& a,
                                            const Point3& b) noexcept {
    const Scalar dx = a.x - b.x;
    const Scalar dy = a.y - b.y;
    const Scalar dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

/// Euclidean distance.
[[nodiscard]] inline Scalar distance(const Point3& a, const Point3& b) noexcept {
    return std::sqrt(squaredDistance(a, b));
}

/// Contiguous, cache-friendly storage for 1M+ points (NFR-1).
class PointCloud {
public:
    PointCloud() = default;
    explicit PointCloud(std::vector<Point3> pts);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] const Point3& operator[](std::size_t i) const;
    [[nodiscard]] const std::vector<Point3>& points() const noexcept;

    void reserve(std::size_t n);
    void push_back(const Point3& p);

private:
    std::vector<Point3> mPoints;
};

/// Axis-aligned bounding box; the 3D generalization of the upstream Rectangle.
struct Aabb {
    Point3 min{};
    Point3 max{};

    [[nodiscard]] bool contains(const Point3& p) const noexcept;

    /// Lower bound along \p dim (renamed from the design's overloaded \c min to
    /// avoid clashing with the \c min data member — see NOTICES / deviations).
    [[nodiscard]] Scalar minCoord(std::size_t dim) const noexcept;
    /// Upper bound along \p dim (see \ref minCoord for the naming note).
    [[nodiscard]] Scalar maxCoord(std::size_t dim) const noexcept;

    /// Cube of half-extent \p radius centered on \p c.
    [[nodiscard]] static Aabb around(const Point3& c, Scalar radius) noexcept;
};

} // namespace pcp

#endif // PCP_CORE_TYPES_HPP
