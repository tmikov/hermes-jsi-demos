# headless-hermes-jsi-node-api

[![build](https://github.com/qalqi/headless-hermes-jsi-node-api/actions/workflows/build.yml/badge.svg)](https://github.com/qalqi/headless-hermes-jsi-node-api/actions/workflows/build.yml)

Run the [Hermes](https://github.com/facebook/hermes) JavaScript engine headless: no React Native, no app, no device.
Each example embeds Hermes in a small native program, through **JSI** (the C++ API React Native uses) or through
Hermes' **Node-API** (the ABI-stable C API Node.js addons are written against). Use them to learn how Hermes is
embedded, to test native modules on a desktop or in CI in seconds, or as a starting point for your own host.

Every example is an independent CMake project you can copy out and adapt; they also build and test together.

New to JSI? The [unofficial JSI docs](https://incubator.callstack.com/unofficial-jsi-docs/index.html) (Callstack
Incubator) explain the API these examples use: runtimes, values, host functions and host objects.

## Quick start

```sh
git clone https://github.com/qalqi/headless-hermes-jsi-node-api.git
cd headless-hermes-jsi-node-api
scripts/build.sh   # builds Hermes at the pinned commit (first run only), then every example
scripts/test.sh    # runs every example and checks its output
```

Requirements: CMake 3.22 or later, Ninja, a C++17 compiler and git. Tested on Linux and macOS (Windows is not
tested yet; pull requests welcome). The first build compiles Hermes and takes a while; later builds reuse it.

## Examples

In order of increasing complexity:

| Example | API | What it shows |
|---|---|---|
| [hello](./hello) | JSI | runs a script embedded in C++ |
| [runner](./runner) | JSI | runs a script from a file, reports JS exceptions |
| [host-functions](./host-functions) | JSI | registers C++ host functions in the global object |
| [hf-runner](./hf-runner) | JSI | loads host functions from shared libraries given on the command line |
| [evloop](./evloop) | JSI | a small event loop: `setTimeout`, `setImmediate`, microtasks, `WeakRef` |

Node-API examples (a host for unmodified Node-API addons, C and napi-rs addons, async work and thread-safe functions,
and the same function written with JSI and with Node-API) are in progress.

## Hermes version

Hermes is built from the commit in [`HERMES_COMMIT`](HERMES_COMMIT), on Hermes' `static_h` branch, which carries both
JSI and Node-API (`API/napi`, Node-API version 10). CI rebuilds it weekly. To use a Hermes you already built, point
the scripts at it:

```sh
HERMES_SRC_DIR=/path/to/hermes HERMES_BUILD_DIR=/path/to/hermes-build scripts/build.sh
```

Or build the examples by hand with CMake (`HERMES_SRC_DIR` is the Hermes checkout, `HERMES_BUILD_DIR` its build
directory):

```sh
cmake -S . -B build -G Ninja -DHERMES_SRC_DIR=<hermes-checkout> -DHERMES_BUILD_DIR=<hermes-build>
cmake --build build --target demos
```

On macOS, build Hermes with `-DHERMES_BUILD_APPLE_FRAMEWORK=OFF` (the scripts do).

Hermes' Node-API is new; its known differences from Node.js (external strings are copied, external memory is not
reported to the GC, async work and thread-safe functions need an event loop from the host) are listed in Hermes'
[COMPATIBILITY.md](https://github.com/facebook/hermes/blob/static_h/API/napi/COMPATIBILITY.md).

## Prebuilt Hermes

The [prebuilt workflow](.github/workflows/release.yml) packages the pinned Hermes for Linux x64, Linux arm64 and macOS
arm64: headers (JSI, Hermes, Node-API), the shared runtime (`libhermesvm`), the static libraries a Node-API host links, the
`hermes` and `hermesc` tools, Hermes' license and the exact commit, with a SHA-256 checksum. Releases are published
under [Releases](https://github.com/qalqi/headless-hermes-jsi-node-api/releases).

From the first release on, [`@qalqi/headless-hermes`](https://www.npmjs.com/package/@qalqi/headless-hermes) on npm installs the runtime
library, the `hermes` and `hermesc` tools and the headers for your platform (the static libraries stay in the release
archives):

```sh
npm i -D @qalqi/headless-hermes
npx hermes script.js
```

## Contributing

Issues and pull requests are welcome: new examples, fixes, Windows support, other Hermes versions. Every pull request
runs the full build and every example's output check on Linux and macOS.

## License

Apache License 2.0: see [LICENSE](LICENSE). Files that came from the original demos (credited below) keep their
public-domain notice. Hermes is fetched at build time, not copied into this repository, and is licensed under MIT.

## Credits

- **Original demos**: [tmikov/hermes-jsi-demos](https://github.com/tmikov/hermes-jsi-demos) by Tzvetan Mikov,
  released into the public domain under the [Unlicense](https://github.com/tmikov/hermes-jsi-demos/blob/master/LICENSE).
  The `hello`, `runner`, `host-functions`, `hf-runner` and `evloop` examples and the original build instructions
  come from there.
- **Hermes**: [facebook/hermes](https://github.com/facebook/hermes) (MIT, Meta Platforms, Inc. and affiliates): the
  engine, its JSI API, and its Node-API implementation (`API/napi`, on the `static_h` branch).
- **JSI**: the JavaScript Interface from [React Native](https://github.com/facebook/react-native) (MIT), as vendored in Hermes;
  documented in the [unofficial JSI docs](https://incubator.callstack.com/unofficial-jsi-docs/index.html) by Callstack Incubator.
- **Node-API**: the [Node.js](https://github.com/nodejs/node) ABI-stable addon API (MIT), whose headers Hermes vendors.
