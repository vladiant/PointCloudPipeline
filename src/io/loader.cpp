/// \file loader.cpp
/// \brief Implementation of the frame loader (see loader.hpp).
#include "pcp/io/loader.hpp"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace pcp::io {
namespace {

Format detectByExtension(const std::filesystem::path& file) {
    std::string ext = file.extension().string();
    for (char& c : ext) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (ext == ".bin") {
        return Format::KittiBin;
    }
    if (ext == ".pcd") {
        return Format::Pcd;
    }
    if (ext == ".ply") {
        return Format::Ply;
    }
    throw std::runtime_error("Unknown point-cloud extension: '" + ext +
                             "'. Specify --format explicitly.");
}

PointCloud loadKittiBin(const std::filesystem::path& file) {
    std::ifstream in(file, std::ios::binary | std::ios::ate);
    if (!in) {
        throw std::runtime_error("Cannot open KITTI .bin file: " +
                                 file.string());
    }
    const std::streamsize bytes = in.tellg();
    if (bytes < 0) {
        throw std::runtime_error("Cannot size file: " + file.string());
    }
    // Little-endian float32 quadruples [x, y, z, reflectance].
    constexpr std::size_t kQuad = 4 * sizeof(float);
    if (static_cast<std::size_t>(bytes) % kQuad != 0) {
        throw std::runtime_error(
            "Malformed KITTI .bin: size not a multiple of 16 bytes: " +
            file.string());
    }
    in.seekg(0, std::ios::beg);

    const std::size_t n = static_cast<std::size_t>(bytes) / kQuad;
    PointCloud cloud;
    cloud.reserve(n);
    float quad[4];
    for (std::size_t i = 0; i < n; ++i) {
        in.read(reinterpret_cast<char*>(quad), kQuad);
        if (!in) {
            throw std::runtime_error("Truncated KITTI .bin file: " +
                                     file.string());
        }
        cloud.push_back(Point3{static_cast<Scalar>(quad[0]),
                               static_cast<Scalar>(quad[1]),
                               static_cast<Scalar>(quad[2])});
    }
    return cloud;
}

PointCloud loadPcdAscii(const std::filesystem::path& file) {
    std::ifstream in(file);
    if (!in) {
        throw std::runtime_error("Cannot open .pcd file: " + file.string());
    }
    std::string line;
    bool dataReached = false;
    while (std::getline(in, line)) {
        if (line.rfind("DATA", 0) == 0) {
            if (line.find("ascii") == std::string::npos) {
                throw std::runtime_error(
                    "Only ASCII .pcd is supported (found: " + line + ")");
            }
            dataReached = true;
            break;
        }
    }
    if (!dataReached) {
        throw std::runtime_error("Malformed .pcd: missing DATA section: " +
                                 file.string());
    }

    PointCloud cloud;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream ls(line);
        float x{}, y{}, z{};
        if (!(ls >> x >> y >> z)) {
            throw std::runtime_error("Malformed .pcd data row: '" + line + "'");
        }
        cloud.push_back(Point3{static_cast<Scalar>(x), static_cast<Scalar>(y),
                               static_cast<Scalar>(z)});
    }
    return cloud;
}

PointCloud loadPlyAscii(const std::filesystem::path& file) {
    std::ifstream in(file);
    if (!in) {
        throw std::runtime_error("Cannot open .ply file: " + file.string());
    }
    std::string line;
    if (!std::getline(in, line) || line.rfind("ply", 0) != 0) {
        throw std::runtime_error("Not a PLY file: " + file.string());
    }
    std::size_t vertexCount = 0;
    bool asciiFormat = false;
    bool headerEnded = false;
    while (std::getline(in, line)) {
        if (line.rfind("format", 0) == 0) {
            asciiFormat = line.find("ascii") != std::string::npos;
        } else if (line.rfind("element vertex", 0) == 0) {
            std::istringstream ls(line);
            std::string a, b;
            ls >> a >> b >> vertexCount;
        } else if (line.rfind("end_header", 0) == 0) {
            headerEnded = true;
            break;
        }
    }
    if (!headerEnded) {
        throw std::runtime_error("Malformed .ply: missing end_header: " +
                                 file.string());
    }
    if (!asciiFormat) {
        throw std::runtime_error("Only ASCII .ply is supported: " +
                                 file.string());
    }

    PointCloud cloud;
    cloud.reserve(vertexCount);
    for (std::size_t i = 0; i < vertexCount; ++i) {
        if (!std::getline(in, line)) {
            throw std::runtime_error("Truncated .ply: expected " +
                                     std::to_string(vertexCount) + " vertices");
        }
        std::istringstream ls(line);
        float x{}, y{}, z{};
        if (!(ls >> x >> y >> z)) {
            throw std::runtime_error("Malformed .ply vertex row: '" + line +
                                     "'");
        }
        cloud.push_back(Point3{static_cast<Scalar>(x), static_cast<Scalar>(y),
                               static_cast<Scalar>(z)});
    }
    return cloud;
}

} // namespace

LoadResult load(const std::filesystem::path& file, Format fmt) {
    if (!std::filesystem::exists(file)) {
        throw std::runtime_error("Input file does not exist: " + file.string());
    }
    const Format resolved = (fmt == Format::Auto) ? detectByExtension(file) : fmt;

    PointCloud cloud;
    switch (resolved) {
    case Format::KittiBin:
        cloud = loadKittiBin(file);
        break;
    case Format::Pcd:
        cloud = loadPcdAscii(file);
        break;
    case Format::Ply:
        cloud = loadPlyAscii(file);
        break;
    case Format::Auto:
        // Unreachable: resolved above.
        throw std::runtime_error("Unresolved format");
    }

    LoadResult result;
    result.pointCount = cloud.size();
    result.detectedFormat = resolved;
    result.cloud = std::move(cloud);
    return result;
}

} // namespace pcp::io
