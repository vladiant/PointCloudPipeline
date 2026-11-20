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
    std::string label; ///< Human-readable measurement name.
    double microseconds{}; ///< Measured duration in microseconds.
};

/// The result of comparing an indexed query against the brute-force baseline.
struct ComparisonReport {
    Timing indexBuild;    ///< filled in by the caller (CLI), if measured
    Timing indexedQuery; ///< Time spent executing the indexed query.
    Timing baselineQuery; ///< Time spent executing the brute-force query.
    double speedup{};                ///< baseline / indexed
    double throughputPointsPerSec{}; ///< FR-9
    bool resultsMatch{};             ///< NFR-5, within tolerance
    std::size_t pointCount{}; ///< Number of points in the compared cloud.
};

/// Run the same k-NN query on both indices and compare within \p tolerance.
/// \p tolerance is an absolute tolerance on squared distances.
/// \param indexed Spatial index under test.
/// \param baseline Brute-force comparison index.
/// \param cloud Source point cloud.
/// \param query Query point.
/// \param k Maximum number of neighbors to retrieve.
/// \param tolerance Absolute squared-distance comparison tolerance.
/// \return Timing and correctness comparison report.
[[nodiscard]] ComparisonReport
compareKNearest(const SpatialIndex& indexed, const SpatialIndex& baseline,
                const PointCloud& cloud, const Point3& query, std::size_t k,
                Scalar tolerance);

/// Run the same radius query on both indices and compare within \p tolerance.
/// \param indexed Spatial index under test.
/// \param baseline Brute-force comparison index.
/// \param cloud Source point cloud.
/// \param query Center point.
/// \param radius Search radius.
/// \param tolerance Absolute squared-distance comparison tolerance.
/// \return Timing and correctness comparison report.
[[nodiscard]] ComparisonReport
compareRadius(const SpatialIndex& indexed, const SpatialIndex& baseline,
              const PointCloud& cloud, const Point3& query, Scalar radius,
              Scalar tolerance);

/// Writes a human-readable benchmark comparison.
/// \param os Destination stream.
/// \param report Benchmark result to print.
/// \return Reference to \p os.
std::ostream& operator<<(std::ostream& os, const ComparisonReport& report);

/// Thin RAII timer; on destruction it records the elapsed microseconds into the
/// referenced Timing. Generalized from the upstream \c TimeBench.
class ScopedTimer {
public:
    /// Starts timing and records elapsed time into \p out on destruction.
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
