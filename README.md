# libcompact

Compact encoding schemes for C with the same ABI as <https://github.com/holepunchto/compact-encoding>.

## API

See [`include/compact.h`](include/compact.h) for the public API.

## Conformance

`test/conformance.c` runs this library against the [compact-encoding corpus](https://github.com/holepunchto/compact-encoding-test) and reports what matched, what diverged and which codecs it does not provide. It reads the fixtures from `node_modules`, so a case added upstream arrives with `npm install`.

```sh
ctest --test-dir build -R conformance -V    # the summary
build/test/conformance --json               # the report
```

## License

Apache-2.0
