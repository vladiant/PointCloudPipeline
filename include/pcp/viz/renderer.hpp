/// \file renderer.hpp
/// \brief Offscreen (and optionally interactive) rendering of a frame + query
///        result (FR-8).
///
/// The header is deliberately free of any Open3D type: the default custom PPM
/// renderer needs no third-party dependency, and the optional interactive
/// Open3D path (PCP_ENABLE_VIZ) stays hidden behind the .cpp (design OQ-4).
#ifndef PCP_VIZ_RENDERER_HPP
#define PCP_VIZ_RENDERER_HPP

#include <filesystem>
#include <optional>
#include <vector>

#include "pcp/core/types.hpp"
#include "pcp/index/spatial_index.hpp"

namespace pcp::viz {

/// Rendering options.
struct RenderOptions {
    bool interactive{false};                          ///< window vs. offscreen
    std::optional<std::filesystem::path> outputImage; ///< e.g. frame.ppm
    int width{960};
    int height{720};
};

/// Render \p cloud, highlighting the \p highlight points (e.g. a query result).
///
/// The default path writes an orthographic-projection PPM image to
/// \c opts.outputImage. An interactive window is only attempted when built with
/// PCP_ENABLE_VIZ; otherwise the offscreen image is written and a note printed.
///
/// \throws std::runtime_error if an output image path is required but cannot be
///         written (system boundary).
void renderFrame(const PointCloud& cloud,
                 const std::vector<Neighbor>& highlight,
                 const RenderOptions& opts);

} // namespace pcp::viz

#endif // PCP_VIZ_RENDERER_HPP
