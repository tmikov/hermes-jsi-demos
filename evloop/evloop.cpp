/*
 * This is free and unencumbered software released into the public domain.
 * For more information, please refer to the LICENSE file in the root directory
 * or at <http://unlicense.org/>
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <thread>
#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#include <hermes/hermes.h>

using namespace ::facebook;

/// The JS library that implements the event loop.
static const char *s_jslib =
#include "jslib.js.inc"
    ;

/// Read the contents of a file into a string.
static std::optional<std::string> readFile(const char *path) {
  std::ifstream fileStream(path);
  std::stringstream stringStream;

  if (fileStream) {
    stringStream << fileStream.rdbuf();
    fileStream.close();
  } else {
    // Handle error - file opening failed
    std::cerr << path << ": error opening file" << std::endl;
    return std::nullopt;
  }

  return stringStream.str();
}

/// The signature of the function that initializes the library.
typedef void (*RegisterNativesFN)(jsi::Runtime &rt);

#ifndef _WIN32
/// Load the library and return the "registerNatives()" function.
static RegisterNativesFN loadRegisterNatives(const char *libraryPath) {
  // Open the library.
  void *handle = dlopen(libraryPath, RTLD_LAZY);
  if (!handle) {
    std::cerr << "*** Cannot open library: " << dlerror() << '\n';
    return nullptr;
  }

  // Clear any existing error.
  dlerror();
  // Load the symbol (function).
  auto func = (RegisterNativesFN)dlsym(handle, "registerNatives");
  if (const char *dlsym_error = dlerror()) {
    std::cerr << "Cannot load symbol 'registerNatives': " << dlsym_error
              << '\n';
    dlclose(handle);
    return nullptr;
  }

  return func;
}
#else
/// Load the library and return the "registerNatives()" function.
static RegisterNativesFN loadRegisterNatives(const char *libraryPath) {
  // Load the library
  HMODULE hModule = LoadLibraryA(libraryPath);
  if (!hModule) {
    std::cerr << "Cannot open library: " << GetLastError() << '\n';
    return nullptr;
  }

  // Get the function address
  auto func = (RegisterNativesFN)GetProcAddress(hModule, "registerNatives");
  if (!func) {
    std::cerr << "Cannot load symbol 'registerNatives': " << GetLastError()
              << '\n';
    FreeLibrary(hModule);
    return nullptr;
  }

  return func;
}
#endif

/// Load all the libraries and call their "registerNatives()" function.
/// \return true if all libraries were loaded successfully.
static bool loadNativeLibraries(jsi::Runtime &rt, int argc, char **argv) {
  try {
    for (int i = 2; i < argc; i++) {
      auto func = loadRegisterNatives(argv[i]);
      if (!func)
        return false;
      func(rt);
    }
  } catch (jsi::JSIException &e) {
    // Handle JSI exceptions here.
    std::cerr << "JSI Exception: " << e.what() << std::endl;
    return false;
  }
  return true;
}

int main(int argc, char **argv) {
  // If no argument is provided, print usage and exit.
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <path-to-js-file> [<shared-lib>...]"
              << std::endl;
    return 1;
  }
  const char *jsPath = argv[1];

  // Read the file.
  auto optCode = readFile(jsPath);
  if (!optCode)
    return 1;

  // You can Customize the runtime config here.
  auto runtimeConfig = ::hermes::vm::RuntimeConfig::Builder()
                           .withIntl(false)
                           .withMicrotaskQueue(true)
                           .build();

  // Create the Hermes runtime.
  auto runtime = facebook::hermes::makeHermesRuntime(runtimeConfig);

  try {
    // Register event loop functions and obtain the runMicroTask() helper
    // function.
    jsi::Object helpers =
        runtime
            ->evaluateJavaScript(
                std::make_unique<jsi::StringBuffer>(s_jslib), "jslib.js.inc")
            .asObject(*runtime);
    jsi::Function peekMacroTask =
        helpers.getPropertyAsFunction(*runtime, "peek");
    jsi::Function runMacroTask = helpers.getPropertyAsFunction(*runtime, "run");

    // There are no pending tasks, but we want to initialize the event loop
    // current time.
    {
      double curTimeMs =
          (double)std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::steady_clock::now().time_since_epoch())
              .count();
      runMacroTask.call(*runtime, curTimeMs);
    }

    // Register host functions.
    if (!loadNativeLibraries(*runtime, argc, argv))
      return 1;

    runtime->evaluateJavaScript(
        std::make_unique<jsi::StringBuffer>(std::move(*optCode)), jsPath);
    runtime->drainMicrotasks();

    double nextTimeMs;

    // This is the event loop. Loop while there are pending tasks.
    while ((nextTimeMs = peekMacroTask.call(*runtime).getNumber()) >= 0) {
      auto now = std::chrono::steady_clock::now();
      double curTimeMs =
          (double)std::chrono::duration_cast<std::chrono::milliseconds>(
              now.time_since_epoch())
              .count();

      // If we have to, sleep until the next task is ready.
      if (nextTimeMs > curTimeMs) {
        std::this_thread::sleep_until(
            now +
            std::chrono::milliseconds((int_least64_t)(nextTimeMs - curTimeMs)));

        // Update the current time, since we slept.
        curTimeMs =
            (double)std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch())
                .count();
      }

      // Run the next task.
      runMacroTask.call(*runtime, curTimeMs);
      runtime->drainMicrotasks();
    }
  } catch (jsi::JSError &e) {
    // Handle JS exceptions here.
    std::cerr << "JS Exception: " << e.getStack() << std::endl;
    return 1;
  } catch (jsi::JSIException &e) {
    // Handle JSI exceptions here.
    std::cerr << "JSI Exception: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
