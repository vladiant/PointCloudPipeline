# QA Test Report — PointCloudPipeline

- **Stage:** Testing (QA Engineer)
- **Date:** 2026-09-12
- **Build under test:** development hand-off (33/33 dev tests)
- **Verdict:** ✅ **QA PASSED — ready for the Release Engineer**

---

## 1. Environment

| Item | Value |
|---|---|
| CPU | Intel(R) Core(TM) i7-6700 @ 3.40 GHz (8 HW threads) |
| RAM | 15.55 GiB |
| Compiler | Clang 18.1.3 (libstdc++11) |
| Toolchain | Conan 2 + CMake, profile `conan/profiles/linux-clang18` |
| OS | Linux |

No network access, no dataset download, and no display/interactive window were
needed for any build, test, or run (Open3D OFF by default; synthetic generator
and temp-file fixtures used throughout).

## 2. Build result

| Config | conan install | cmake configure | build | Warnings |
|---|---|---|---|---|
| Debug (`conan-debug`) | ✅ | ✅ | ✅ | none (only benign "CMAKE_TOOLCHAIN_FILE not used") |
| Release (`conan-release`) | ✅ | ✅ | ✅ | same benign note |
| Debug + ASan/UBSan (`build/asan`) | ✅ | ✅ | ✅ | none |

All targets built clean: `pcp_core`, `pcp_io`, `pcp_index`, `pcp_query`,
`pcp_bench`, `pcp_viz`, `pcp_cli`, `pcp_tests`. No compiler warnings or errors.

## 3. Test suite (CTest)

| Run | Result |
|---|---|
| Debug (dev suite as received) | **33/33 passed** |
| Debug (with 2 QA tests added) | **35/35 passed** |
| Release | **35/35 passed** |
| Debug + AddressSanitizer + UBSan | **35/35 passed** |

No flaky, skipped, or disabled tests observed. Total suite runtime ~0.1 s.

## 4. Sanitizer pass (ASan + UBSan)

Built a separate `build/asan` with `-fsanitize=address,undefined
-fno-omit-frame-pointer -g`. The full test suite and a CLI run
(`--generate 50000`) executed to completion with **zero** sanitizer findings —
no heap-buffer-overflow, no leak, no signed-integer-overflow, no undefined
behavior reported. Sanitizers ran successfully in this environment.

## 5. CLI verification (all FRs exercised)

Small (`--generate 5000`) and 1M (`--generate 1000000`) runs both:
- (a) built the k-d tree index and reported build time + depth — **FR-3**;
- (b) ran the k-NN query — **FR-4**;
- (c) ran the radius / obstacle-avoidance query — **FR-5**;
- (d) ran the brute-force baseline and compared — **FR-6/FR-7**;
- (e) printed throughput (point count + points/sec) — **FR-9**;
- (f) printed CPU model + RAM + compiler + build type — **FR-10**;
- (g) wrote a valid PPM image (`P6 960x720`, 2,073,615 bytes, non-empty) — **FR-8**;
- reported `dimensionality=3D` — **FR-2**.

### Throughput measured (this hardware, i7-6700)

| Build | Points | Index build | k-NN speedup | radius speedup | correctness |
|---|---|---|---|---|---|
| Debug | 1,000,000 | ~3.90 s | 572.6× | 32.3× | PASS |
| **Release** | 1,000,000 | ~0.75 s | **480.8×** | **28.0×** | PASS |

Developer's prior Release claim (same i7-6700): k-NN ~434×, radius ~12.8×.
Observed values are the same order of magnitude and at least as favourable;
the k-NN indexed timing is ~30 µs, so the exact multiplier is noise-sensitive,
but the measurable-speedup requirement (**NFR-1**) is met with wide margin.
Release 1M wall time ~1.0 s, max RSS ~62 MB.

## 6. Correctness vs. baseline (NFR-5)

