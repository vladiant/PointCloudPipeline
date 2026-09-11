/// \file args.cpp
/// \brief Implementation of CLI argument parsing (see args.hpp).
#include "args.hpp"

#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace pcp::cli {
namespace {

std::array<float, 3> parseVec3(const std::string& s) {
    std::array<float, 3> v{};
    std::stringstream ss(s);
    std::string token;
    std::size_t i = 0;
    while (std::getline(ss, token, ',') && i < 3) {
        v[i++] = std::stof(token);
    }
    if (i != 3) {
        throw std::runtime_error("--query expects x,y,z (got '" + s + "')");
    }
    return v;
}

std::string requireValue(int argc, char** argv, int& i, const char* flag) {
    if (i + 1 >= argc) {
        throw std::runtime_error(std::string("Missing value for ") + flag);
    }
    return argv[++i];
}

} // namespace

std::string usage() {
    return "pcp_cli --input <file> [--format auto|kitti|pcd|ply]\n"
           "        --query <x,y,z>\n"
           "        [--k <int=8>] [--radius <float>]\n"
           "        [--tolerance <float=1e-4>]\n"
           "        [--generate <N>] [--seed <int=42>]\n"
           "        [--visualize] [--no-visualize] [--viz-output <path>]\n"
           "        [--repeat <int=1>] [--help]\n"
           "\n"
           "If neither --input nor --generate is given, a synthetic 200000-point\n"
           "cloud is generated so the demo runs offline with no dataset.";
}

Args parseArgs(int argc, char** argv) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--help" || a == "-h") {
            args.help = true;
        } else if (a == "--input") {
            args.input = requireValue(argc, argv, i, "--input");
        } else if (a == "--format") {
            args.format = requireValue(argc, argv, i, "--format");
        } else if (a == "--query") {
            args.query = parseVec3(requireValue(argc, argv, i, "--query"));
        } else if (a == "--k") {
            args.k = static_cast<std::size_t>(
                std::stoull(requireValue(argc, argv, i, "--k")));
        } else if (a == "--radius") {
            args.radius = std::stof(requireValue(argc, argv, i, "--radius"));
        } else if (a == "--tolerance") {
            args.tolerance =
                std::stof(requireValue(argc, argv, i, "--tolerance"));
        } else if (a == "--generate") {
            args.generate = static_cast<std::size_t>(
                std::stoull(requireValue(argc, argv, i, "--generate")));
        } else if (a == "--seed") {
            args.seed = static_cast<unsigned>(
                std::stoul(requireValue(argc, argv, i, "--seed")));
        } else if (a == "--visualize") {
            args.visualize = true;
        } else if (a == "--no-visualize") {
            args.visualize = false;
        } else if (a == "--viz-output") {
            args.vizOutput = requireValue(argc, argv, i, "--viz-output");
        } else if (a == "--repeat") {
            args.repeat = static_cast<std::size_t>(
                std::stoull(requireValue(argc, argv, i, "--repeat")));
            if (args.repeat == 0) {
                args.repeat = 1;
            }
        } else {
            throw std::runtime_error("Unknown argument: '" + a + "'\n" +
                                     usage());
        }
    }
    return args;
}

} // namespace pcp::cli
