#ifndef ECLAIR_HTML_TOKENIZER_CONFIG
#define ECLAIR_HTML_TOKENIZER_CONFIG

#include "eclair-html/tag_names.h"

namespace eclair_html {
namespace html_parser {

enum class TextReadingModes {
  NORMAL,
  RAW_TEXT,
  RCDATA,
  SCRIPT_DATA,
  PLAINTEXT,
};

struct TokenizerConfig {
  TokenizerConfig() : readingMode(TextReadingModes::NORMAL) {}
  virtual ~TokenizerConfig() {}

  virtual bool isHTMLNamespace() const = 0;

  TextReadingModes readingMode;
  std::u16string expectedEndTag;
};

} // namespace html_parser
} // namespace eclair_html

#endif
