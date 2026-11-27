'use strict';
// Paths into the prebuilt Hermes for this platform (installed as an optional dependency).
const path = require('path');

const supported = ['linux-x64', 'linux-arm64', 'darwin-arm64'];

function platformPackageDir() {
  const key = `${process.platform}-${process.arch}`;
  if (!supported.includes(key)) {
    throw new Error(`@qalqi/headless-hermes has no prebuilt Hermes for ${key} (available: ${supported.join(', ')})`);
  }
  try {
    return path.dirname(require.resolve(`@qalqi/headless-hermes-${key}/package.json`));
  } catch {
    throw new Error(`@qalqi/headless-hermes-${key} is not installed: reinstall without --no-optional / --omit=optional`);
  }
}

const dir = () => platformPackageDir();
module.exports = {
  platformPackageDir,
  /** Headers: include/jsi, include/hermes (hermes.h, Public/), include/hermes/napi (node_api.h), include/napi (hermes_napi.h). */
  includeDir: () => path.join(dir(), 'include'),
  /** The JSI runtime library (libhermesvm). */
  libDir: () => path.join(dir(), 'lib'),
  /** A bundled tool: 'hermes' or 'hermesc'. */
  bin: (name) => path.join(dir(), 'bin', name),
  /** The Hermes commit and build details. */
  buildInfo: () => require('fs').readFileSync(path.join(dir(), 'BUILD-INFO.txt'), 'utf8'),
};
