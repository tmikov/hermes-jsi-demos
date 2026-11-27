# @qalqi/headless-hermes

The [Hermes](https://github.com/facebook/hermes) JavaScript engine for headless use, without React Native: the
`hermes` and `hermesc` tools, the JSI runtime library (`libhermesvm`) and the headers for JSI, Hermes and Node-API,
prebuilt for Linux x64, Linux arm64 and macOS arm64. Built from a pinned commit of Hermes' `static_h` branch.

```sh
npm i -D @qalqi/headless-hermes
npx hermes script.js
```

```js
const hermes = require('@qalqi/headless-hermes');
hermes.includeDir(); // headers for building your own host
hermes.libDir();     // libhermesvm
hermes.buildInfo();  // the exact Hermes commit
```

Only need `hermesc` to compile bytecode for a React Native build? That's Meta's
[`hermes-compiler`](https://www.npmjs.com/package/hermes-compiler). This package is for running and embedding Hermes
outside React Native.

Examples, the build scripts and the static libraries for linking your own Node-API host:
[headless-hermes-jsi-node-api](https://github.com/qalqi/headless-hermes-jsi-node-api). Packaging is Apache-2.0;
Hermes is MIT (LICENSE-hermes in each platform package).
