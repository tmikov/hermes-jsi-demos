#!/usr/bin/env bash
# Build everything: Hermes at the pinned commit (if not built yet), then every demo.
#   DEMOS_BUILD_DIR   where the demos are built (default: build)
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
export HERMES_SRC_DIR="${HERMES_SRC_DIR:-$root/.hermes/src}"
export HERMES_BUILD_DIR="${HERMES_BUILD_DIR:-$root/.hermes/build}"
out="${DEMOS_BUILD_DIR:-$root/build}"

if [ ! -x "$HERMES_BUILD_DIR/bin/hermes" ]; then
  "$root/scripts/build-hermes.sh"
fi
cmake -S "$root" -B "$out" -G Ninja \
  -DHERMES_SRC_DIR="$HERMES_SRC_DIR" \
  -DHERMES_BUILD_DIR="$HERMES_BUILD_DIR"
cmake --build "$out" --target demos
echo "Built. Run the demos and check their output with: scripts/test.sh"
