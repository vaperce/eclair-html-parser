# eclair-html-parser

A simple and efficient C++ HTML parser following [Whatwg HTML specification](https://html.spec.whatwg.org/multipage/).

Features:

* full HTML/XML parsing with browser like DOM tree corrections
* browser like encoding detection
* HTML content cleanup
* simple DOM access API
* pretty printing

Made with ❤️ in Paris 🇫🇷.

## Usage example

```cpp
Document document;

// Parse document
document.parse(
  "<html><head><title>test</title></head><body><h1>Test</h1></body></html>",
  71,
  "utf8",
  nullptr
);

// Clean document
document.clean(CleanFlags::SPACE);

// Access root node
Node root = document.root();
```

## Build

Requires CMake.

Builds static libraries only.

```bash
> mkdir build
> cd build
> cmake -DCMAKE_INSTALL_PREFIX=~/my-install-dir ..
> make -j
> make install
```

## Dependencies

* compact_enc_det: head
* icu4c: 65-1

Dependencies are automatically downloaded and built during the build.
