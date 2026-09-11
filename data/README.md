# Datasets

No dataset is committed to this repository. Three input paths are supported.

## 1. Offline synthetic cloud (no download — default)

`pcp_cli` generates a deterministic synthetic cloud when no `--input` is given.
This is what the unit tests and the default demo use, so the project builds,
tests and runs **fully offline**:

```bash
pcp_cli --generate 1000000 --query 0,0,0 --k 8 --radius 5
```

## 2. Tiny redistributable sample (`sample.pcd`)

`data/sample.pcd` is a 24-point freely-redistributable ASCII cloud used for the
visualization / IO smoke demo:

```bash
pcp_cli --input data/sample.pcd --query 0,0,0 --k 4 --radius 2
```

## 3. KITTI Velodyne `.bin` (optional, requires registration)

The KITTI odometry Velodyne dataset provides realistic LiDAR sweeps
(~100k–130k points each). A single sweep is below the 1M-point target, so
`fetch_kitti.sh` aggregates ~10 consecutive sweeps into one ~1M-point frame
(design OQ-3, NFR-3).

1. Register and accept the license at
   <https://www.cvlibs.net/datasets/kitti/>.
2. Export the URL of the odometry Velodyne archive you are entitled to:
   ```bash
   export KITTI_VELODYNE_URL="<your entitled archive URL>"
   ./data/fetch_kitti.sh 00 10 ./data/kitti
   ```
3. Run:
   ```bash
   pcp_cli --input data/kitti/aggregated_00_10.bin --format kitti \
           --query 0,0,0 --k 8 --radius 5
   ```

KITTI `.bin` layout: little-endian `float32` quadruples `[x, y, z, reflectance]`;
the loader reads `x/y/z` and discards reflectance.
