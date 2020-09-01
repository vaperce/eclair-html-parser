#ifndef ECLAIR_HTML_COMMON_ERROR_MANAGER
#define ECLAIR_HTML_COMMON_ERROR_MANAGER

#include <string>
#include <vector>

#include "common/position.h"
#include "common/tokens.h"
#include "eclair-html/errors.h"

namespace eclair_html {
namespace html_parser {

class ErrorManager {
public:
  explicit ErrorManager(Errors* errors) : _errors(errors) {}

  void add(ErrorKinds error, Token& token) { add(error, token.position()); }

  void add(ErrorKinds error, const Position& position) {
    if (!_errors) {
      return;
    }

    _errors->push_back(Error(error, position.line, position.lineOffset));
  }

private:
  Errors* _errors;
};

} // namespace html_parser
} // namespace eclair_html

#endif
