# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This repository contains educational demos showcasing how to use the Hermes JavaScript engine with JSI (JavaScript Interface) without React Native or any other framework. Each demo is an independent CMake project that can be built separately or together from the root.

## Build System

### Prerequisites
- CMake 3.22 or later
- Ninja build system
- C++17 compatible compiler
- A pre-built Hermes installation

### Building Hermes (Required First)
Before building demos, you must have Hermes built separately:
```bash
git clone https://github.com/facebook/hermes.git
mkdir hermes-debug
cd hermes-debug
cmake -G Ninja -DHERMES_BUILD_APPLE_FRAMEWORK=OFF -DCMAKE_BUILD_TYPE=Debug <path-to-hermes-checkout>
ninja
```

**IMPORTANT**: On macOS, `-DHERMES_BUILD_APPLE_FRAMEWORK=OFF` must be specified.

### Building All Demos
From the repository root:
```bash
mkdir demos-debug
cd demos-debug
cmake -G Ninja -DHERMES_SRC_DIR=<path-to-hermes-checkout> \
  -DHERMES_BUILD_DIR=<path-to-hermes-build> \
  <path-to-demos-checkout>
ninja demos
```

The `demos` target builds all demo executables.

### Building Individual Demos
Each demo subdirectory (hello, runner, host-functions, hf-runner, evloop) can be built independently:
```bash
mkdir build
cd build
cmake -G Ninja -DHERMES_SRC_DIR=<path-to-hermes-src> \
  -DHERMES_BUILD_DIR=<path-to-hermes-build> \
  <path-to-specific-demo>
ninja
```

### CMake Variables
All demos require two CMake cache variables:
- `HERMES_SRC_DIR`: Path to Hermes source checkout
- `HERMES_BUILD_DIR`: Path to Hermes build directory

## Demo Architecture (in increasing complexity)

### 1. hello
The simplest demo - executes embedded JavaScript code and demonstrates basic error handling.
- Creates Hermes runtime with custom config
- Evaluates JavaScript from a C++ string literal
- Shows JSError and JSIException handling pattern

### 2. runner
Executes JavaScript from an external file.
- Reads JS file using `readFile()` helper
- Pattern: `./runner <path-to-js-file>`

### 3. host-functions
Demonstrates registering C++ functions callable from JavaScript.
- Uses `registerNatives(jsi::Runtime&)` pattern
- Host functions are registered into the global object
- Shows how to create jsi::Function and bind C++ lambdas

### 4. hf-runner
Advanced host function loading from dynamically loaded shared libraries.
- Loads multiple shared libraries at runtime via `dlopen`/`LoadLibraryA`
- Each library exports `registerNatives(jsi::Runtime&)` function
- Pattern: `./hf-runner <js-file> [<shared-lib>...]`
- Builds example libraries: `hfadd.so`, `hfmyprint.so`
- Uses `extern "C"` linkage for registerNatives in shared libraries

### 5. evloop
Full event loop implementation enabling setTimeout, setImmediate, and WeakRef.
- Includes JavaScript library (jslib.js.inc) compiled into the binary
- Implements macro task queue with C++ event loop
- Uses `withMicrotaskQueue(true)` runtime config
- Calls `runtime->drainMicrotasks()` after each macro task
- Event loop pattern:
  1. Initialize with jslib.js.inc to get `peek()` and `run()` helpers
  2. Execute main JavaScript
  3. Loop: check next task time, sleep if needed, run task, drain microtasks
  4. Exit when no pending tasks
- Can also load shared libraries like hf-runner

## Code Patterns

### Runtime Creation
```cpp
auto runtimeConfig = hermes::vm::RuntimeConfig::Builder()
    .withIntl(false)
    .withMicrotaskQueue(true)  // Only for evloop
    .build();
auto runtime = facebook::hermes::makeHermesRuntime(runtimeConfig);
```

### Exception Handling
Always catch both exception types:
```cpp
try {
    // JSI operations
} catch (jsi::JSError &e) {
    // JavaScript exceptions (includes stack trace)
    std::cerr << "JS Exception: " << e.getStack() << std::endl;
} catch (jsi::JSIException &e) {
    // JSI API exceptions
    std::cerr << "JSI Exception: " << e.what() << std::endl;
}
```

### Host Function Registration
For static linking (host-functions):
```cpp
void registerNatives(facebook::jsi::Runtime &rt);
```

For dynamic loading (hf-runner, evloop):
```cpp
extern "C" void registerNatives(facebook::jsi::Runtime &rt);
```

### CMake Structure for Each Demo
All demos follow the same CMake pattern:
- Validate HERMES_SRC_DIR and HERMES_BUILD_DIR
- Include directories: API, API/jsi, public
- Link directories: API/hermes, jsi
- Link libraries: hermes, jsi
- C++17 standard required

### Platform-Specific Code
- Dynamic library loading uses `#ifndef _WIN32` to handle both POSIX (`dlopen`) and Windows (`LoadLibraryA`) APIs
- Windows support is not actively tested but has been considered in the code

## Working with This Codebase

When modifying or creating new demos:
- Follow the existing demo structure and naming conventions
- Maintain C++17 compatibility
- Use the standard exception handling pattern
- Each demo's CMakeLists.txt should validate both HERMES_SRC_DIR and HERMES_BUILD_DIR
- Register new demos in the root CMakeLists.txt and add to the `demos` target
- Keep demos independent and self-contained
- Use `registerNatives` naming convention for host function initialization

## Testing Changes
After modifying code:
1. Clean build directory: `rm -rf <build-dir>`
2. Reconfigure: `cmake -G Ninja -DHERMES_SRC_DIR=... -DHERMES_BUILD_DIR=... <source-dir>`
3. Build: `ninja demos`
4. Run individual demos with their respective usage patterns
