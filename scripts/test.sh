#!/usr/bin/env bash
# Run every demo and compare its standard output with expected/<demo>.txt.
#   DEMOS_BUILD_DIR   where the demos were built (default: build)
set -uo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
out="${DEMOS_BUILD_DIR:-$root/build}"
case "$(uname)" in Darwin) so=dylib ;; *) so=so ;; esac
failed=0

check() {
  local name="$1"; shift
  local actual
  actual="$("$@" 2>/dev/null)"
  if diff -u "$root/expected/$name.txt" <(printf '%s\n' "$actual") > "$out/$name.diff"; then
    echo "ok    $name"
  else
    echo "FAIL  $name"; cat "$out/$name.diff"; failed=1
  fi
}

check hello          "$out/hello/hello"
check runner         "$out/runner/runner" "$root/runner/hello.js"
check host-functions "$out/host-functions/host-functions" "$root/host-functions/demo.js"
check hf-runner      "$out/hf-runner/hf-runner" "$root/hf-runner/demo.js" \
                     "$out/hf-runner/libhfadd.$so" "$out/hf-runner/libhfmyprint.$so"
check evloop         "$out/evloop/evloop" "$root/evloop/timer.js"
exit $failed
