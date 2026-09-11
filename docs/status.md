# Project Status — PointCloudPipeline

**Goal:** Point-cloud processing demo handling 1M+ points/frame, built on the existing
[KDTree](https://github.com/vlantonov/KDTree) and [QuadTreeSample](https://github.com/vlantonov/QuadTreeSample)
work. Read a sample LiDAR/RGB-D dataset, build a spatial index, run a nearest-neighbor /
obstacle-avoidance query, benchmark against a brute-force baseline, visualize a sample frame,
and report achieved throughput (points/frame) and hardware used.

## Pipeline (new-project chain)

| Stage | Owner agent | Status |
|-------|-------------|--------|
| Requirements | Requirements Analyst | ✅ Done |
| Design | System Architect | ✅ Done |
| Development | Cpp Developer | 🔄 In progress |
| QA | QA Engineer | ⬜ Not started |
| Release | Release Engineer | ⬜ Not started |
| Documentation | Technical Writer | ⬜ Not started |

## Process rules for this project
- Commit after each completed stage using the `semver-commit-description` skill.
- At the end, set the version and publish tags using the `semver-version-publish` skill.

## Open questions / blockers
- Deferred to System Architect: index dimensionality (both upstream trees are 2D), whether to add k-NN, dataset/format choice, visualization dependency, float vs double, which queries ship, upstream reuse strategy (vendor vs submodule). **Resolved in design doc.**
- PM decisions on design residuals: (1) do NOT redistribute KITTI in-repo — document the download and ship a small freely-redistributable sample cloud for smoke tests; (2) default visualization output is offscreen PPM/PNG (reproducible/CI-friendly), interactive Open3D optional behind `PCP_ENABLE_VIZ`.

## Change log (PM view)
- 2026-09-12: Scoped as a new portfolio project; kicked off the Requirements stage.
- 2026-09-12: Requirements stage complete (SRS written); handed off to System Architect.
- 2026-09-12: Design stage complete (3D k-d tree + brute-force baseline, GoogleTest, offscreen renderer default, vendor upstream under third_party/); PM resolved dataset-redistribution and viz-output residuals; handed off to Cpp Developer.
