# Changelog

All notable changes to **PointCloudPipeline** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] — 2026-09-12

Initial public release. A C++17 point-cloud processing demo that ingests a
frame, builds a balanced 3D k-d tree, answers k-nearest-neighbor and
obstacle-avoidance (radius) queries, benchmarks the indexed query against a
brute-force baseline, verifies correctness, reports throughput and hardware, and
writes an offscreen visualization — targeting realistic **1,000,000+ point**
frames.

### Added

- **`pcp_core`** — `Point3` / `PointCloud` / `Aabb` types, `HardwareInfo`
  reporting (CPU model + RAM), and a seeded synthetic-cloud generator for fully
  offline runs.
- **`pcp_io`** — loaders for KITTI Velodyne `.bin` and ASCII `.pcd` / `.ply`,
  with format auto-detection.
- **`pcp_index`** — a shared `SpatialIndex` interface, a 3D `KdTree3d`
  (median-split bulk builder, max-heap k-NN search, and radius search) adapted
  and extended from the upstream MIT `KDTree`, plus a `BruteForceIndex` linear
  baseline. Both indices implement the same interface, so queries and benchmarks
  run unchanged over either (mock-free correctness comparison).
- **`pcp_query`** — k-NN and obstacle-avoidance / radius queries written once
  against `SpatialIndex`.
- **`pcp_bench`** — timing / throughput / correctness harness comparing indexed
  vs. baseline results.
- **`pcp_viz`** — offscreen PPM renderer built by default (no third-party
  dependency); optional Open3D interactive path behind `PCP_ENABLE_VIZ`.
- **`pcp_cli`** — driver executable wiring load/generate → build → query →
  benchmark → report → offscreen visualization, with
  `--generate` / `--seed` / `--visualize` / `--no-visualize` / `--viz-output`
  flags for offline, reproducible demo runs.
- GoogleTest suite (**35/35 passing**); clean under ASan + UBSan.
- Conan 2 + CMake presets (`conan-debug` / `conan-release`) with a Clang 18
  profile; fully offline build and test.
- CI: GitHub Actions (build-test Debug + Release, sanitizers, CPack TGZ package)
  with a GitLab mirror.
- `NOTICES.md` attributing the vendored MIT `KDTree` / `QuadTreeSample` upstream
  sources under `third_party/`.

### Benchmark headline

Measured on an Intel Core i7-6700 @ 3.40 GHz (4C/8T, ~15.5 GiB RAM),
Clang 18.1.3, Release, over **1,000,000 synthetic points/frame** (which **meets
the 1M+ points/frame target**):

- k-NN (k=8): **~480× faster** than brute force (~34 µs vs ~15 ms), correctness
  PASS vs baseline.
- Radius / obstacle query: **~28× faster** than brute force, correctness PASS.
- k-d tree build: ~0.68–0.75 s; a 960×720 offscreen PPM image emitted.

### Notes

- `--repeat` is parsed and validated but not yet wired into timing (queries are
  measured once); multi-run averaging is a planned follow-up.
- This release is tagged `v0.1.0` as part of release close-out (tag published by
  the maintainer, not by this changelog entry).

[0.1.0]: https://github.com/vlantonov/PointCloudPipeline/releases/tag/v0.1.0
