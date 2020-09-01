# eclair-html-parser

## About

A simple and efficient C++ HTML parser following [Whatwg HTML specification](https://html.spec.whatwg.org/multipage/).
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
