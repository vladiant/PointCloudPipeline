/// \file io_tests.cpp
/// \brief Unit tests for the frame loader (KITTI .bin, .pcd, .ply).
#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "pcp/io/loader.hpp"

namespace {

std::filesystem::path tempFile(const std::string& name) {
    return std::filesystem::temp_directory_path() / name;
}

TEST(KittiLoader, ReadsKnownPointCount) {
    const auto path = tempFile("pcp_test_frame.bin");
    {
        std::ofstream out(path, std::ios::binary);
        // 3 points, each x,y,z,reflectance float32.
        const float data[] = {1.0f, 2.0f, 3.0f, 0.5f, //
                              4.0f, 5.0f, 6.0f, 0.6f,  //
                              7.0f, 8.0f, 9.0f, 0.7f};
        out.write(reinterpret_cast<const char*>(data), sizeof(data));
    }
    const auto result = pcp::io::load(path);
    EXPECT_EQ(result.pointCount, 3u);
    EXPECT_EQ(result.detectedFormat, pcp::io::Format::KittiBin);
    ASSERT_EQ(result.cloud.size(), 3u);
    EXPECT_FLOAT_EQ(result.cloud[1].x, 4.0f); // reflectance discarded
    EXPECT_FLOAT_EQ(result.cloud[2].z, 9.0f);
    std::filesystem::remove(path);
}

TEST(KittiLoader, MalformedSizeThrows) {
    const auto path = tempFile("pcp_test_bad.bin");
    {
        std::ofstream out(path, std::ios::binary);
        const float data[] = {1.0f, 2.0f, 3.0f}; // 12 bytes, not multiple of 16
        out.write(reinterpret_cast<const char*>(data), sizeof(data));
    }
    EXPECT_THROW((void)pcp::io::load(path), std::runtime_error);
    std::filesystem::remove(path);
}

TEST(Loader, MissingFileThrows) {
    EXPECT_THROW((void)pcp::io::load(tempFile("pcp_does_not_exist.bin")),
                 std::runtime_error);
}

TEST(Loader, UnknownExtensionThrows) {
    const auto path = tempFile("pcp_test.xyz");
    { std::ofstream out(path); out << "0 0 0\n"; }
    EXPECT_THROW((void)pcp::io::load(path), std::runtime_error);
    std::filesystem::remove(path);
}

TEST(PcdLoader, ReadsAsciiPoints) {
    const auto path = tempFile("pcp_test.pcd");
    {
        std::ofstream out(path);
        out << "# .PCD v0.7\nVERSION 0.7\nFIELDS x y z\nSIZE 4 4 4\n"
               "TYPE F F F\nCOUNT 1 1 1\nWIDTH 2\nHEIGHT 1\n"
               "POINTS 2\nDATA ascii\n"
               "1.0 2.0 3.0\n4.0 5.0 6.0\n";
    }
    const auto result = pcp::io::load(path);
    ASSERT_EQ(result.cloud.size(), 2u);
    EXPECT_FLOAT_EQ(result.cloud[0].x, 1.0f);
    EXPECT_FLOAT_EQ(result.cloud[1].z, 6.0f);
    std::filesystem::remove(path);
}

TEST(PlyLoader, ReadsAsciiVertices) {
    const auto path = tempFile("pcp_test.ply");
    {
        std::ofstream out(path);
        out << "ply\nformat ascii 1.0\nelement vertex 3\n"
               "property float x\nproperty float y\nproperty float z\n"
               "end_header\n0 0 0\n1 1 1\n2 2 2\n";
    }
    const auto result = pcp::io::load(path);
    ASSERT_EQ(result.cloud.size(), 3u);
    EXPECT_FLOAT_EQ(result.cloud[2].y, 2.0f);
    std::filesystem::remove(path);
}

TEST(PlyLoader, BinaryFormatThrows) {
    const auto path = tempFile("pcp_test_bin.ply");
    {
        std::ofstream out(path);
        out << "ply\nformat binary_little_endian 1.0\nelement vertex 1\n"
               "property float x\nproperty float y\nproperty float z\n"
               "end_header\n";
    }
    EXPECT_THROW((void)pcp::io::load(path), std::runtime_error);
    std::filesystem::remove(path);
}

} // namespace
