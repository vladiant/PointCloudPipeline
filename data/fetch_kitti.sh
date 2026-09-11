#!/usr/bin/env bash
#
# fetch_kitti.sh - documented, reproducible acquisition of a ~1M-point frame
# from the KITTI odometry Velodyne dataset (design OQ-3, NFR-3).
#
# KITTI data is NOT committed to this repository (it is large and gated behind
# registration). This script downloads a Velodyne sweep archive and aggregates
# N consecutive sweeps (~8-10 frames, ~100k-130k points each) into a single
# ~1M-point cloud that pcp_cli can consume.
#
# Usage:
#   ./fetch_kitti.sh [SEQUENCE] [NUM_SWEEPS] [OUT_DIR]
#
# Prerequisites: register at https://www.cvlibs.net/datasets/kitti/ and accept
# the license, then set KITTI_VELODYNE_URL to the odometry Velodyne archive URL
# you are entitled to download (the project does not redistribute it).
#
# If you cannot download KITTI, you do NOT need this script: pcp_cli generates a
# synthetic 1M-point cloud offline with:
#   pcp_cli --generate 1000000 --query 0,0,0 --k 8 --radius 5
set -euo pipefail

SEQUENCE="${1:-00}"
NUM_SWEEPS="${2:-10}"
OUT_DIR="${3:-$(dirname "$0")/kitti}"

: "${KITTI_VELODYNE_URL:?Set KITTI_VELODYNE_URL to your entitled odometry Velodyne archive URL}"

mkdir -p "${OUT_DIR}"
ARCHIVE="${OUT_DIR}/velodyne.zip"

echo "Downloading KITTI Velodyne archive..."
curl -L --fail -o "${ARCHIVE}" "${KITTI_VELODYNE_URL}"

echo "Extracting sequence ${SEQUENCE}..."
unzip -o "${ARCHIVE}" "dataset/sequences/${SEQUENCE}/velodyne/*" -d "${OUT_DIR}"

SWEEP_DIR="${OUT_DIR}/dataset/sequences/${SEQUENCE}/velodyne"
AGG="${OUT_DIR}/aggregated_${SEQUENCE}_${NUM_SWEEPS}.bin"

echo "Aggregating the first ${NUM_SWEEPS} sweeps into ${AGG}..."
: > "${AGG}"
count=0
for f in $(ls "${SWEEP_DIR}"/*.bin | sort | head -n "${NUM_SWEEPS}"); do
  cat "${f}" >> "${AGG}"
  count=$((count + 1))
done

echo "Aggregated ${count} sweeps -> ${AGG}"
echo "Run: pcp_cli --input ${AGG} --format kitti --query 0,0,0 --k 8 --radius 5"
