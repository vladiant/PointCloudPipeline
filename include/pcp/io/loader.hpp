/// \file loader.hpp
/// \brief Load a single point-cloud frame from disk into a PointCloud
///        (FR-1, FR-2).
#ifndef PCP_IO_LOADER_HPP
#define PCP_IO_LOADER_HPP

#include <cstddef>
#include <filesystem>

#include "pcp/core/types.hpp"

namespace pcp::io {

/// Supported input formats. \c Auto detects by file extension.
enum class Format { Auto, KittiBin, Pcd, Ply };

/// Result of a successful load.
struct LoadResult {
    PointCloud cloud;
    std::size_t pointCount{}; ///< Reported to satisfy the FR-1 acceptance test.
    Format detectedFormat{};
};

/// Load \p file into a PointCloud.
///
/// KITTI \c .bin layout: little-endian float32 quadruples [x, y, z, reflectance];
/// x/y/z are read and reflectance discarded. ASCII \c .pcd / \c .ply are parsed
/// for their first three coordinate columns.
///
/// \throws std::runtime_error on unreadable or malformed input (system boundary).
[[nodiscard]] LoadResult load(const std::filesystem::path& file,
                              Format fmt = Format::Auto);

} // namespace pcp::io

#endif // PCP_IO_LOADER_HPP
