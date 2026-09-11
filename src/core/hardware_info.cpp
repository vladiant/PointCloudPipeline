/// \file hardware_info.cpp
/// \brief Implementation of hardware/build-configuration reporting (FR-10).
#include "pcp/core/hardware_info.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>

#ifndef PCP_BUILD_TYPE
#define PCP_BUILD_TYPE "unknown"
#endif

namespace pcp {
namespace {

std::string parseCpuModel() {
    std::ifstream in("/proc/cpuinfo");
    std::string line;
    while (std::getline(in, line)) {
        const auto colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        const std::string key = line.substr(0, colon);
        if (key.rfind("model name", 0) == 0) {
            std::string value = line.substr(colon + 1);
            const auto start = value.find_first_not_of(" \t");
            return start == std::string::npos ? std::string{}
                                              : value.substr(start);
        }
    }
    return "unknown";
}

std::uint64_t parseTotalRamBytes() {
    std::ifstream in("/proc/meminfo");
    std::string key;
    std::uint64_t valueKb = 0;
    std::string unit;
    while (in >> key >> valueKb >> unit) {
        if (key == "MemTotal:") {
            return valueKb * 1024ULL;
        }
        std::string rest;
        std::getline(in, rest);
    }
    return 0;
}

std::string detectCompiler() {
    std::ostringstream os;
#if defined(__clang__)
    os << "Clang " << __clang_major__ << '.' << __clang_minor__ << '.'
       << __clang_patchlevel__;
#elif defined(__GNUC__)
    os << "GCC " << __GNUC__ << '.' << __GNUC_MINOR__ << '.'
       << __GNUC_PATCHLEVEL__;
#else
    os << "unknown";
#endif
    return os.str();
}

} // namespace

HardwareInfo queryHardwareInfo() {
    HardwareInfo info;
    info.cpuModel = parseCpuModel();
    info.totalRamBytes = parseTotalRamBytes();
    info.hardwareThreads = std::thread::hardware_concurrency();
    info.compiler = detectCompiler();
    info.buildType = PCP_BUILD_TYPE;
    return info;
}

std::ostream& operator<<(std::ostream& os, const HardwareInfo& info) {
    const std::ios::fmtflags savedFlags = os.flags();
    const std::streamsize savedPrec = os.precision();
    const double ramGiB =
        static_cast<double>(info.totalRamBytes) / (1024.0 * 1024.0 * 1024.0);
    os << std::fixed << std::setprecision(2);
    os << "Hardware / build configuration:\n"
       << "  CPU model    : " << info.cpuModel << '\n'
       << "  Total RAM    : " << ramGiB << " GiB\n"
       << "  HW threads   : " << info.hardwareThreads << '\n'
       << "  Compiler     : " << info.compiler << '\n'
       << "  Build type   : " << info.buildType;
    os.flags(savedFlags);
    os.precision(savedPrec);
    return os;
}

} // namespace pcp
