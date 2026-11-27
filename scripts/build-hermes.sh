#!/usr/bin/env bash
# Fetch Hermes at the commit pinned in HERMES_COMMIT and build it (JSI and Node-API).
#   HERMES_SRC_DIR    where the checkout goes   (default: .hermes/src)
#   HERMES_BUILD_DIR  where the build goes      (default: .hermes/build)
#   BUILD_TYPE        Release or Debug          (default: Release)
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
commit="$(tr -d '[:space:]' < "$root/HERMES_COMMIT")"
src="${HERMES_SRC_DIR:-$root/.hermes/src}"
build="${HERMES_BUILD_DIR:-$root/.hermes/build}"
type="${BUILD_TYPE:-Release}"

if [ ! -d "$src/.git" ]; then
  mkdir -p "$src"
  git -C "$src" init -q
  git -C "$src" remote add origin https://github.com/facebook/hermes.git
fi
if [ "$(git -C "$src" rev-parse -q --verify HEAD 2>/dev/null || true)" != "$commit" ]; then
  echo "Fetching Hermes $commit"
  git -C "$src" fetch -q --depth 1 origin "$commit"
  git -C "$src" checkout -q --detach FETCH_HEAD
fi

echo "Building Hermes ($type) in $build"
cmake -S "$src" -B "$build" -G Ninja \
  -DCMAKE_BUILD_TYPE="$type" \
  -DHERMES_BUILD_APPLE_FRAMEWORK=OFF \
  -DHERMES_ENABLE_NAPI=ON \
  -DHERMES_ENABLE_TEST_SUITE=OFF
cmake --build "$build"
