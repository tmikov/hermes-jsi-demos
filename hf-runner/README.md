# hf-runner

Hermes and JSI demo of JSI Host Functions loaded dynamically from
shared libraries specified on the command line.

`hf-runner` dynamically loads shared libraries specified on the command line
after the input JS files, and calls them to register any native functions into
the global object. Then it executes the input JS file, which can use the registered
natives.

## Usage

```
hf-runner <js-file> [<shared-lib> ...]
```

## Example

`hf-runner` comes with two example libraries: libhfadd and libhfmyprint. The register
the functions `add` and `myprint` respectively.

The provided file `demo.js` uses both of these functions.

```sh
$ hf-runner demo.js libhfadd.so libhfmyprint.so
```

## Shared Library API

The shared libraries must export a function named `registerNatives` with the following
signature:

```c
void registerNatives(facebook::jsi::Runtime &rt);
```
