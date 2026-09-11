/// \file main.cpp
/// \brief pcp_cli driver: load/generate -> build index -> query -> benchmark ->
///        report -> offscreen visualization (design section 9, FR-1..FR-10).
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "pcp/bench/harness.hpp"
#include "pcp/core/hardware_info.hpp"
#include "pcp/core/synthetic.hpp"
#include "pcp/core/types.hpp"
#include "pcp/index/brute_force_index.hpp"
#include "pcp/index/kd_tree_3d.hpp"
#include "pcp/io/loader.hpp"
#include "pcp/query/queries.hpp"
#include "pcp/viz/renderer.hpp"

#include "args.hpp"

namespace {

pcp::io::Format toFormat(const std::string& s) {
    if (s == "auto") {
        return pcp::io::Format::Auto;
    }
    if (s == "kitti") {
        return pcp::io::Format::KittiBin;
    }
    if (s == "pcd") {
        return pcp::io::Format::Pcd;
    }
    if (s == "ply") {
        return pcp::io::Format::Ply;
    }
    throw std::runtime_error("Unknown --format: '" + s + "'");
}

pcp::Point3 centroidFallback(const pcp::PointCloud& cloud) {
    // A stable, seed-independent default probe: the first stored point.
    return cloud.empty() ? pcp::Point3{} : cloud[0];
}

} // namespace

int main(int argc, char** argv) {
    try {
        const pcp::cli::Args args = pcp::cli::parseArgs(argc, argv);
        if (args.help) {
            std::cout << pcp::cli::usage() << '\n';
            return 0;
        }

        // --- Load or generate the frame (FR-1) -------------------------------
        pcp::PointCloud cloud;
        if (args.input.has_value()) {
            const auto loaded =
                pcp::io::load(*args.input, toFormat(args.format));
            cloud = std::move(loaded.cloud);
            std::cout << "Loaded " << loaded.pointCount << " points from "
                      << *args.input << '\n';
        } else {
            const std::size_t n = args.generate > 0 ? args.generate : 200000;
            cloud = pcp::makeSyntheticCloud(n, args.seed);
            std::cout << "Generated " << cloud.size()
                      << " synthetic points (seed=" << args.seed << ")\n";
        }
        std::cout << "dimensionality=3D\n"; // FR-2

        if (cloud.empty()) {
            std::cerr << "Empty cloud; nothing to do.\n";
            return 1;
        }

        // --- Build indices (FR-3) --------------------------------------------
        pcp::bench::Timing buildTiming;
        buildTiming.label = "KdTree3d build";
        std::unique_ptr<pcp::KdTree3d> kdtree;
        {
            pcp::bench::ScopedTimer t(buildTiming);
            kdtree = std::make_unique<pcp::KdTree3d>(cloud);
        }
        pcp::BruteForceIndex baseline(cloud);
        std::cout << "Built " << kdtree->name() << " (depth "
                  << kdtree->depth() << ") in " << buildTiming.microseconds
                  << " us\n";

        // --- Query location ---------------------------------------------------
        pcp::Point3 query = centroidFallback(cloud);
        if (args.query.has_value()) {
            const auto& q = *args.query;
            query = pcp::Point3{q[0], q[1], q[2]};
        }
        const pcp::Scalar radius =
            args.radius.has_value() ? *args.radius : pcp::Scalar{5.0f};

        // --- k-NN benchmark vs baseline (FR-4, FR-6, FR-7, FR-9) -------------
        pcp::bench::ComparisonReport knn = pcp::bench::compareKNearest(
            *kdtree, baseline, cloud, query, args.k, args.tolerance);
        knn.indexBuild = buildTiming;
        std::cout << "\n[k-NN k=" << args.k << " @ (" << query.x << ',' << query.y
                  << ',' << query.z << ")]\n"
                  << knn << '\n';

        // --- Obstacle-avoidance / radius benchmark (FR-5) --------------------
        pcp::bench::ComparisonReport rad = pcp::bench::compareRadius(
            *kdtree, baseline, cloud, query, radius, args.tolerance);
        const auto obstacles =
            pcp::query::obstaclesWithin(*kdtree, query, radius);
        std::cout << "\n[radius R=" << radius << "]\n"
                  << rad << '\n'
                  << "  obstacle present   : "
                  << (obstacles.obstaclePresent ? "YES" : "no") << " ("
                  << obstacles.points.size() << " points)\n";

        // --- Hardware / build report (FR-10) ---------------------------------
        std::cout << '\n' << pcp::queryHardwareInfo() << '\n';

        // --- Offscreen visualization (FR-8) ----------------------------------
        if (args.visualize) {
            const auto knnResult =
                pcp::query::nearestNeighbors(*kdtree, query, args.k);
            pcp::viz::RenderOptions opts;
            opts.interactive = false;
            opts.outputImage = args.vizOutput;
            pcp::viz::renderFrame(cloud, knnResult.neighbors, opts);
            std::cout << "\nWrote visualization image: " << args.vizOutput
                      << '\n';
        }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
