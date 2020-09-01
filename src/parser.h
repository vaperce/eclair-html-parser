#ifndef ECLAIR_HTML_PARSER
#define ECLAIR_HTML_PARSER

#include <string>
#include <vector>

#include "common/error_manager.h"
#include "eclair-html/errors.h"
#include "eclair-html/node.h"
#include "parse_buffer/parse_buffer.h"
#include "tokenizer/tokenizer.h"
#include "tree_builder/tree_builder.h"

namespace eclair_html {
namespace html_parser {

class Parser {
public:
  explicit Parser(Node& document, const char* data, unsigned length,
                  Errors* errors)
      : _document(document), _data(data), _length(length),
        _errorManager(errors) {}

  bool parse(const char* charset) {
    ParseBuffer buffer(_data, _length);
    if (!buffer.make(charset)) {
      return false;
    }

    _parse(buffer.content);
    return true;
  }

  bool parse(const char* url, const char* httpCharset) {
    ParseBuffer buffer(_data, _length);
    if (!buffer.make(url, httpCharset)) {
      return false;
    }

    _parse(buffer.content);
    return true;
  }

private:
  void _parse(const std::u16string& buffer) {
    TreeBuilder treeBuilder(_document, _errorManager);
    Tokenizer tokenizer(buffer, treeBuilder, treeBuilder.tokenizerConfig(),
                        _errorManager);
    tokenizer.tokenize();
  }

  Node& _document;
  const char* _data;
  unsigned _length;
  ErrorManager _errorManager;
};

} // namespace html_parser
} // namespace eclair_html

#endif
