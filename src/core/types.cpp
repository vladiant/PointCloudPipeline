/// \file types.cpp
/// \brief Implementation of PointCloud and Aabb (see types.hpp).
#include "pcp/core/types.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace pcp {

PointCloud::PointCloud(std::vector<Point3> pts) : mPoints(std::move(pts)) {}

std::size_t PointCloud::size() const noexcept { return mPoints.size(); }

bool PointCloud::empty() const noexcept { return mPoints.empty(); }

const Point3& PointCloud::operator[](std::size_t i) const { return mPoints[i]; }

const std::vector<Point3>& PointCloud::points() const noexcept {
    return mPoints;
}

void PointCloud::reserve(std::size_t n) { mPoints.reserve(n); }

void PointCloud::push_back(const Point3& p) { mPoints.push_back(p); }

bool Aabb::contains(const Point3& p) const noexcept {
    return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y &&
           p.z >= min.z && p.z <= max.z;
}

Scalar Aabb::minCoord(std::size_t dim) const noexcept { return min[dim]; }

Scalar Aabb::maxCoord(std::size_t dim) const noexcept { return max[dim]; }

Aabb Aabb::around(const Point3& c, Scalar radius) noexcept {
    const Scalar r = std::abs(radius);
    return Aabb{Point3{c.x - r, c.y - r, c.z - r},
               Point3{c.x + r, c.y + r, c.z + r}};
}

} // namespace pcp
