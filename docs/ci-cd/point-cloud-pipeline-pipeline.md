# CI/CD & Release Pipeline — PointCloudPipeline

Owner: Release Engineer. Primary CI is **GitHub Actions**
([`.github/workflows/ci.yml`](../../.github/workflows/ci.yml)); a minimal
**GitLab CI** mirror ([`.gitlab-ci.yml`](../../.gitlab-ci.yml)) is provided for
portability. Both use the exact Conan 2 + CMake-preset flow the repo ships
(Clang 18, `conan/profiles/linux-clang18`), with Open3D **OFF** so CI needs no
heavy/network/display dependencies.

## Pipeline stages

| Job / stage | What it does | Fails the pipeline when |
|---|---|---|
| `build-test` (matrix: Debug, Release) | Install Clang 18 + Conan 2 → `conan install` → `cmake --preset` → `cmake --build` → `ctest --output-on-failure`. Release also runs the CLI and uploads `pcp_cli` + a sample `frame.ppm`. | Any compile or test failure. |
| `sanitizers` (Debug + ASan/UBSan) | Same install, configured with `-fsanitize=address,undefined -fno-omit-frame-pointer -g`, then full `ctest`. Mirrors the QA `build/asan` run. | Any sanitizer finding or test failure. |
| `package` (CPack TGZ) | Release build → `cpack -G TGZ` → uploads `PointCloudPipeline-<version>-Linux.tar.gz`. Gated on `build-test`. | Configure/build/pack failure. |

The QA test stage is the gate: a red pipeline reflects a real problem to fix
upstream, never something to silence.

## Key implementation details

- **Conan cache:** `~/.conan2/p` cached via `actions/cache`, keyed on
  `hashFiles('conanfile.py')`. The CMake build directory is **not** cached —
  Conan's package store is the binary cache.
- **Explicit build_type qualifiers:** `conan install` passes both `-s:h` and
  `-s:b build_type=...`. Omitting `:b` sets the type only on the host profile and
  breaks dependency source builds.
- **Clang 18 aliasing:** the shipped profile references `/usr/bin/clang{,++}`;
  CI symlinks those to the `clang-18` binaries from the Ubuntu 24.04 repos.

## Reproduce locally

```bash
# --- Debug build + tests ---
conan install . \
  --profile:host conan/profiles/linux-clang18 \
  --profile:build conan/profiles/linux-clang18 \
  --build=missing -s:h build_type=Debug -s:b build_type=Debug
cmake --preset conan-debug
cmake --build --preset conan-debug --parallel
ctest --test-dir build/Debug --output-on-failure

# --- Release build ---
conan install . \
  --profile:host conan/profiles/linux-clang18 \
  --profile:build conan/profiles/linux-clang18 \
  --build=missing -s:h build_type=Release -s:b build_type=Release
cmake --preset conan-release
cmake --build --preset conan-release --parallel
ctest --test-dir build/Release --output-on-failure

# --- ASan + UBSan (QA parity) ---
cmake --preset conan-debug \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer -g" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build --preset conan-debug --parallel
ctest --test-dir build/Debug --output-on-failure
```

## Packaging / release artifact

CPack is configured in [`CMakeLists.txt`](../../CMakeLists.txt) (TGZ generator).
It installs the demo CLI (`bin/pcp_cli`) plus `README.md`, `LICENSE`, and
`NOTICES.md` under `share/doc/`.

```bash
# after a Release build
cpack -G TGZ --config build/Release/CPackConfig.cmake -B dist
# -> dist/PointCloudPipeline-0.1.0-Linux.tar.gz
#      PointCloudPipeline-0.1.0-Linux/bin/pcp_cli
#      PointCloudPipeline-0.1.0-Linux/share/doc/PointCloudPipeline/{README.md,LICENSE,NOTICES.md}
```

A plain staged layout is equally available via
`cmake --install build/Release --prefix <dir>`.

## Versioning

The project version lives in [`VERSION`](../../VERSION) (currently `0.1.0`) and
matches `project(... VERSION 0.1.0)` and `conanfile.py`. The version tag is **not**
created here — the PM runs the `semver-version-publish` skill at close-out to bump
`VERSION` and publish the annotated tag.
