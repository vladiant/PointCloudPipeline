/// \file hardware_info.hpp
/// \brief Records the machine + build configuration for a benchmark run
///        (FR-10 / NFR-4).
#ifndef PCP_CORE_HARDWARE_INFO_HPP
#define PCP_CORE_HARDWARE_INFO_HPP

#include <cstdint>
#include <ostream>
#include <string>

namespace pcp {

/// Snapshot of the environment a benchmark number was produced under.
struct HardwareInfo {
    std::string cpuModel;          ///< Parsed from /proc/cpuinfo (Linux-first).
    std::uint64_t totalRamBytes{}; ///< Parsed from /proc/meminfo.
    unsigned hardwareThreads{};    ///< std::thread::hardware_concurrency().
    std::string compiler;          ///< e.g. "Clang 18.1.3", captured at build.
    std::string buildType;         ///< e.g. "Release".
};

/// Query the current machine's hardware/build info.
[[nodiscard]] HardwareInfo queryHardwareInfo();

std::ostream& operator<<(std::ostream& os, const HardwareInfo& info);

} // namespace pcp

#endif // PCP_CORE_HARDWARE_INFO_HPP
