#!/usr/bin/env bash
# Package a built Hermes (scripts/build-hermes.sh) as a prebuilt archive: headers, libraries, tools, licenses.
#   HERMES_SRC_DIR / HERMES_BUILD_DIR   as in build-hermes.sh
#   DIST_DIR                            where the archive goes (default: dist)
# Output: dist/hermes-<commit12>-<os>-<arch>.tar.gz and its .sha256
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
src="${HERMES_SRC_DIR:-$root/.hermes/src}"
build="${HERMES_BUILD_DIR:-$root/.hermes/build}"
dist="${DIST_DIR:-$root/dist}"
commit="$(tr -d '[:space:]' < "$root/HERMES_COMMIT")"

case "$(uname -s)" in Linux) os=linux; so=so ;; Darwin) os=macos; so=dylib ;; *) echo "unsupported OS" >&2; exit 1 ;; esac
case "$(uname -m)" in x86_64|amd64) arch=x64 ;; arm64|aarch64) arch=arm64 ;; *) arch="$(uname -m)" ;; esac
name="hermes-${commit:0:12}-$os-$arch"
out="$dist/$name"
rm -rf "$out" && mkdir -p "$out"/{include,lib,bin}

# Headers: JSI, the Hermes JSI runtime and its public config, Node-API (the vendored headers and Hermes' host API).
mkdir -p "$out/include/jsi" "$out/include/hermes/Public" "$out/include/hermes/napi" "$out/include/napi"
cp "$src"/API/jsi/jsi/*.h "$out/include/jsi/"
cp "$src"/API/hermes/hermes.h "$out/include/hermes/"
cp "$src"/public/hermes/Public/*.h "$out/include/hermes/Public/"
cp "$src"/include/hermes/napi/*.h "$out/include/hermes/napi/"
cp "$src"/API/napi/hermes_napi.h "$out/include/napi/"
[ -f "$src/API/napi/hermes_napi_compile.h" ] && cp "$src/API/napi/hermes_napi_compile.h" "$out/include/napi/"

# Libraries: the shared JSI runtime and every static library the build produced (the Node-API host links these).
# libhermesvm on Hermes' static_h branch (libhermes on older Hermes).
found_shared=0
while IFS= read -r f; do cp -P "$f" "$out/lib/"; found_shared=1; done < <(find "$build" \( -name "libhermesvm.$so*" -o -name "libhermes.$so*" \) -not -path '*/CMakeFiles/*')
[ "$found_shared" = 1 ] || { echo "neither libhermesvm.$so nor libhermes.$so found under $build" >&2; exit 1; }
find "$build" -name '*.a' -not -path '*/CMakeFiles/*' -exec cp {} "$out/lib/" \;

# Tools.
for t in hermes hermesc napi-runner; do
  [ -x "$build/bin/$t" ] && cp "$build/bin/$t" "$out/bin/"
done

# Licenses and provenance.
cp "$src/LICENSE" "$out/LICENSE-hermes"
cp "$root/LICENSE" "$out/LICENSE"
cat > "$out/BUILD-INFO.txt" <<INFO
Hermes commit: $commit (https://github.com/facebook/hermes/commit/$commit)
Platform:      $os-$arch
Build type:    ${BUILD_TYPE:-Release}
Built by:      headless-hermes-jsi-node-api scripts/build-hermes.sh and scripts/package.sh
Hermes is licensed under MIT (LICENSE-hermes); the packaging scripts under Apache-2.0 (LICENSE).
INFO

tar -C "$dist" -czf "$dist/$name.tar.gz" "$name"
( cd "$dist" && if command -v sha256sum >/dev/null; then sha256sum "$name.tar.gz"; else shasum -a 256 "$name.tar.gz"; fi > "$name.tar.gz.sha256" )
echo "$dist/$name.tar.gz"
du -sh "$dist/$name.tar.gz"
