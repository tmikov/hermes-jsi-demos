/*
* This is free and unencumbered software released into the public domain.
* For more information, please refer to the LICENSE file in the root directory
* or at <http://unlicense.org/>
*/

#include <hermes/hermes.h>
#include <iostream>

/// JS code to be executed.
static const char *code = R"(
    print("Hello, World!");
    throw Error("Surprise!");
)";

int main() {
  // You can Customize the runtime config here.
  auto runtimeConfig =
      hermes::vm::RuntimeConfig::Builder().withIntl(false).build();

  // Create the Hermes runtime.
  auto runtime = facebook::hermes::makeHermesRuntime(runtimeConfig);

  // Execute some JS.
  int status = 0;
  try {
    runtime->evaluateJavaScript(
        std::make_unique<facebook::jsi::StringBuffer>(code), "main.js");
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