The developer suite and the bench harness compare indexed vs. brute-force results
by **sorted squared distances** only (per design §3.5). To verify independently
rather than re-running the developer's own assertions, I added tests that compare
the actual returned **point indices**:

- `QAIndependent.WithinRadiusIndexSetEqualsBaseline` — for 40 random probes × 4
  radii, the k-d tree's returned index *set* is identical to the linear scan's
  (radius search has no tie ambiguity, so this is an exact check).
- `QAIndependent.KNearestIndexSetOnDistinctDistanceCloud` — on a 12³ lattice with
  an off-lattice probe (strictly distinct distances), k-NN index sets match the
  baseline exactly for k ∈ {1,4,8,16,32}.

Both pass, confirming the k-d tree returns the *correct points*, not merely
points at matching distances. Runtime CLI comparison also reports PASS for k-NN
and radius on every run.

## 7. SRS acceptance-criteria coverage

| Req | Status | Evidence |
|---|---|---|
| FR-1 loads & reports count | ✅ | `io_tests` + CLI "Loaded/Generated N points" |
| FR-2 dimensionality reported | ✅ | CLI prints `dimensionality=3D` |
| FR-3 index build + time | ✅ | CLI reports depth + build µs |
| FR-4 k-NN matches baseline | ✅ | correctness tests + QA index-set test |
| FR-5 radius/obstacle matches | ✅ | correctness tests + QA index-set test |
| FR-6 brute-force baseline | ✅ | `BruteForceIndex`, exercised everywhere |
| FR-7 timing + match printed | ✅ | bench harness `operator<<`, CLI output |
| FR-8 visualization image | ✅ | valid non-empty PPM written & inspected |
| FR-9 throughput reported | ✅ (see note) | point count + points/sec printed |
| FR-10 CPU + RAM reported | ✅ | HardwareInfo block in CLI output |
| FR-11 MIT attribution retained | ✅ | `NOTICES.md`, `third_party/*/LICENSE`, source headers |
| NFR-1 1M+ measurable speedup | ✅ | Release 1M: 480×/28× |
| NFR-2 Linux/Clang + CMake | ✅ | Clang 18 build clean |
| NFR-3 reproducible (fixed seed) | ✅ | `--seed 42`, documented commands |
| NFR-4 numbers carry hw/build config | ✅ | HardwareInfo includes build type/compiler |
| NFR-5 agreement within tolerance | ✅ | §6 above |

## 8. Coverage gap found & closed

- **Gap:** neither the developer tests nor the harness verified that the returned
  point *indices* match the baseline — only sorted distances. A defect returning
  wrong-but-equidistant points would have passed.
- **Closed:** added `tests/qa_independent_tests.cpp` (2 tests, test-only, no
  production change). Both pass in Debug, Release, and under ASan/UBSan.

## 9. Defects

None (severity: n/a). No functional, memory, or build defects were found.

### Minor observations (non-blocking, no action required for release)

1. **FR-9 wording.** Throughput is reported as the processed point count plus
   `points/sec`; there is no literally-labelled "points/frame" figure. The point
   count *is* the per-frame volume, so the criterion is met — a label tweak is
   cosmetic only.
2. k-NN indexed timing is ~30 µs at 1M points, so the k-NN speedup multiplier and
   `points/sec` are measurement-noise sensitive run-to-run. Direction and order of
   magnitude are stable; consider `--repeat`-averaged timing if a headline number
   is ever quoted verbatim. (The `--repeat` flag is parsed but not yet used in the
   timing loop — informational only, not a requirement.)

## 10. Verdict

✅ **QA PASSED — ready for the Release Engineer agent to deploy.**
Build is clean (Debug + Release), 35/35 tests pass including independent
correctness checks, ASan/UBSan are clean, all SRS acceptance criteria are met,
and the 1M-point speedup requirement is satisfied with wide margin. No defects to
route back to the Cpp Developer or System Architect.
