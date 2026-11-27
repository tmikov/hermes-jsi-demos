#!/usr/bin/env bash
# Turn the prebuilt archives in dist/ (scripts/package.sh, one per platform) into npm packages in npm/out/:
# @qalqi/headless-hermes-<platform> (runtime library, tools, headers, licenses) and @qalqi/headless-hermes.
# Static libraries stay in the GitHub Release archives, not on npm.
#   VERSION   npm version for every package (default: npm/headless-hermes/package.json's)
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
out="$root/npm/out"
version="${VERSION:-$(node -p "require('$root/npm/headless-hermes/package.json').version")}"
commit="$(tr -d '[:space:]' < "$root/HERMES_COMMIT")"
rm -rf "$out" && mkdir -p "$out"

for archive in "$root"/dist/hermes-*.tar.gz; do
  base="$(basename "$archive" .tar.gz)"        # hermes-<commit12>-<os>-<arch>
  os_arch="${base#hermes-*-}"                   # <os>-<arch>
  os="${os_arch%-*}"; arch="${os_arch##*-}"
  npm_os="$([ "$os" = macos ] && echo darwin || echo "$os")"
  name="headless-hermes-$npm_os-$arch"
  pkg="$out/$name"
  mkdir -p "$pkg"
  tar -xzf "$archive" -C "$out"
  src="$out/$base"
  cp -R "$src/include" "$pkg/"
  mkdir -p "$pkg/lib" "$pkg/bin"
  cp -P "$src"/lib/libhermesvm.* "$pkg/lib/" 2>/dev/null || cp -P "$src"/lib/libhermes.* "$pkg/lib/"
  for t in hermes hermesc; do [ -f "$src/bin/$t" ] && cp "$src/bin/$t" "$pkg/bin/"; done
  cp "$src/LICENSE" "$src/LICENSE-hermes" "$src/BUILD-INFO.txt" "$pkg/"
  cat > "$pkg/package.json" <<JSON
{
  "name": "@qalqi/$name",
  "version": "$version",
  "description": "Prebuilt Hermes ($npm_os-$arch) for @qalqi/headless-hermes: libhermesvm, hermes, hermesc, headers. Hermes commit $commit.",
  "license": "Apache-2.0 AND MIT",
  "repository": { "type": "git", "url": "git+https://github.com/qalqi/headless-hermes-jsi-node-api.git" },
  "os": ["$npm_os"],
  "cpu": ["$arch"],
  "hermes": { "commit": "$commit" },
  "files": ["include/", "lib/", "bin/", "LICENSE", "LICENSE-hermes", "BUILD-INFO.txt"]
}
JSON
  rm -rf "$src"
  echo "$pkg"
done

cp -R "$root/npm/headless-hermes" "$out/headless-hermes"
node -e "
const fs=require('fs'),p='$out/headless-hermes/package.json',j=JSON.parse(fs.readFileSync(p));
j.version='$version'; j.hermes={commit:'$commit'};
for (const k of Object.keys(j.optionalDependencies)) j.optionalDependencies[k]='$version';
fs.writeFileSync(p, JSON.stringify(j,null,2)+'\n');"
echo "$out/headless-hermes"
