#ifndef ECLAIR_HTML_TOKENIZER_STATE_LOGGER
#define ECLAIR_HTML_TOKENIZER_STATE_LOGGER

#include <iostream>

#include "tokenizer/states.h"

namespace xyz {
namespace html_parser {

  class StateLogger {
  public:
    static void log(TokenizerStates state) {
#ifdef XYZ_PARSER_DEBUG_LOGS
      _log(state);
#endif
    }

  private:
    static void _log(TokenizerStates state) {
      std::cerr << "[DEBUG] TOKENIZER MODE: ";
      switch (state) {
        case TokenizerStates::DATA:
          std::cerr << "DATA";
          break;
        case TokenizerStates::RCDATA:
          std::cerr << "RCDATA";
          break;
        case TokenizerStates::RAWTEXT:
          std::cerr << "RAWTEXT";
          break;
        case TokenizerStates::SCRIPT_DATA:
          std::cerr << "SCRIPT_DATA";
          break;
        case TokenizerStates::PLAINTEXT:
          std::cerr << "PLAINTEXT";
          break;
        case TokenizerStates::TAG_OPEN:
          std::cerr << "TAG_OPEN";
          break;
        case TokenizerStates::END_TAG_OPEN:
          std::cerr << "END_TAG_OPEN";
          break;
        case TokenizerStates::TAG_NAME:
          std::cerr << "TAG_NAME";
          break;
        case TokenizerStates::RCDATA_LESS_THAN_SIGN:
          std::cerr << "RCDATA_LESS_THAN_SIGN";
          break;
        case TokenizerStates::RCDATA_END_TAG_OPEN:
          std::cerr << "RCDATA_END_TAG_OPEN";
          break;
        case TokenizerStates::RCDATA_END_TAG_NAME:
          std::cerr << "RCDATA_END_TAG_NAME";
          break;
        case TokenizerStates::RAWTEXT_LESS_THAN_SIGN:
          std::cerr << "RAWTEXT_LESS_THAN_SIGN";
          break;
        case TokenizerStates::RAWTEXT_END_TAG_OPEN:
          std::cerr << "RAWTEXT_END_TAG_OPEN";
          break;
        case TokenizerStates::RAWTEXT_END_TAG_NAME:
          std::cerr << "RAWTEXT_END_TAG_NAME";
          break;
        case TokenizerStates::SCRIPT_DATA_LESS_THAN_SIGN:
          std::cerr << "SCRIPT_DATA_LESS_THAN_SIGN";
          break;
        case TokenizerStates::SCRIPT_DATA_END_TAG_OPEN:
          std::cerr << "SCRIPT_DATA_END_TAG_OPEN";
          break;
        case TokenizerStates::SCRIPT_DATA_END_TAG_NAME:
          std::cerr << "SCRIPT_DATA_END_TAG_NAME";
          break;
        case TokenizerStates::SCRIPT_DATA_ESCAPE_START:
          std::cerr << "SCRIPT_DATA_ESCAPE_START";
          break;
        case TokenizerStates::SCRIPT_DATA_ESCAPE_START_DASH:
          std::cerr << "SCRIPT_DATA_ESCAPE_START_DASH";
          break;
        case TokenizerStates::SCRIPT_DATA_ESCAPED:
          std::cerr << "SCRIPT_DATA_ESCAPED";
          break;
        case TokenizerStates::SCRIPT_DATA_ESCAPED_DASH:
          std::cerr << "SCRIPT_DATA_ESCAPED_DASH";
          break;
        case TokenizerStates::SCRIPT_DATA_ESCAPED_DASH_DASH:
          std::cerr << "SCRIPT_DATA_ESCAPED_DASH_DASH";
          break;
        case TokenizerStates::SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN:
          std::cerr << "SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN";
          break;
        case TokenizerStates::SCRIPT_DATA_ESCAPED_END_TAG_OPEN:
          std::cerr << "SCRIPT_DATA_ESCAPED_END_TAG_OPEN";
          break;
        case TokenizerStates::SCRIPT_DATA_ESCAPED_END_TAG_NAME:
          std::cerr << "SCRIPT_DATA_ESCAPED_END_TAG_NAME";
          break;
        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPE_START:
          std::cerr << "SCRIPT_DATA_DOUBLE_ESCAPE_START";
          break;
        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED:
          std::cerr << "SCRIPT_DATA_DOUBLE_ESCAPED";
          break;
        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_DASH:
          std::cerr << "SCRIPT_DATA_DOUBLE_ESCAPED_DASH";
          break;
        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH:
          std::cerr << "SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH";
          break;
        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN:
          std::cerr << "SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN";
          break;
        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPE_END:
          std::cerr << "SCRIPT_DATA_DOUBLE_ESCAPE_END";
          break;
        case TokenizerStates::BEFORE_ATTRIBUTE_NAME:
          std::cerr << "BEFORE_ATTRIBUTE_NAME";
          break;
        case TokenizerStates::ATTRIBUTE_NAME:
          std::cerr << "ATTRIBUTE_NAME";
          break;
        case TokenizerStates::AFTER_ATTRIBUTE_NAME:
          std::cerr << "AFTER_ATTRIBUTE_NAME";
          break;
        case TokenizerStates::BEFORE_ATTRIBUTE_VALUE:
          std::cerr << "BEFORE_ATTRIBUTE_VALUE";
          break;
        case TokenizerStates::ATTRIBUTE_VALUE_DOUBLE_QUOTED:
          std::cerr << "ATTRIBUTE_VALUE_DOUBLE_QUOTED";
          break;
        case TokenizerStates::ATTRIBUTE_VALUE_SINGLE_QUOTED:
          std::cerr << "ATTRIBUTE_VALUE_SINGLE_QUOTED";
          break;
        case TokenizerStates::ATTRIBUTE_VALUE_UNQUOTED:
          std::cerr << "ATTRIBUTE_VALUE_UNQUOTED";
          break;
        case TokenizerStates::AFTER_ATTRIBUTE_VALUE_QUOTED:
          std::cerr << "AFTER_ATTRIBUTE_VALUE_QUOTED";
          break;
        case TokenizerStates::SELF_CLOSING_START_TAG:
          std::cerr << "SELF_CLOSING_START_TAG";
          break;
        case TokenizerStates::BOGUS_COMMENT:
          std::cerr << "BOGUS_COMMENT";
          break;
        case TokenizerStates::MARKUP_DECLARATION_OPEN:
          std::cerr << "MARKUP_DECLARATION_OPEN";
          break;
        case TokenizerStates::COMMENT_START:
          std::cerr << "COMMENT_START";
          break;
        case TokenizerStates::COMMENT_START_DASH:
          std::cerr << "COMMENT_START_DASH";
          break;
        case TokenizerStates::COMMENT:
          std::cerr << "COMMENT";
          break;
        case TokenizerStates::COMMENT_LESS_THAN_SIGN:
          std::cerr << "COMMENT_LESS_THAN_SIGN";
          break;
        case TokenizerStates::COMMENT_LESS_THAN_SIGN_BANG:
          std::cerr << "COMMENT_LESS_THAN_SIGN_BANG";
          break;
        case TokenizerStates::COMMENT_LESS_THAN_SIGN_BANG_DASH:
          std::cerr << "COMMENT_LESS_THAN_SIGN_BANG_DASH";
          break;
        case TokenizerStates::COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH:
          std::cerr << "COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH";
          break;
        case TokenizerStates::COMMENT_END_DASH:
          std::cerr << "COMMENT_END_DASH";
          break;
        case TokenizerStates::COMMENT_END:
          std::cerr << "COMMENT_END";
          break;
        case TokenizerStates::COMMENT_END_BANG:
          std::cerr << "COMMENT_END_BANG";
          break;
        case TokenizerStates::DOCTYPE:
          std::cerr << "DOCTYPE";
          break;
        case TokenizerStates::BEFORE_DOCTYPE_NAME:
          std::cerr << "BEFORE_DOCTYPE_NAME";
          break;
        case TokenizerStates::DOCTYPE_NAME:
          std::cerr << "DOCTYPE_NAME";
          break;
        case TokenizerStates::AFTER_DOCTYPE_NAME:
          std::cerr << "AFTER_DOCTYPE_NAME";
          break;
        case TokenizerStates::AFTER_DOCTYPE_PUBLIC_KEYWORD:
          std::cerr << "AFTER_DOCTYPE_PUBLIC_KEYWORD";
          break;
        case TokenizerStates::BEFORE_DOCTYPE_PUBLIC_IDENTIFIER:
          std::cerr << "BEFORE_DOCTYPE_PUBLIC_IDENTIFIER";
          break;
        case TokenizerStates::DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED:
          std::cerr << "DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED";
          break;
        case TokenizerStates::DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED:
          std::cerr << "DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED";
          break;
        case TokenizerStates::AFTER_DOCTYPE_PUBLIC_IDENTIFIER:
          std::cerr << "AFTER_DOCTYPE_PUBLIC_IDENTIFIER";
          break;
        case TokenizerStates::BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS:
          std::cerr << "BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS";
          break;
        case TokenizerStates::AFTER_DOCTYPE_SYSTEM_KEYWORD:
          std::cerr << "AFTER_DOCTYPE_SYSTEM_KEYWORD";
          break;
        case TokenizerStates::BEFORE_DOCTYPE_SYSTEM_IDENTIFIER:
          std::cerr << "BEFORE_DOCTYPE_SYSTEM_IDENTIFIER";
          break;
        case TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED:
          std::cerr << "DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED";
          break;
        case TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED:
          std::cerr << "DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED";
          break;
        case TokenizerStates::AFTER_DOCTYPE_SYSTEM_IDENTIFIER:
          std::cerr << "AFTER_DOCTYPE_SYSTEM_IDENTIFIER";
          break;
        case TokenizerStates::BOGUS_DOCTYPE:
          std::cerr << "BOGUS_DOCTYPE";
          break;
        case TokenizerStates::CDATA_SECTION:
          std::cerr << "CDATA_SECTION";
          break;
        case TokenizerStates::CDATA_SECTION_BRACKET:
          std::cerr << "CDATA_SECTION_BRACKET";
          break;
        case TokenizerStates::CDATA_SECTION_END:
          std::cerr << "CDATA_SECTION_END";
          break;
        case TokenizerStates::CHARACTER_REFERENCE:
          std::cerr << "CHARACTER_REFERENCE";
          break;
        case TokenizerStates::NAMED_CHARACTER_REFERENCE:
          std::cerr << "NAMED_CHARACTER_REFERENCE";
          break;
        case TokenizerStates::AMBIGUOUS_AMPERSAND:
          std::cerr << "AMBIGUOUS_AMPERSAND";
          break;
        case TokenizerStates::NUMERIC_CHARACTER_REFERENCE:
          std::cerr << "NUMERIC_CHARACTER_REFERENCE";
          break;
        case TokenizerStates::HEXADECIMAL_CHARACTER_REFERENCE_START:
          std::cerr << "HEXADECIMAL_CHARACTER_REFERENCE_START";
          break;
        case TokenizerStates::DECIMAL_CHARACTER_REFERENCE_START:
          std::cerr << "DECIMAL_CHARACTER_REFERENCE_START";
          break;
        case TokenizerStates::HEXADECIMAL_CHARACTER_REFERENCE:
          std::cerr << "HEXADECIMAL_CHARACTER_REFERENCE";
          break;
        case TokenizerStates::DECIMAL_CHARACTER_REFERENCE:
          std::cerr << "DECIMAL_CHARACTER_REFERENCE";
          break;
        case TokenizerStates::NUMERIC_CHARACTER_REFERENCE_END:
          std::cerr << "NUMERIC_CHARACTER_REFERENCE_END";
          break;
      }
      std::cerr << std::endl;
    }
  };

}
}

#endif
