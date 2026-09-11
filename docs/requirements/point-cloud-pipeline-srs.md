# Software Requirements Specification — PointCloudPipeline

- **Project:** PointCloudPipeline
- **Stage:** Requirement Analysis
- **Status:** Draft for System Architect hand-off
- **Date:** 2026-09-12

---

## 1. Overview

### 1.1 Purpose
PointCloudPipeline is a C++ portfolio demo that ingests a realistic point-cloud
frame, builds a spatial index, answers spatial queries (nearest-neighbor and/or
obstacle-avoidance), benchmarks the indexed query against a brute-force baseline,
visualizes a sample frame, and reports achieved throughput. Its goal is to
demonstrate — for a portfolio audience — the ability to handle realistic data
volumes (target **1M+ points per frame**) by reusing and extending the author's
existing spatial-index code.

### 1.2 Scope
The demo is built **on top of two existing MIT-licensed repositories by the same
author**:

| Upstream repo | Purpose | Dimensionality (as-is) | Provided API (as-is) | License |
|---|---|---|---|---|
| [KDTree](https://github.com/vlantonov/KDTree) | k-d tree sample | **2D** (`Point{x, y}`, `float`) | `insertPoint`, `findPoint` (exact match), `findPointsInArea(Rectangle)` (range), `deletePoint`, `getAllPoints`, `getDepth` | MIT |
| [QuadTreeSample](https://github.com/vlantonov/QuadTreeSample) | quadtree sample | **2D** (`area_quadtree`, `point_quadtree` variants) | point-insert and area-query style operations | MIT |

**Grounded observations that shape scope (not assumptions):**
- Both upstream repos are **sample programs** (single-`main.cpp` style), not
  packaged reusable libraries; extracting a reusable index component is implied
  work.
- The KDTree sample is **2D despite its name** (`operator[]` cycles over two
  dimensions); it stores `float x, y`.
- **Neither upstream provides a k-nearest-neighbor query today.** KDTree offers
  exact-match lookup and rectangular range search only. A true k-NN and/or 3D
  index is new work to be scoped by the System Architect.
- Both licenses are **MIT**; downstream stages must preserve attribution and the
  MIT notice for any reused code.

### 1.3 Out of Scope
- OS-1: Production-grade or real-time streaming point-cloud processing.
- OS-2: GPU acceleration.
- OS-3: Full SLAM, mapping, segmentation, or ML-based perception.
- OS-4: Multi-frame temporal tracking; the demo processes a single sample frame.
- OS-5: A general-purpose, API-stable spatial-index library for third parties.
- OS-6: Windows/macOS support in the first iteration (Linux-first; see NFR-2).
- OS-7: Cross-format dataset conversion tooling beyond the one chosen input path.

---

## 2. Functional Requirements

Each requirement is independently testable. See §7 for mapped acceptance criteria.

- **FR-1 (Dataset ingestion):** The demo shall read a single point-cloud frame
  from a file into an in-memory point set. Candidate input formats to be
  documented/decided downstream: ASCII/binary `.ply`, `.pcd`, or KITTI-style
  `.bin`. The chosen format(s) and their parsing must be documented.
- **FR-2 (Point representation):** The demo shall represent ingested points with
  explicit coordinate dimensionality (2D or 3D) and record which dimensionality
  is used, because the reused indices are currently 2D (see §5, §6).
- **FR-3 (Spatial index build):** The demo shall build a spatial index over the
  ingested points by reusing and/or adapting the existing KDTree and/or
  QuadTreeSample code.
- **FR-4 (Nearest-neighbor query):** The demo shall answer a nearest-neighbor
  query (nearest point, or k-nearest points, to a given query location) using the
  spatial index. Because no k-NN exists upstream, this query is new work.
- **FR-5 (Obstacle-avoidance query):** The demo shall answer at least one
  obstacle-avoidance-style query (e.g. "are there points within radius R of a
  location / along a path") using the spatial index. FR-4 and FR-5 may be
  satisfied independently or together; at least one of the two must be delivered,
  and the delivered set must be documented.
- **FR-6 (Brute-force baseline):** The demo shall implement a brute-force
  (linear-scan) baseline that answers the same query as FR-4 and/or FR-5 and
  produces results usable for correctness comparison.
- **FR-7 (Benchmark & comparison):** The demo shall benchmark the indexed query
  against the brute-force baseline over the same input frame and report both the
  timing of each and the correctness match between them.
- **FR-8 (Visualization):** The demo shall visualize a sample frame (the point
  cloud and, where meaningful, the query result). Candidate technologies to be
  documented: Open3D, or a simple custom point renderer. The final tool choice is
  deferred (see §6).
- **FR-9 (Throughput reporting):** The demo shall report achieved throughput in
  points/frame for the processed frame.
- **FR-10 (Hardware reporting):** The demo shall record and report the hardware
  used for the benchmark run (at minimum CPU model and available RAM).
- **FR-11 (License attribution):** The project shall retain the MIT license and
  author attribution for any code reused from the two upstream repositories.

---

## 3. Non-Functional Requirements

- **NFR-1 (Performance target):** The pipeline shall be capable of building the
  index and answering the query for a frame of **1,000,000+ points**. The indexed
  query shall demonstrate a measurable speedup over the brute-force baseline on
  such a frame.
- **NFR-2 (Portability — Linux first):** The project shall build and run on Linux
  with a mainstream compiler (GCC or Clang) and CMake. Other platforms are out of
  scope for the first iteration (OS-6).
- **NFR-3 (Reproducible benchmarks):** Benchmark runs shall be reproducible: the
  input frame, build configuration (e.g. Release), and any random seeds shall be
  fixed and documented so a reader can re-run and obtain comparable results.
- **NFR-4 (Recorded environment):** Every reported benchmark number shall be
  accompanied by the hardware and build configuration under which it was produced
  (supports FR-10).
- **NFR-5 (Correctness of speedup claim):** The indexed query result shall match
  the brute-force baseline result (within a documented tolerance for
  floating-point distances) so that reported speedups compare equivalent work.
- **NFR-6 (Portfolio clarity):** The demo shall be runnable via a documented,
  small number of steps (build + run + observe output), suitable for a portfolio
  reviewer.
- **NFR-7 (License compliance):** Reuse of upstream code shall comply with the MIT
  terms of both source repositories (supports FR-11).

---

## 4. Data & Interface Assumptions

- **DA-1 (Coordinates):** Point coordinates are floating-point. Upstream KDTree
  uses `float`; the choice of `float` vs `double` for the pipeline is deferred.
- **DA-2 (Dimensionality gap):** Real LiDAR/RGB-D clouds are **3D**, but both
  reused indices are currently **2D**. The pipeline must either (a) project/reduce
  to 2D, or (b) extend an index to 3D. This choice is deferred (see §6, OQ-1).
- **DA-3 (Frame size):** A representative frame is on the order of 1M+ points
  (e.g. one KITTI Velodyne sweep is ~100k–130k points; a target of 1M+ may
  require a denser dataset or aggregated frame — to be confirmed, OQ-3).
- **DA-4 (Single-file input):** Input is a single local file per frame; no network
  ingestion is assumed.
- **DA-5 (Query input):** A query is defined by a location (and a radius or k),
  supplied via a documented mechanism (CLI argument, config, or fixed constant) —
  exact mechanism deferred to design.
- **DA-6 (Upstream is sample code):** Reused code will likely need refactoring
  from `main.cpp` demos into a callable component; treat upstream as source to
  adapt, not as a stable dependency to link against.

---

## 5. Constraints & Assumptions

- **C-1:** Built on the two named MIT-licensed upstream repos; attribution and
  license must be preserved.
- **C-2:** C++ with CMake build and a unit-test framework (GoogleTest or Catch2),
  consistent with the portfolio's conventions.
- **C-3:** Upstream KDTree targets C++17-level features (`std::optional`,
  `[[nodiscard]]`); the pipeline may assume C++17 or newer.
- **C-4:** Single-frame, single-machine, CPU-only scope (see §1.3).
- **C-5:** The k-NN query (FR-4) is **not** provided by upstream and is net-new.

---

## 6. Open Questions (deferred to the System Architect)

- **OQ-1 (Index vs query dimensionality):** KDTree and QuadTree are both 2D today.
  Which index serves which query — extend KDTree to 3D for nearest-neighbor, use
  QuadTree for a 2D top-down obstacle-avoidance view, or project 3D→2D? This is
  the central architectural decision.
- **OQ-2 (k-NN implementation):** No nearest-neighbor search exists upstream. Is
  k-NN added to the adapted KDTree, or is FR-4 satisfied via radius/range search
  built on the existing `findPointsInArea`-style primitive?
- **OQ-3 (Dataset source & format):** Which concrete dataset and format
  (`.ply` / `.pcd` / KITTI `.bin`)? Does a single native frame reach 1M+ points,
  or must frames be aggregated/synthesized to hit the target?
- **OQ-4 (Visualization dependency):** Open3D (heavier dependency, richer output)
  vs a minimal custom renderer (lighter, more code). Trade-off to be decided.
- **OQ-5 (Coordinate precision):** `float` (matches upstream, less memory at 1M
  points) vs `double` (more precise distances). Impacts NFR-1 memory budget.
- **OQ-6 (Which queries ship):** FR-4, FR-5, or both? At least one is required;
  scope which to deliver.
- **OQ-7 (Reuse strategy):** Vendor/copy the upstream sources into this repo, or
  fork/submodule them, given they are sample programs rather than libraries?

---

## 7. Acceptance Criteria

| Req | Acceptance criterion |
|---|---|
| FR-1 | Given a sample point-cloud file in the chosen format, running the demo loads all points and reports the loaded point count matching the file. |
| FR-2 | The demo output/log states the coordinate dimensionality (2D or 3D) used for the run. |
| FR-3 | The demo builds a spatial index from the loaded points without error and reports index build time. |
| FR-4 | For a given query location, the indexed nearest-neighbor result matches the brute-force nearest neighbor (within tolerance, per NFR-5). |
| FR-5 | For a given location and radius/path, the obstacle-avoidance query returns the set of points matching the brute-force result (within tolerance). |
| FR-6 | A brute-force baseline for the same query exists and produces a reference result. |
| FR-7 | The demo prints timing for both indexed and brute-force queries and a correctness match (pass/fail) between them. |
| FR-8 | Running the visualization step displays or writes an image of the sample frame (and query result where applicable). |
| FR-9 | The demo reports achieved throughput in points/frame for the run. |
| FR-10 | The demo output includes the CPU model and available RAM used for the run. |
| FR-11 | The repository retains the MIT license text and attribution for reused KDTree/QuadTree code. |
| NFR-1 | A benchmark run on a 1M+ point frame completes and the indexed query is measurably faster than brute force. |
| NFR-2 | The project builds and runs on Linux via CMake with GCC or Clang. |
| NFR-3 | A documented command sequence reproduces the benchmark with fixed input and seed. |
| NFR-4 | Each reported number is accompanied by its hardware/build configuration. |
| NFR-5 | Indexed and brute-force results agree within the documented tolerance. |

---

*Requirements are ready for the System Architect agent to design against.*
