# headless-hermes-jsi-node-api

Runnable examples of the [Hermes](https://github.com/facebook/hermes) JavaScript engine embedded headless, without
React Native or any other framework, through **JSI** and through Hermes' **Node-API**. Every demo is an
independent CMake project that can be copied and customized; they can also be built and tested together.

Quick start (builds Hermes at the commit pinned in [`HERMES_COMMIT`](HERMES_COMMIT), then every demo):

```sh
scripts/build.sh
scripts/test.sh
```

## Demos

In order of increasing complexity:
* [hello](./hello) - a simple "Hello, world!" demo, running a JS script embedded in C++.
* [runner](./runner) - runs a JS script from a file.
* [host-functions](./host-functions) - demonstrates registering host functions into the global object.
* [hf-runner](./hf-runner) - demonstrate registering host functions from dynamically loaded shared libraries.
* [evloop](./evloop) - demonstrates a simple event loop, which enables setTimeout() and setImmediate(), plus WeakRef.

## Building

### Pre-requisites

* CMake 3.20 or later
* Ninja
* A C++ compiler
* git

Building and running the demos has been tested on Linux and macOS, but not on
Windows. PRs adding support for Windows are welcome.

On macOS, I recommend installing XCode or the XCode command line tools, and using `brew`
to obtain CMake and Ninja.

### Build Hermes

First you need to build hermes. Obtain a clean checkout of Hermes from https://github.com/facebook/hermes.git:
```sh
$ git clone https://github.com/facebook/hermes.git 
```

Create a Hermes build directory somewhere, depending on your preferences, and name it according. 
Then configure and build Hermes:
```sh
mkdir hermes-debug
cd hermes-debug
cmake -G Ninja -DHERMES_BUILD_APPLE_FRAMEWORK=OFF -DCMAKE_BUILD_TYPE=Debug <path-to-hermes-checkout> 
ninja
```

You can change the build type by specifying `-DCMAKE_BUILD_TYPE=Release`.
`-DHERMES_BUILD_APPLE_FRAMEWORK=OFF` is important on MacOS, so make sure you don't forget it.

### Build the demos

All demos require two CMake defines to be set:
* `HERMES_SRC_DIR` - the path to the Hermes checkout.
* `HERMES_BUILD_DIR` - the path to the Hermes build directory from above.

Create a build directory for the demos in a location, depending on your preferences, and name it accordingly.
Then configure and build the demos:
```sh
mkdir demos-debug
cd demos-debug
cmake -G Ninja -DHERMES_SRC_DIR=<path-to-hermes-checkout> \
  -DHERMES_BUILD_DIR=<path-to-hermes-build> \
  <path-to-demos-checkout>
ninja demos
```
## Final Words

This project is built for education purposes, and is not intended to be used in production.
Questions, discussions and PRs are highly encouraged.

## License

Apache License 2.0: see [LICENSE](LICENSE). Files that came from the original demos (credited below) keep their
public-domain notice. Hermes is fetched at build time, not copied into this repository, and is licensed under MIT.

## Credits

- **Original demos**: [tmikov/hermes-jsi-demos](https://github.com/tmikov/hermes-jsi-demos) by Tzvetan Mikov,
  released into the public domain under the [Unlicense](https://github.com/tmikov/hermes-jsi-demos/blob/master/LICENSE).
  The `hello`, `runner`, `host-functions`, `hf-runner` and `evloop` demos and the original build instructions come
  from there.
- **Hermes**: [facebook/hermes](https://github.com/facebook/hermes) (MIT, Meta Platforms, Inc. and affiliates): the
  engine, its JSI API, and its Node-API implementation (`API/napi`, on the `static_h` branch).
- **JSI**: the JavaScript Interface from [React Native](https://github.com/facebook/react-native) (MIT), as vendored in Hermes.
- **Node-API**: the [Node.js](https://github.com/nodejs/node) ABI-stable addon API (MIT), whose headers Hermes vendors.

