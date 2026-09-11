/// \file args.hpp
/// \brief CLI argument parsing for pcp_cli (design section 9).
#ifndef PCP_CLI_ARGS_HPP
#define PCP_CLI_ARGS_HPP

#include <array>
#include <cstddef>
#include <optional>
#include <string>

namespace pcp::cli {

/// Parsed command-line options.
struct Args {
    std::optional<std::string> input; ///< input file; if absent, generate
    std::string format = "auto";      ///< auto|kitti|pcd|ply
    std::optional<std::array<float, 3>> query; ///< query location
    std::size_t k = 8;
    std::optional<float> radius; ///< obstacle-avoidance radius
    float tolerance = 1e-4f;
    std::size_t generate = 0; ///< synthetic point count when no --input
    unsigned seed = 42;
    bool visualize = true;
    std::string vizOutput = "frame.ppm";
    std::size_t repeat = 1;
    bool help = false;
};

/// Parse \p argc / \p argv into Args. \throws std::runtime_error on bad input.
[[nodiscard]] Args parseArgs(int argc, char** argv);

/// Usage text for --help and error messages.
[[nodiscard]] std::string usage();

} // namespace pcp::cli

#endif // PCP_CLI_ARGS_HPP
