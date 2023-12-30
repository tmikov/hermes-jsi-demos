/*
 * This is free and unencumbered software released into the public domain.
 * For more information, please refer to the LICENSE file in the root directory
 * or at <http://unlicense.org/>
 */

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include <hermes/hermes.h>

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

int main(int argc, char **argv) {
  // If no argument is provided, print usage and exit.
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <path-to-js-file>" << std::endl;
    return 1;
  }
  const char *jsPath = argv[1];

  // Read the file.
  auto optCode = readFile(jsPath);
  if (!optCode)
    return 1;

  // You can Customize the runtime config here.
  auto runtimeConfig =
      hermes::vm::RuntimeConfig::Builder().withIntl(false).build();

  // Create the Hermes runtime.
  auto runtime = facebook::hermes::makeHermesRuntime(runtimeConfig);

  // Execute some JS.
  int status = 0;
  try {
    runtime->evaluateJavaScript(
        std::make_unique<facebook::jsi::StringBuffer>(std::move(*optCode)),
        jsPath);
  } catch (facebook::jsi::JSError &e) {
    // Handle JS exceptions here.
    std::cerr << "JS Exception: " << e.getStack() << std::endl;
    status = 1;
  } catch (facebook::jsi::JSIException &e) {
    // Handle JSI exceptions here.
    std::cerr << "JSI Exception: " << e.what() << std::endl;
    status = 1;
  }

  return status;
}
