/*
 * This is free and unencumbered software released into the public domain.
 * For more information, please refer to the LICENSE file in the root directory
 * or at <http://unlicense.org/>
 */

#include "registerNatives.h"

#include <iostream>

using namespace facebook;

/// Print all arguments separated by spaces.
static jsi::Value hostMyPrint(
    jsi::Runtime &rt,
    const jsi::Value &,
    const jsi::Value *args,
    size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (i)
      std::cout << ' ';
    std::cout << args[i].toString(rt).utf8(rt);
  }
  std::cout << std::endl;
  return jsi::Value::undefined();
}

extern "C" void registerNatives(jsi::Runtime &rt) {
  rt.global().setProperty(
      rt,
      "myPrint",
      jsi::Function::createFromHostFunction(
          rt, jsi::PropNameID::forAscii(rt, "print"), 0, hostMyPrint));
}
