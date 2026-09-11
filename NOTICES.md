# Third-Party Notices & Attribution

PointCloudPipeline reuses and adapts code from two MIT-licensed upstream
repositories by the same author. Their license texts are preserved verbatim
under `third_party/`.

## Adapted upstream sources

| Upstream repo | URL | License | What was adapted |
|---|---|---|---|
| KDTree | https://github.com/vlantonov/KDTree | MIT | The recursive node / `unique_ptr` chain, split-on-dimension logic and range-search pruning idea were adapted into `pcp_index` (`KdTree3d`). Extended from **2D to 3D** (`dim % 3`), given a **bulk median-split builder** and a **first-class k-NN** search — none of which existed upstream. |
| QuadTreeSample | https://github.com/vlantonov/QuadTreeSample | MIT | Referenced as a 2D spatial-index sample. **Not** used in the shipped code path (kept as an attributed reference only, per design OQ-1). |

License files:
- `third_party/KDTree/LICENSE`
- `third_party/QuadTreeSample/LICENSE`

Each adapted source file (`src/index/kd_tree_3d.cpp`,
`include/pcp/index/kd_tree_3d.hpp`, `include/pcp/core/types.hpp`) carries a
short header comment crediting the upstream KDTree repository (FR-11 / NFR-7).

The project's own `LICENSE` (MIT) is compatible with both upstream licenses.

## Third-party build dependencies (via Conan / ConanCenter)

| Dependency | Version | Role | License |
|---|---|---|---|
| GoogleTest (`gtest`) | 1.14.0 | unit-test framework (test-only) | BSD-3-Clause |
| spdlog | 1.14.1 | optional logging (PRIVATE) | MIT |
| Open3D (`open3d`) | 0.18.0 | optional interactive viz (`PCP_ENABLE_VIZ` only) | MIT |

## Deviations from the design document

The implementation follows `docs/design/point-cloud-pipeline-design.md` except
for the following small, documented deviations (all agreed by the design's own
open-question resolutions and the PM decisions):

1. **`Aabb` accessor naming.** The design's `Aabb` declared both `Point3 min/max`
   data members *and* `Scalar min(dim)/max(dim)` accessor methods — a name clash
   that cannot compile. The data members `min`/`max` are kept; the per-axis
   accessors are renamed `minCoord(dim)` / `maxCoord(dim)`.
2. **`pcp_viz` is always built.** Section 4.2 gated the whole `pcp_viz` target
   behind `PCP_ENABLE_VIZ` with a stub otherwise. Because the default custom PPM
   renderer needs **no** third-party dependency (design OQ-4) and the PM requires
   an offscreen image as the default output, `pcp_viz` is always compiled and
   `PCP_ENABLE_VIZ` now gates **only** the optional Open3D interactive path
   inside it. The `renderer_stub.cpp` file is therefore unnecessary and omitted.
3. **Synthetic generator added.** `pcp::makeSyntheticCloud` (in `pcp_core`) is an
   additive helper, not in the original interface list, required by the PM
   decision that tests and a demo run offline and by risk R-1's synthetic 1M
   fallback.
4. **CLI `--generate` / `--seed` / `--no-visualize` flags** were added to the
   section 9 contract to support the offline synthetic path; the original flags
   are all still honored.
