# hermes-jsi-demos

This repository contains small demos of using the Hermes JavaScript engine with JSI
without React Native or any other framework.

Every demo is constructed as an independent CMake project, which can be copied
and customized, but they can also be built together from the top-level directory.

## Demos
* hello - a simple "Hello, world!" demo, running a JS script embedded in C++.
* runner - runs a JS script from a file.
* host-functions - demonstrates registering host functions into the global object.
* hf-runner - demonstrate registering host functions from dynamically loaded shared libraries.

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

You can use a different build type by using `-DCMAKE_BUILD_TYPE=Release`.
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
