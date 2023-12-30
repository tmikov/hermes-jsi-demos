/*
 * This is free and unencumbered software released into the public domain.
 * For more information, please refer to the LICENSE file in the root directory
 * or at <http://unlicense.org/>
 */

#include "registerNatives.h"

using namespace facebook;

/// A very complicated function that adds all its arguments, which must be
/// numbers, together.
static jsi::Value hostAdd(
    jsi::Runtime &rt,
    const jsi::Value &,
    const jsi::Value *args,
    size_t count) {
  double sum = 0;
  for (size_t i = 0; i < count; ++i) {
    if (!args[i].isNumber())
      throw jsi::JSError(rt, "Argument must be a number");
    sum += args[i].asNumber();
  }
  return sum;
}

extern "C" void registerNatives(jsi::Runtime &rt) {
  rt.global().setProperty(
      rt,
      "add",
      jsi::Function::createFromHostFunction(
          rt, jsi::PropNameID::forAscii(rt, "add"), 2, hostAdd));
}
