# PointCloudPipeline

A C++17 portfolio demo that ingests a point-cloud frame, builds a **balanced 3D
k-d tree**, answers **k-nearest-neighbor** and **obstacle-avoidance (radius)**
queries, benchmarks the indexed query against a **brute-force baseline**,
verifies correctness, reports **throughput** and the **hardware** it ran on, and
writes an **offscreen visualization** image — targeting realistic **1M+ point**
frames.

It reuses and extends two MIT-licensed upstream repos
([KDTree](https://github.com/vlantonov/KDTree),
[QuadTreeSample](https://github.com/vlantonov/QuadTreeSample)); see
[`NOTICES.md`](NOTICES.md) for attribution.

## Architecture

Each pipeline stage is an independently testable static library; the heavy,
optional visualization dependency never contaminates the core.

```
pcp_cli ──> pcp_io ──┐
        ──> pcp_index ├─> pcp_core
        ──> pcp_query ┘   (Point3, PointCloud, Aabb, HardwareInfo)
        ──> pcp_bench
        ──> pcp_viz (default PPM; optional Open3D behind PCP_ENABLE_VIZ)
```

`pcp_query` and `pcp_bench` are written once against the `SpatialIndex`
interface, so they run unchanged over both `KdTree3d` and `BruteForceIndex` —
which is what makes the correctness comparison mock-free.

## Build & test (Linux, offline)

Requires Conan 2, CMake ≥ 3.20 and Clang 18 (a profile is provided).

```bash
conan install . \
  --profile:host conan/profiles/linux-clang18 \
  --profile:build conan/profiles/linux-clang18 \
  --build=missing -s:h build_type=Debug -s:b build_type=Debug

cmake --preset conan-debug
cmake --build --preset conan-debug --parallel

ctest --test-dir build/Debug --output-on-failure
```

A Release build (used for the throughput numbers) swaps `build_type=Debug` for
`build_type=Release` and uses `--preset conan-release`.

Nothing above requires network access (other than Conan fetching pinned packages
once) or a display. The optional Open3D interactive viewer is **OFF by default**.

## Run the demo

Fully offline synthetic run (no dataset needed):

```bash
./build/Release/pcp_cli --generate 1000000 --query 0,0,0 --k 8 --radius 5
```

Tiny redistributable sample:

```bash
./build/Release/pcp_cli --input data/sample.pcd --query 0,0,0 --k 4 --radius 2
```

KITTI Velodyne frame (optional — see [`data/README.md`](data/README.md)):

```bash
./build/Release/pcp_cli --input data/kitti/aggregated_00_10.bin --format kitti \
                        --query 0,0,0 --k 8 --radius 5
```

### CLI contract

```
pcp_cli --input <file> [--format auto|kitti|pcd|ply]
        --query <x,y,z>
        [--k <int=8>] [--radius <float>] [--tolerance <float=1e-4>]
        [--generate <N>] [--seed <int=42>]
        [--visualize] [--no-visualize] [--viz-output <path=frame.ppm>]
        [--repeat <int=1>] [--help]
```

Output covers: loaded/generated point count (FR-1), `dimensionality=3D` (FR-2),
index build time (FR-3), indexed-vs-baseline timings + `PASS/FAIL` correctness
(FR-7), throughput in points/sec (FR-9), a `HardwareInfo` block (FR-10), and the
offscreen image path (FR-8).

## Build options

| Option | Default | Effect |
|---|---|---|
| `PCP_ENABLE_VIZ` | OFF | Compile the optional Open3D interactive path (adds the Open3D dependency). The default PPM offscreen renderer is always built. |
| `PCP_ENABLE_SPDLOG` | ON | Use spdlog for logging (PRIVATE, non-leaking). |
| `PCP_BUILD_TESTS` | ON | Build the GoogleTest suite. |

## Reproducibility (NFR-3)

Benchmarks are reproducible: the synthetic cloud is seeded (`--seed`, default
42), the build type is reported, and every reported number is accompanied by its
`HardwareInfo`. Use a `Release` build for representative timings.

## License

MIT — see [`LICENSE`](LICENSE). Upstream attribution: [`NOTICES.md`](NOTICES.md).
