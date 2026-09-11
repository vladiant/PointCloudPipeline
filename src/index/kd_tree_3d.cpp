/// \file kd_tree_3d.cpp
/// \brief Implementation of the balanced 3D k-d tree (see kd_tree_3d.hpp).
///
/// Adapted from vlantonov/KDTree (MIT): the recursive node / unique_ptr chain
/// and split-on-dimension pruning idea are reused; the 3D generalization,
/// bulk median-split build and k-NN max-heap search are new work.
#include "pcp/index/kd_tree_3d.hpp"

#include <algorithm>
#include <cstddef>
#include <numeric>

namespace pcp {
namespace {

// Max-heap comparator on squared distance: heap.front() is the current worst.
struct WorseFirst {
    bool operator()(const Neighbor& a, const Neighbor& b) const noexcept {
        return a.squaredDist < b.squaredDist;
    }
};

} // namespace

KdTree3d::KdTree3d(const PointCloud& cloud) : mCloud(&cloud) {
    const std::size_t n = cloud.size();
    if (n == 0) {
        return;
    }
    std::vector<std::size_t> idx(n);
    std::iota(idx.begin(), idx.end(), std::size_t{0});
    mRoot = buildRange(idx, 0, n, 0);
}

std::unique_ptr<KdTree3d::Node>
KdTree3d::buildRange(std::vector<std::size_t>& idx, std::size_t lo,
                     std::size_t hi, int axis) {
    if (lo >= hi) {
        return nullptr;
    }
    const std::size_t mid = lo + (hi - lo) / 2;
    const int a = axis % 3;
    std::nth_element(idx.begin() + static_cast<std::ptrdiff_t>(lo),
                     idx.begin() + static_cast<std::ptrdiff_t>(mid),
                     idx.begin() + static_cast<std::ptrdiff_t>(hi),
                     [this, a](std::size_t l, std::size_t r) {
                         return (*mCloud)[l][static_cast<std::size_t>(a)] <
                                (*mCloud)[r][static_cast<std::size_t>(a)];
                     });

    auto node = std::make_unique<Node>();
    node->point = idx[mid];
    node->left = buildRange(idx, lo, mid, axis + 1);
    node->right = buildRange(idx, mid + 1, hi, axis + 1);
    return node;
}

void KdTree3d::knnSearch(const Node* node, const Point3& query, std::size_t k,
                         int axis, std::vector<Neighbor>& heap) const {
    if (node == nullptr) {
        return;
    }
    const Point3& p = (*mCloud)[node->point];
    const Scalar d = squaredDistance(p, query);
    if (heap.size() < k) {
        heap.push_back(Neighbor{node->point, d});
        std::push_heap(heap.begin(), heap.end(), WorseFirst{});
    } else if (d < heap.front().squaredDist) {
        std::pop_heap(heap.begin(), heap.end(), WorseFirst{});
        heap.back() = Neighbor{node->point, d};
        std::push_heap(heap.begin(), heap.end(), WorseFirst{});
    }

    const int a = axis % 3;
    const Scalar diff =
        query[static_cast<std::size_t>(a)] - p[static_cast<std::size_t>(a)];
    const Node* near = diff < 0 ? node->left.get() : node->right.get();
    const Node* far = diff < 0 ? node->right.get() : node->left.get();

    knnSearch(near, query, k, axis + 1, heap);
    if (heap.size() < k || diff * diff < heap.front().squaredDist) {
        knnSearch(far, query, k, axis + 1, heap);
    }
}

void KdTree3d::radiusSearch(const Node* node, const Point3& query,
                            Scalar radiusSq, int axis,
                            std::vector<Neighbor>& out) const {
    if (node == nullptr) {
        return;
    }
    const Point3& p = (*mCloud)[node->point];
    const Scalar d = squaredDistance(p, query);
    if (d <= radiusSq) {
        out.push_back(Neighbor{node->point, d});
    }

    const int a = axis % 3;
    const Scalar diff =
        query[static_cast<std::size_t>(a)] - p[static_cast<std::size_t>(a)];
    const Node* near = diff < 0 ? node->left.get() : node->right.get();
    const Node* far = diff < 0 ? node->right.get() : node->left.get();

    radiusSearch(near, query, radiusSq, axis + 1, out);
    if (diff * diff <= radiusSq) {
        radiusSearch(far, query, radiusSq, axis + 1, out);
    }
}

std::vector<Neighbor> KdTree3d::kNearest(const Point3& query,
                                         std::size_t k) const {
    std::vector<Neighbor> heap;
    if (k == 0 || mRoot == nullptr) {
        return heap;
    }
    heap.reserve(k);
    knnSearch(mRoot.get(), query, k, 0, heap);
    std::sort(heap.begin(), heap.end(),
              [](const Neighbor& a, const Neighbor& b) {
                  return a.squaredDist < b.squaredDist;
              });
    return heap;
}

std::vector<Neighbor> KdTree3d::withinRadius(const Point3& query,
                                             Scalar radius) const {
    std::vector<Neighbor> out;
    if (mRoot == nullptr || radius < 0) {
        return out;
    }
    radiusSearch(mRoot.get(), query, radius * radius, 0, out);
    std::sort(out.begin(), out.end(),
              [](const Neighbor& a, const Neighbor& b) {
                  return a.squaredDist < b.squaredDist;
              });
    return out;
}

std::string KdTree3d::name() const { return "KdTree3d"; }

int KdTree3d::depth() const noexcept {
    // Iterative-friendly recursion over an already-built tree.
    struct Rec {
        static int of(const Node* n) {
            if (n == nullptr) {
                return 0;
            }
            return 1 + std::max(of(n->left.get()), of(n->right.get()));
        }
    };
    return Rec::of(mRoot.get());
}

} // namespace pcp
