/// \file synthetic.hpp
/// \brief Deterministic synthetic point-cloud generator.
///
/// Additive helper (not in the original design interfaces) required by the PM
/// decision that tests and a demo must run without any external download, and
/// by R-1's synthetic 1M-point fallback. Deterministic for reproducibility
/// (NFR-3).
#ifndef PCP_CORE_SYNTHETIC_HPP
#define PCP_CORE_SYNTHETIC_HPP

#include <cstddef>

#include "pcp/core/types.hpp"

namespace pcp {

/// Generate \p count points uniformly in the cube [-extent, extent]^3.
/// \param seed fixed RNG seed for reproducible benchmarks.
[[nodiscard]] PointCloud makeSyntheticCloud(std::size_t count,
                                            unsigned seed = 42u,
                                            Scalar extent = 50.0f);

} // namespace pcp

#endif // PCP_CORE_SYNTHETIC_HPP
