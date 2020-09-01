#ifndef ECLAIR_HTML_TOKENIZER_TOKENIZER
#define ECLAIR_HTML_TOKENIZER_TOKENIZER

#include <string>

#include "common/token_receiver.h"
#include "common/error_manager.h"
#include "tokenizer/config.h"
#include "tokenizer/state_handler.h"

namespace xyz {
namespace html_parser {

  class Tokenizer {
  public:
    Tokenizer(const std::u16string& data, TokenReceiver& tokenReceiver,
        TokenizerConfig& config, ErrorManager& errorManager)
      : _stateHandler(data, tokenReceiver, config, errorManager) {
    }

    void tokenize() {
      while (_stateHandler.tokenize()) {
      }
    }

  private:
    StateHandler _stateHandler;
  };

}
}

#endif
