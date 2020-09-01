#ifndef ECLAIR_HTML_COMMON_TOKEN_RECEIVER
#define ECLAIR_HTML_COMMON_TOKEN_RECEIVER

#include "common/tokens.h"

namespace eclair_html {
namespace html_parser {

struct TokenReceiver {
  virtual ~TokenReceiver() {}
  virtual void process(Token& token) = 0;
};

} // namespace html_parser
} // namespace eclair_html

#endif
