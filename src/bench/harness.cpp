/// \file harness.cpp
/// \brief Implementation of the timing/throughput/correctness harness
///        (see harness.hpp).
#include "pcp/bench/harness.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <vector>

namespace pcp::bench {
namespace {

using Clock = std::chrono::steady_clock;

long long nowNs() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               Clock::now().time_since_epoch())
        .count();
}

// Compare two neighbor sets by their sorted squared distances within an
// absolute tolerance. Set-based comparison avoids false FAILs on equidistant
// points (design section 3.5 / R-4).
bool distancesMatch(std::vector<Neighbor> a, std::vector<Neighbor> b,
                    Scalar tolerance) {
    if (a.size() != b.size()) {
        return false;
    }
    const auto byDist = [](const Neighbor& l, const Neighbor& r) {
        return l.squaredDist < r.squaredDist;
    };
    std::sort(a.begin(), a.end(), byDist);
    std::sort(b.begin(), b.end(), byDist);
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::fabs(a[i].squaredDist - b[i].squaredDist) > tolerance) {
            return false;
        }
    }
    return true;
}

} // namespace

ScopedTimer::ScopedTimer(Timing& out) : mOut(out), mStartNs(nowNs()) {}

ScopedTimer::~ScopedTimer() {
    const long long elapsedNs = nowNs() - mStartNs;
    mOut.microseconds = static_cast<double>(elapsedNs) / 1000.0;
}

ComparisonReport compareKNearest(const SpatialIndex& indexed,
                                 const SpatialIndex& baseline,
                                 const PointCloud& cloud, const Point3& query,
                                 std::size_t k, Scalar tolerance) {
    ComparisonReport report;
    report.pointCount = cloud.size();
    report.indexedQuery.label = indexed.name() + " kNN";
    report.baselineQuery.label = baseline.name() + " kNN";

    std::vector<Neighbor> indexedResult;
    {
        ScopedTimer t(report.indexedQuery);
        indexedResult = indexed.kNearest(query, k);
    }
    std::vector<Neighbor> baselineResult;
    {
        ScopedTimer t(report.baselineQuery);
        baselineResult = baseline.kNearest(query, k);
    }

    report.speedup = report.indexedQuery.microseconds > 0.0
                         ? report.baselineQuery.microseconds /
                               report.indexedQuery.microseconds
                         : 0.0;
    report.throughputPointsPerSec =
        report.indexedQuery.microseconds > 0.0
            ? static_cast<double>(report.pointCount) /
                  (report.indexedQuery.microseconds / 1e6)
            : 0.0;
    report.resultsMatch =
        distancesMatch(indexedResult, baselineResult, tolerance);
    return report;
}

ComparisonReport compareRadius(const SpatialIndex& indexed,
                               const SpatialIndex& baseline,
                               const PointCloud& cloud, const Point3& query,
                               Scalar radius, Scalar tolerance) {
    ComparisonReport report;
    report.pointCount = cloud.size();
    report.indexedQuery.label = indexed.name() + " radius";
    report.baselineQuery.label = baseline.name() + " radius";

    std::vector<Neighbor> indexedResult;
    {
        ScopedTimer t(report.indexedQuery);
        indexedResult = indexed.withinRadius(query, radius);
    }
    std::vector<Neighbor> baselineResult;
    {
        ScopedTimer t(report.baselineQuery);
        baselineResult = baseline.withinRadius(query, radius);
    }

    report.speedup = report.indexedQuery.microseconds > 0.0
                         ? report.baselineQuery.microseconds /
                               report.indexedQuery.microseconds
                         : 0.0;
    report.throughputPointsPerSec =
        report.indexedQuery.microseconds > 0.0
            ? static_cast<double>(report.pointCount) /
                  (report.indexedQuery.microseconds / 1e6)
            : 0.0;
    report.resultsMatch =
        distancesMatch(indexedResult, baselineResult, tolerance);
    return report;
}

std::ostream& operator<<(std::ostream& os, const ComparisonReport& report) {
    const std::ios::fmtflags savedFlags = os.flags();
    const std::streamsize savedPrec = os.precision();

    os << std::fixed << std::setprecision(2);
    os << "Benchmark comparison (" << report.pointCount << " points):\n";
    if (!report.indexBuild.label.empty()) {
        os << "  " << report.indexBuild.label << " : "
           << report.indexBuild.microseconds << " us\n";
    }
    os << "  " << report.indexedQuery.label << "  : "
       << report.indexedQuery.microseconds << " us\n"
       << "  " << report.baselineQuery.label << " : "
       << report.baselineQuery.microseconds << " us\n"
       << "  speedup            : " << report.speedup << "x\n"
       << "  throughput         : " << std::setprecision(0)
       << report.throughputPointsPerSec << " points/sec\n"
       << "  correctness match  : " << (report.resultsMatch ? "PASS" : "FAIL");

    os.flags(savedFlags);
    os.precision(savedPrec);
    return os;
}

} // namespace pcp::bench
