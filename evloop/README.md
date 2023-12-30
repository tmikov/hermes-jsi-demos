# evloop

This example builds on the [hf-runner](../hf-runner) example. It adds a simple event
loop, which allows the JavaScript code to register callbacks using setTimeout() and
setImmediate(). It also enables micro-task support, which in turn enables WeakRef
support.

## Usage

```
evloop <js-file> [<shared-lib> ...]
```
