#!/usr/bin/env bash
# Regenerate docs/screenshots: one headless frame per design tour (Vulkan via
# Mesa lavapipe works; no display needed).
set -euo pipefail
cd "$(dirname "$0")/.."
bazel build //:ivviz
mkdir -p docs/screenshots
./bazel-bin/src/app/ivviz --screenshots="$PWD/docs/screenshots" "$@"
