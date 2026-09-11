/// \file renderer.cpp
/// \brief Default custom PPM renderer with an optional Open3D interactive path.
///
/// The default path writes an orthographic (top-down, X/Y) projection of the
/// cloud to a binary PPM, highlighting query-result points in red. The Open3D
/// interactive window is compiled only when PCP_ENABLE_VIZ is defined (OQ-4).
#include "pcp/viz/renderer.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <vector>

#ifdef PCP_ENABLE_VIZ
#include <iostream>

#include <open3d/Open3D.h>
#endif

namespace pcp::viz {
namespace {

struct Rgb {
    std::uint8_t r{};
    std::uint8_t g{};
    std::uint8_t b{};
};

void writePpm(const std::filesystem::path& path, int width, int height,
              const std::vector<Rgb>& pixels) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot write image: " + path.string());
    }
    out << "P6\n" << width << ' ' << height << "\n255\n";
    out.write(reinterpret_cast<const char*>(pixels.data()),
              static_cast<std::streamsize>(pixels.size() * sizeof(Rgb)));
    if (!out) {
        throw std::runtime_error("Failed writing image data: " + path.string());
    }
}

// Orthographic top-down projection of the cloud into a PPM image.
void renderOffscreen(const PointCloud& cloud,
                     const std::vector<Neighbor>& highlight,
                     const RenderOptions& opts) {
    const std::filesystem::path& path = *opts.outputImage;
    const int w = std::max(opts.width, 1);
    const int h = std::max(opts.height, 1);
    std::vector<Rgb> pixels(static_cast<std::size_t>(w) * h, Rgb{0, 0, 0});

    if (cloud.empty()) {
        writePpm(path, w, h, pixels);
        return;
    }

    Scalar minX = std::numeric_limits<Scalar>::max();
    Scalar minY = std::numeric_limits<Scalar>::max();
    Scalar maxX = std::numeric_limits<Scalar>::lowest();
    Scalar maxY = std::numeric_limits<Scalar>::lowest();
    for (const Point3& p : cloud.points()) {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
        maxX = std::max(maxX, p.x);
        maxY = std::max(maxY, p.y);
    }
    const Scalar spanX = std::max(maxX - minX, Scalar{1e-6f});
    const Scalar spanY = std::max(maxY - minY, Scalar{1e-6f});

    const auto toPixel = [&](const Point3& p) -> std::size_t {
        int px = static_cast<int>((p.x - minX) / spanX * (w - 1));
        int py = static_cast<int>((p.y - minY) / spanY * (h - 1));
        px = std::clamp(px, 0, w - 1);
        py = std::clamp(py, 0, h - 1);
        // Flip Y so +Y points up in the image.
        return static_cast<std::size_t>((h - 1 - py)) * w + px;
    };

    for (const Point3& p : cloud.points()) {
        pixels[toPixel(p)] = Rgb{200, 200, 200};
    }
    // Highlighted points drawn last (on top) as a small red cross.
    for (const Neighbor& n : highlight) {
        if (n.index >= cloud.size()) {
            continue;
        }
        const Point3& p = cloud[n.index];
        int px = std::clamp(static_cast<int>((p.x - minX) / spanX * (w - 1)), 0,
                            w - 1);
        int py = std::clamp(static_cast<int>((p.y - minY) / spanY * (h - 1)), 0,
                            h - 1);
        for (int d = -2; d <= 2; ++d) {
            const int cx = std::clamp(px + d, 0, w - 1);
            const int cy = std::clamp(py, 0, h - 1);
            pixels[static_cast<std::size_t>(h - 1 - cy) * w + cx] =
                Rgb{255, 40, 40};
            const int cx2 = std::clamp(px, 0, w - 1);
            const int cy2 = std::clamp(py + d, 0, h - 1);
            pixels[static_cast<std::size_t>(h - 1 - cy2) * w + cx2] =
                Rgb{255, 40, 40};
        }
    }

    writePpm(path, w, h, pixels);
}

} // namespace

void renderFrame(const PointCloud& cloud,
                 const std::vector<Neighbor>& highlight,
                 const RenderOptions& opts) {
#ifdef PCP_ENABLE_VIZ
    if (opts.interactive) {
        auto o3dCloud = std::make_shared<open3d::geometry::PointCloud>();
        o3dCloud->points_.reserve(cloud.size());
        for (const Point3& p : cloud.points()) {
            o3dCloud->points_.emplace_back(p.x, p.y, p.z);
        }
        o3dCloud->PaintUniformColor({0.8, 0.8, 0.8});
        for (const Neighbor& n : highlight) {
            if (n.index < o3dCloud->colors_.size()) {
                o3dCloud->colors_[n.index] = {1.0, 0.15, 0.15};
            }
        }
        open3d::visualization::DrawGeometries({o3dCloud}, "PointCloudPipeline");
        return;
    }
#endif
    if (!opts.outputImage.has_value()) {
        throw std::runtime_error(
            "renderFrame: offscreen mode requires RenderOptions::outputImage");
    }
    renderOffscreen(cloud, highlight, opts);
}

} // namespace pcp::viz
