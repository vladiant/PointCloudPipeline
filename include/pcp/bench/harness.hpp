/// \file harness.hpp
/// \brief Timing, throughput and correctness-comparison harness (FR-7, FR-9,
///        NFR-5).
#ifndef PCP_BENCH_HARNESS_HPP
#define PCP_BENCH_HARNESS_HPP

#include <cstddef>
#include <ostream>
#include <string>

#include "pcp/core/types.hpp"
#include "pcp/index/spatial_index.hpp"

namespace pcp::bench {

/// A single labeled timing measurement.
struct Timing {
    std::string label;
    double microseconds{};
};

/// The result of comparing an indexed query against the brute-force baseline.
struct ComparisonReport {
    Timing indexBuild;    ///< filled in by the caller (CLI), if measured
    Timing indexedQuery;
    Timing baselineQuery;
    double speedup{};                ///< baseline / indexed
    double throughputPointsPerSec{}; ///< FR-9
    bool resultsMatch{};             ///< NFR-5, within tolerance
    std::size_t pointCount{};
};

/// Run the same k-NN query on both indices and compare within \p tolerance.
/// \p tolerance is an absolute tolerance on squared distances.
[[nodiscard]] ComparisonReport
compareKNearest(const SpatialIndex& indexed, const SpatialIndex& baseline,
                const PointCloud& cloud, const Point3& query, std::size_t k,
                Scalar tolerance);

/// Run the same radius query on both indices and compare within \p tolerance.
[[nodiscard]] ComparisonReport
compareRadius(const SpatialIndex& indexed, const SpatialIndex& baseline,
              const PointCloud& cloud, const Point3& query, Scalar radius,
              Scalar tolerance);

std::ostream& operator<<(std::ostream& os, const ComparisonReport& report);

/// Thin RAII timer; on destruction it records the elapsed microseconds into the
/// referenced Timing. Generalized from the upstream \c TimeBench.
class ScopedTimer {
public:
    explicit ScopedTimer(Timing& out);
    ~ScopedTimer();

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
    Timing& mOut;
    long long mStartNs;
};

} // namespace pcp::bench

#endif // PCP_BENCH_HARNESS_HPP
