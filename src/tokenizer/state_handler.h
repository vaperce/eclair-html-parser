#ifndef ECLAIR_HTML_TOKENIZER_STATE_HANDLER
#define ECLAIR_HTML_TOKENIZER_STATE_HANDLER

#include "common/error_manager.h"
#include "tokenizer/base_state_handler.h"
#include "tokenizer/entities.h"

namespace eclair_html {
namespace html_parser {

  class StateHandler : public BaseStateHandler {
  public:
    explicit StateHandler(const std::u16string& data,
        TokenReceiver& tokenReceiver, TokenizerConfig& config,
        ErrorManager& errorManager)
      : BaseStateHandler(tokenReceiver, data, config, errorManager) {
    }

    bool tokenize() {
      _checkTextInsertionMode();
      _consume();
      _tokenize();
      return !_eof();
    }

  private:
    void _tokenize() {
      switch (_state()) {
        case TokenizerStates::DATA:
          if (_eof()) {
            _emitEof();
          } else if (_cc() == '&') {
            _saveReturnState();
            _switch(TokenizerStates::CHARACTER_REFERENCE);
          } else if (_cc() == '<') {
            _switch(TokenizerStates::TAG_OPEN);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _emitChar();
          } else {
            _emitChar();
          }
          break;

        case TokenizerStates::RCDATA:
          if (_eof()) {
            _emitEof();
          } else if (_cc() == '&') {
            _saveReturnState();
            _switch(TokenizerStates::CHARACTER_REFERENCE);
          } else if (_cc() == '<') {
            _switch(TokenizerStates::RCDATA_LESS_THAN_SIGN);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _emitReplacementChar();
          } else {
            _emitChar();
          }
          break;

        case TokenizerStates::RAWTEXT:
          if (_eof()) {
            _emitEof();
          } else if (_cc() == '<') {
            _switch(TokenizerStates::RAWTEXT_LESS_THAN_SIGN);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _emitReplacementChar();
          } else {
            _emitChar();
          }
          break;

        case TokenizerStates::SCRIPT_DATA:
          if (_eof()) {
            _emitEof();
          } else if (_cc() == '<') {
            _switch(TokenizerStates::SCRIPT_DATA_LESS_THAN_SIGN);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _emitReplacementChar();
          } else {
            _emitChar();
          }
          break;

        case TokenizerStates::PLAINTEXT:
          if (_eof()) {
            _emitEof();
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _emitReplacementChar();
          } else {
            _emitChar();
          }
          break;

        case TokenizerStates::TAG_OPEN:
          if (_eof()) {
            _error(ErrorKinds::EOF_BEFORE_TAG_NAME);
            _emitChar('<');
            _emitEof();
          } else if (_cc() == '!') {
            _switch(TokenizerStates::MARKUP_DECLARATION_OPEN);
          } else if (_cc() == '/') {
            _switch(TokenizerStates::END_TAG_OPEN);
          } else if (_isAsciiAlpha()) {
            _newStartTag();
            _reconsume(TokenizerStates::TAG_NAME);
          } else if (_cc() == '?') {
            _error(ErrorKinds::UNEXPECTED_QUESTION_MARK_INSTEAD_OF_TAG_NAME);
            _newComment();
            _reconsume(TokenizerStates::BOGUS_COMMENT);
          } else {
            _error(ErrorKinds::INVALID_FIRST_CHARACTER_OF_TAG_NAME);
            _emitChar('<');
            _reconsume(TokenizerStates::DATA);
          }
          break;

        case TokenizerStates::END_TAG_OPEN:
          if (_eof()) {
            _error(ErrorKinds::EOF_BEFORE_TAG_NAME);
            _emitChar('<');
            _emitChar('/');
            _emitEof();
          } else if (_isAsciiAlpha()) {
            _newEndTag();
            _reconsume(TokenizerStates::TAG_NAME);
          } else if (_cc() == '>') {
            _error(ErrorKinds::MISSING_END_TAG_NAME);
            _switch(TokenizerStates::DATA);
          } else {
            _error(ErrorKinds::INVALID_FIRST_CHARACTER_OF_TAG_NAME);
            _newComment();
            _reconsume(TokenizerStates::BOGUS_COMMENT);
          }
          break;

        case TokenizerStates::TAG_NAME:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_TAG);
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            _switch(TokenizerStates::BEFORE_ATTRIBUTE_NAME);
          } else if (_cc() == '/') {
            _switch(TokenizerStates::SELF_CLOSING_START_TAG);
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitTag();
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _emitReplacementChar();
          } else {
            _appendTagName();
          }
          break;

        case TokenizerStates::RCDATA_LESS_THAN_SIGN:
          if (!_eof() && _cc() == '/') {
            _resetTempBuffer();
            _switch(TokenizerStates::RCDATA_END_TAG_OPEN);
          } else {
            _emitChar('<');
            _reconsume(TokenizerStates::RCDATA);
          }
          break;

        case TokenizerStates::RCDATA_END_TAG_OPEN:
          if (!_eof() && _isAsciiAlpha()) {
            _newEndTag();
            _reconsume(TokenizerStates::RCDATA_END_TAG_NAME);
          } else {
            _emitChar('<');
            _emitChar('/');
            _reconsume(TokenizerStates::RCDATA);
          }
          break;

        case TokenizerStates::RCDATA_END_TAG_NAME:
          if (!_eof()) {
            if ((_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
                  _cc() == ' ') &&
                _isAppropriateEndTag()) {
              _switch(TokenizerStates::BEFORE_ATTRIBUTE_NAME);
              break;
            }
            if (_cc() == '/' && _isAppropriateEndTag()) {
              _switch(TokenizerStates::SELF_CLOSING_START_TAG);
              break;
            }
            if (_cc() == '>' && _isAppropriateEndTag()) {
              _switch(TokenizerStates::DATA);
              _emitTag();
              break;
            }
            if (_isAsciiAlpha()) {
              _appendTagName();
              _appendTempBuffer();
              break;
            }
          }
          _emitChar('<');
          _emitChar('/');
          _emitTempBufferChars();
          _reconsume(TokenizerStates::RCDATA);
          break;

        case TokenizerStates::RAWTEXT_LESS_THAN_SIGN:
          if (!_eof() && _cc() == '/') {
            _resetTempBuffer();
            _switch(TokenizerStates::RAWTEXT_END_TAG_OPEN);
          } else {
            _emitChar('<');
            _reconsume(TokenizerStates::RAWTEXT);
          }
          break;

        case TokenizerStates::RAWTEXT_END_TAG_OPEN:
          if (!_eof() && _isAsciiAlpha()) {
            _newEndTag();
            _reconsume(TokenizerStates::RAWTEXT_END_TAG_NAME);
          } else {
            _emitChar('<');
            _emitChar('/');
            _reconsume(TokenizerStates::RAWTEXT);
          }
          break;

        case TokenizerStates::RAWTEXT_END_TAG_NAME:
          if (!_eof()) {
            if ((_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
                  _cc() == ' ') &&
                _isAppropriateEndTag()) {
              _switch(TokenizerStates::BEFORE_ATTRIBUTE_NAME);
              break;
            }
            if (_cc() == '/' && _isAppropriateEndTag()) {
              _switch(TokenizerStates::SELF_CLOSING_START_TAG);
              break;
            }
            if (_cc() == '>' && _isAppropriateEndTag()) {
              _switch(TokenizerStates::DATA);
              _emitTag();
              break;
            }
            if (_isAsciiAlpha()) {
              _appendTagName();
              _appendTempBuffer();
              break;
            }
          }
          _emitChar('<');
          _emitChar('/');
          _emitTempBufferChars();
          _reconsume(TokenizerStates::RAWTEXT);
          break;

        case TokenizerStates::SCRIPT_DATA_LESS_THAN_SIGN:
          if (!_eof()) {
            if (_cc() == '/') {
              _resetTempBuffer();
              _switch(TokenizerStates::SCRIPT_DATA_END_TAG_OPEN);
              break;
            }
            if (_cc() == '!') {
              _switch(TokenizerStates::SCRIPT_DATA_ESCAPE_START);
              _emitChar('<');
              _emitChar('!');
              break;
            }
          }
          _emitChar('<');
          _reconsume(TokenizerStates::SCRIPT_DATA);
          break;

        case TokenizerStates::SCRIPT_DATA_END_TAG_OPEN:
          if (!_eof() && _isAsciiAlpha()) {
            _newEndTag();
            _reconsume(TokenizerStates::SCRIPT_DATA_END_TAG_NAME);
          } else {
            _emitChar('<');
            _emitChar('/');
            _reconsume(TokenizerStates::SCRIPT_DATA);
          }
          break;

        case TokenizerStates::SCRIPT_DATA_END_TAG_NAME:
          if (!_eof()) {
            if ((_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
                  _cc() == ' ') &&
                _isAppropriateEndTag()) {
              _switch(TokenizerStates::BEFORE_ATTRIBUTE_NAME);
              break;
            }
            if (_cc() == '/' && _isAppropriateEndTag()) {
              _switch(TokenizerStates::SELF_CLOSING_START_TAG);
              break;
            }
            if (_cc() == '>' && _isAppropriateEndTag()) {
              _switch(TokenizerStates::DATA);
              _emitTag();
              break;
            }
            if (_isAsciiAlpha()) {
              _appendTagName();
              _appendTempBuffer();
              break;
            }
          }
          _emitChar('<');
          _emitChar('/');
          _emitTempBufferChars();
          _reconsume(TokenizerStates::SCRIPT_DATA);
          break;

        case TokenizerStates::SCRIPT_DATA_ESCAPE_START:
          if (!_eof() && _cc() == '-') {
            _reconsume(TokenizerStates::SCRIPT_DATA_ESCAPE_START_DASH);
            _emitChar('-');
          } else {
            _reconsume(TokenizerStates::SCRIPT_DATA);
          }
          break;

        case TokenizerStates::SCRIPT_DATA_ESCAPE_START_DASH:
          if (!_eof() && _cc() == '-') {
            _reconsume(TokenizerStates::SCRIPT_DATA_ESCAPED_DASH_DASH);
            _emitChar('-');
          } else {
            _reconsume(TokenizerStates::SCRIPT_DATA);
          }
          break;

        case TokenizerStates::SCRIPT_DATA_ESCAPED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT);
            _emitEof();
          } else if (_cc() == '-') {
            _switch(TokenizerStates::SCRIPT_DATA_ESCAPED_DASH);
            _emitChar('-');
          } else if (_cc() == '<') {
            _switch(TokenizerStates::SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _emitReplacementChar();
          } else {
            _emitChar();
          }
          break;

        case TokenizerStates::SCRIPT_DATA_ESCAPED_DASH:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT);
            _emitEof();
          } else if (_cc() == '-') {
            _switch(TokenizerStates::SCRIPT_DATA_ESCAPED_DASH_DASH);
            _emitChar('-');
          } else if (_cc() == '<') {
            _switch(TokenizerStates::SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _switch(TokenizerStates::SCRIPT_DATA_ESCAPED);
            _emitReplacementChar();
          } else {
            _switch(TokenizerStates::SCRIPT_DATA_ESCAPED);
            _emitChar();
          }
          break;

        case TokenizerStates::SCRIPT_DATA_ESCAPED_DASH_DASH:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT);
            _emitEof();
          } else if (_cc() == '-') {
            _emitChar('-');
          } else if (_cc() == '<') {
            _switch(TokenizerStates::SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN);
          } else if (_cc() == '>') {
            _switch(TokenizerStates::SCRIPT_DATA);
            _emitChar('>');
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _switch(TokenizerStates::SCRIPT_DATA_ESCAPED);
            _emitReplacementChar();
          } else {
            _switch(TokenizerStates::SCRIPT_DATA_ESCAPED);
            _emitChar();
          }
          break;

        case TokenizerStates::SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN:
          if (!_eof()) {
            if (_cc() == '/') {
              _resetTempBuffer();
              _switch(TokenizerStates::SCRIPT_DATA_ESCAPED_END_TAG_OPEN);
              break;
            }
            if (_isAsciiAlpha()) {
              _resetTempBuffer();
              _emitChar('<');
              _reconsume(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPE_START);
              break;
            }
          }
          _emitChar('<');
          _reconsume(TokenizerStates::SCRIPT_DATA_ESCAPED);
          break;

        case TokenizerStates::SCRIPT_DATA_ESCAPED_END_TAG_OPEN:
          if (!_eof() && _isAsciiAlpha()) {
            _newEndTag();
            _reconsume(TokenizerStates::SCRIPT_DATA_ESCAPED_END_TAG_NAME);
          } else {
            _emitChar('<');
            _emitChar('/');
            _reconsume(TokenizerStates::SCRIPT_DATA_ESCAPED);
          }
          break;

        case TokenizerStates::SCRIPT_DATA_ESCAPED_END_TAG_NAME:
          if (!_eof()) {
            if ((_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
                  _cc() == ' ') &&
                _isAppropriateEndTag()) {
              _switch(TokenizerStates::BEFORE_ATTRIBUTE_NAME);
              break;
            }
            if (_cc() == '/' && _isAppropriateEndTag()) {
              _switch(TokenizerStates::SELF_CLOSING_START_TAG);
              break;
            }
            if (_cc() == '>' && _isAppropriateEndTag()) {
              _switch(TokenizerStates::DATA);
              _emitTag();
              break;
            }
            if (_isAsciiAlpha()) {
              _appendTagName();
              _appendTempBuffer();
              break;
            }
          }
          _emitChar('<');
          _emitChar('/');
          _emitTempBufferChars();
          _reconsume(TokenizerStates::SCRIPT_DATA_ESCAPED);
          break;

        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPE_START:
          if (!_eof()) {
            if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
                _cc() == ' ' ||
                _cc() == '/' || _cc() == '>') {
              if (_isTempBuffer(u"script")) {
                _switch(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED);
              } else {
                _switch(TokenizerStates::SCRIPT_DATA_ESCAPED);
              }
              _emitChar();
              break;
            }
            if (_isAsciiAlpha()) {
              _appendTempBufferLower();
              _emitChar();
              break;
            }
          }
          _reconsume(TokenizerStates::SCRIPT_DATA_ESCAPED);
          break;

        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT);
            _emitEof();
          } else if (_cc() == '-') {
            _switch(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_DASH);
            _emitChar('-');
          } else if (_cc() == '<') {
            _switch(
                TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN);
            _emitChar('<');
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _emitReplacementChar();
          } else {
            _emitChar();
          }
          break;

        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_DASH:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT);
            _emitEof();
          } else if (_cc() == '-') {
            _switch(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH);
            _emitChar('-');
          } else if (_cc() == '<') {
            _switch(
                TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN);
            _emitChar('<');
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _switch(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED);
            _emitReplacementChar();
          } else {
            _switch(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED);
            _emitChar();
          }
          break;

        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_SCRIPT_HTML_COMMENT_LIKE_TEXT);
            _emitEof();
          } else if (_cc() == '-') {
            _emitChar('-');
          } else if (_cc() == '<') {
            _switch(
                TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN);
            _emitChar('<');
          } else if (_cc() == '>') {
            _switch(TokenizerStates::SCRIPT_DATA);
            _emitChar('>');
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _switch(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED);
            _emitReplacementChar();
          } else {
            _switch(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED);
            _emitChar();
          }
          break;

        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN:
          if (!_eof() && _cc() == '/') {
            _resetTempBuffer();
            _switch(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPE_END);
            _emitChar('>');
          } else {
            _reconsume(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED);
          }
          break;

        case TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPE_END:
          if (!_eof()) {
            if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
                _cc() == ' ' ||
                _cc() == '/' || _cc() == '>') {
              if (_isTempBuffer(u"script")) {
                _switch(TokenizerStates::SCRIPT_DATA_ESCAPED);
              } else {
                _switch(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED);
              }
              _emitChar();
              break;
            }
            if (_isAsciiAlpha()) {
              _appendTempBufferLower();
              _emitChar();
              break;
            }
          }
          _reconsume(TokenizerStates::SCRIPT_DATA_DOUBLE_ESCAPED);
          break;

        case TokenizerStates::BEFORE_ATTRIBUTE_NAME:
          if (_eof() || _cc() == '/' || _cc() == '>') {
            _reconsume(TokenizerStates::AFTER_ATTRIBUTE_NAME);
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            // ignore
          } else if (_cc() == '=') {
            _error(ErrorKinds::UNEXPECTED_EQUALS_SIGN_BEFORE_ATTRIBUTE_NAME);
            _newAttribute();
            _appendAttributeName();
            _switch(TokenizerStates::ATTRIBUTE_NAME);
          } else {
            _newAttribute();
            _reconsume(TokenizerStates::ATTRIBUTE_NAME);
          }
          break;

        case TokenizerStates::ATTRIBUTE_NAME:
          if (_eof() || _cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ' || _cc() == '/' || _cc() == '>') {
            _reconsume(TokenizerStates::AFTER_ATTRIBUTE_NAME);
          } else if (_cc() == '=') {
            _switch(TokenizerStates::BEFORE_ATTRIBUTE_VALUE);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementAttributeName();
          } else if (_cc() == '"' || _cc() == '\'' || _cc() == '<') {
            _error(ErrorKinds::UNEXPECTED_CHARACTER_IN_ATTRIBUTE_NAME);
            _appendAttributeName();
          } else {
            _appendAttributeName();
          }
          break;

        case TokenizerStates::AFTER_ATTRIBUTE_NAME:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_TAG);
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            // ignore
          } else if (_cc() == '/') {
            _switch(TokenizerStates::SELF_CLOSING_START_TAG);
          } else if (_cc() == '=') {
            _switch(TokenizerStates::BEFORE_ATTRIBUTE_VALUE);
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitTag();
          } else {
            _newAttribute();
            _reconsume(TokenizerStates::ATTRIBUTE_NAME);
          }
          break;

        case TokenizerStates::BEFORE_ATTRIBUTE_VALUE:
          if (!_eof()) {
            if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
                _cc() == ' ') {
              break;
            }
            if (_cc() == '"') {
              _switch(TokenizerStates::ATTRIBUTE_VALUE_DOUBLE_QUOTED);
              break;
            }
            if (_cc() == '\'') {
              _switch(TokenizerStates::ATTRIBUTE_VALUE_SINGLE_QUOTED);
              break;
            }
            if (_cc() == '>') {
              _error(ErrorKinds::MISSING_ATTRIBUTE_VALUE);
              _switch(TokenizerStates::DATA);
              _emitTag();
              break;
            }
          }
          _reconsume(TokenizerStates::ATTRIBUTE_VALUE_UNQUOTED);
          break;

        case TokenizerStates::ATTRIBUTE_VALUE_DOUBLE_QUOTED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_TAG);
            _emitEof();
          } else if (_cc() == '"') {
            _switch(TokenizerStates::AFTER_ATTRIBUTE_VALUE_QUOTED);
          } else if (_cc() == '&') {
            _saveReturnState();
            _switch(TokenizerStates::CHARACTER_REFERENCE);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementAttributeValue();
          } else {
            _appendAttributeValue();
          }
          break;

        case TokenizerStates::ATTRIBUTE_VALUE_SINGLE_QUOTED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_TAG);
            _emitEof();
          } else if (_cc() == '\'') {
            _switch(TokenizerStates::AFTER_ATTRIBUTE_VALUE_QUOTED);
          } else if (_cc() == '&') {
            _saveReturnState();
            _switch(TokenizerStates::CHARACTER_REFERENCE);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementAttributeValue();
          } else {
            _appendAttributeValue();
          }
          break;

        case TokenizerStates::ATTRIBUTE_VALUE_UNQUOTED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_TAG);
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            _switch(TokenizerStates::BEFORE_ATTRIBUTE_NAME);
          } else if (_cc() == '&') {
            _saveReturnState();
            _switch(TokenizerStates::CHARACTER_REFERENCE);
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitTag();
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementAttributeValue();
          } else if (_cc() == '"' || _cc() == '\'' || _cc() == '<' ||
              _cc() == '=' || _cc() == '`') {
            _error(
                ErrorKinds::UNEXPECTED_CHARACTER_IN_UNQUOTED_ATTRIBUTE_VALUE);
            _appendAttributeValue();
          } else {
            _appendAttributeValue();
          }
          break;

        case TokenizerStates::AFTER_ATTRIBUTE_VALUE_QUOTED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_TAG);
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            _switch(TokenizerStates::BEFORE_ATTRIBUTE_NAME);
          } else if (_cc() == '/') {
            _switch(TokenizerStates::SELF_CLOSING_START_TAG);
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitTag();
          } else {
            _error(ErrorKinds::MISSING_WHITESPACE_BETWEEN_ATTRIBUTES);
            _reconsume(TokenizerStates::BEFORE_ATTRIBUTE_NAME);
          }
          break;

        case TokenizerStates::SELF_CLOSING_START_TAG:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_TAG);
            _emitEof();
          } else if (_cc() == '>') {
            _setSelfClosing();
            _switch(TokenizerStates::DATA);
            _emitTag();
          } else {
            _error(ErrorKinds::UNEXPECTED_SOLIDUS_IN_TAG);
            _reconsume(TokenizerStates::BEFORE_ATTRIBUTE_NAME);
          }
          break;

        case TokenizerStates::BOGUS_COMMENT:
          if (_eof()) {
            _emitComment();
            _emitEof();
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitComment();
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementComment();
          } else {
            _appendComment();
          }
          break;

        case TokenizerStates::MARKUP_DECLARATION_OPEN:
          if (_areNextCharacters(u"--", true)) {
            _consume(2);
            _newComment();
            _switch(TokenizerStates::COMMENT_START);
          } else if (_areNextCharacters(u"DOCTYPE", false)) {
            _consume(7);
            _switch(TokenizerStates::DOCTYPE);
          } else if (_areNextCharacters(u"[CDATA[", true)) {
            _consume(7);
            if (_isHTMLNamespace()) {
              _error(ErrorKinds::CDATA_IN_HTML_CONTENT);
              _newComment();
              _appendComment(u"[CDATA[");
              _switch(TokenizerStates::BOGUS_COMMENT);
            } else {
              _switch(TokenizerStates::CDATA_SECTION);
            }
          } else {
            _error(ErrorKinds::INCORRECTLY_OPENED_COMMENT);
            _newComment();
            // As we consume by default, reconsume is needed
            _reconsume(TokenizerStates::BOGUS_COMMENT);
          }
          break;

        case TokenizerStates::COMMENT_START:
          if (!_eof()) {
            if (_cc() == '-') {
              _switch(TokenizerStates::COMMENT_START_DASH);
              break;
            }
            if (_cc() == '>') {
              _error(ErrorKinds::ABRUPT_CLOSING_OF_EMPTY_COMMENT);
              _switch(TokenizerStates::DATA);
              _emitComment();
              break;
            }
          }
          _reconsume(TokenizerStates::COMMENT);
          break;

        case TokenizerStates::COMMENT_START_DASH:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_COMMENT);
            _emitComment();
            _emitEof();
          } else if (_cc() == '-') {
            _switch(TokenizerStates::COMMENT_END);
          } else if (_cc() == '>') {
            _error(ErrorKinds::ABRUPT_CLOSING_OF_EMPTY_COMMENT);
            _switch(TokenizerStates::DATA);
            _appendComment();
          } else {
            _appendComment(u"-");
            _reconsume(TokenizerStates::COMMENT);
          }
          break;

        case TokenizerStates::COMMENT:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_COMMENT);
            _emitComment();
            _emitEof();
          } else if (_cc() == '<') {
            _appendComment();
            _switch(TokenizerStates::COMMENT_LESS_THAN_SIGN);
          } else if (_cc() == '-') {
            _switch(TokenizerStates::COMMENT_END_DASH);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementComment();
          } else {
            _appendComment();
          }
          break;

        case TokenizerStates::COMMENT_LESS_THAN_SIGN:
          if (!_eof()) {
            if (_cc() == '!') {
              _appendComment();
              _switch(TokenizerStates::COMMENT_LESS_THAN_SIGN_BANG);
              break;
            }
            if (_cc() == '<') {
              _appendComment();
              break;
            }
          }
          _reconsume(TokenizerStates::COMMENT);
          break;

        case TokenizerStates::COMMENT_LESS_THAN_SIGN_BANG:
          if (!_eof() && _cc() == '-') {
            _switch(TokenizerStates::COMMENT_LESS_THAN_SIGN_BANG_DASH);
          } else {
            _reconsume(TokenizerStates::COMMENT);
          }
          break;

        case TokenizerStates::COMMENT_LESS_THAN_SIGN_BANG_DASH:
          if (!_eof() && _cc() == '-') {
            _switch(TokenizerStates::COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH);
          } else {
            _reconsume(TokenizerStates::COMMENT_END_DASH);
          }
          break;

        case TokenizerStates::COMMENT_LESS_THAN_SIGN_BANG_DASH_DASH:
          if (_eof() || _cc() == '-') {
            _reconsume(TokenizerStates::COMMENT_END);
          } else {
            _error(ErrorKinds::NESTED_COMMENT);
            _reconsume(TokenizerStates::COMMENT_END);
          }
          break;

        case TokenizerStates::COMMENT_END_DASH:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_COMMENT);
            _emitComment();
            _emitEof();
          } else if (_cc() == '-') {
            _switch(TokenizerStates::COMMENT_END);
          } else {
            _appendComment(u"-");
            _reconsume(TokenizerStates::COMMENT);
          }
          break;

        case TokenizerStates::COMMENT_END:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_COMMENT);
            _emitComment();
            _emitEof();
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitComment();
          } else if (_cc() == '!') {
            _switch(TokenizerStates::COMMENT_END_BANG);
          } else if (_cc() == '-') {
            _appendComment();
          } else {
            _appendComment(u"-");
            _reconsume(TokenizerStates::COMMENT);
          }
          break;

        case TokenizerStates::COMMENT_END_BANG:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_COMMENT);
            _emitComment();
            _emitEof();
          } else if (_cc() == '-') {
            _appendComment(u"--!");
            _switch(TokenizerStates::COMMENT_END_DASH);
          } else if (_cc() == '>') {
            _error(ErrorKinds::INCORRECTLY_CLOSED_COMMENT);
            _switch(TokenizerStates::DATA);
            _emitComment();
          } else {
            _appendComment(u"--!");
            _reconsume(TokenizerStates::COMMENT);
          }
          break;

        case TokenizerStates::DOCTYPE:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _newDoctype();
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            _switch(TokenizerStates::BEFORE_DOCTYPE_NAME);
          } else if (_cc() == '>') {
            _reconsume(TokenizerStates::BEFORE_DOCTYPE_NAME);
          } else {
            _error(ErrorKinds::MISSING_WHITESPACE_BEFORE_DOCTYPE_NAME);
            _reconsume(TokenizerStates::BEFORE_DOCTYPE_NAME);
          }
          break;

        case TokenizerStates::BEFORE_DOCTYPE_NAME:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _newDoctype();
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            // ignore
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _newDoctype();
            _appendReplacementDoctypeName();
            _switch(TokenizerStates::DOCTYPE_NAME);
          } else if (_cc() == '>') {
            _error(ErrorKinds::MISSING_DOCTYPE_NAME);
            _newDoctype();
            _setDoctypeForceQuirks();
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _newDoctype();
            _appendDoctypeName();
            _switch(TokenizerStates::DOCTYPE_NAME);
          }
          break;

        case TokenizerStates::DOCTYPE_NAME:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            _switch(TokenizerStates::AFTER_DOCTYPE_NAME);
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementDoctypeName();
          } else {
            _appendDoctypeName();
          }
          break;

        case TokenizerStates::AFTER_DOCTYPE_NAME:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            // ignore
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else if (_areNextCharacters(u"PUBLIC", false)) {
            _consume(6);
            _switch(TokenizerStates::AFTER_DOCTYPE_PUBLIC_KEYWORD);
          } else if (_areNextCharacters(u"SYSTEM", false)) {
            _consume(6);
            _switch(TokenizerStates::AFTER_DOCTYPE_SYSTEM_KEYWORD);
          } else {
            _error(ErrorKinds::INVALID_CHARACTER_SEQUENCE_AFTER_DOCTYPE_NAME);
            _setDoctypeForceQuirks();
            _reconsume(TokenizerStates::BOGUS_DOCTYPE);
          }
          break;

        case TokenizerStates::AFTER_DOCTYPE_PUBLIC_KEYWORD:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            _switch(TokenizerStates::BEFORE_DOCTYPE_PUBLIC_IDENTIFIER);
          } else if (_cc() == '"') {
            _error(
                ErrorKinds::MISSING_WHITESPACE_AFTER_DOCTYPE_PUBLIC_KEYWORD);
            _initDoctypePublicIdentifier();
            _switch(TokenizerStates::DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED);
          } else if (_cc() == '\'') {
            _error(
                ErrorKinds::MISSING_WHITESPACE_AFTER_DOCTYPE_PUBLIC_KEYWORD);
            _initDoctypePublicIdentifier();
            _switch(TokenizerStates::DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED);
          } else if (_cc() == '>') {
            _error(ErrorKinds::MISSING_DOCTYPE_PUBLIC_IDENTIFIER);
            _setDoctypeForceQuirks();
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _error(ErrorKinds::MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER);
            _setDoctypeForceQuirks();
            _reconsume(TokenizerStates::BOGUS_DOCTYPE);
          }
          break;

        case TokenizerStates::BEFORE_DOCTYPE_PUBLIC_IDENTIFIER:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            // ignore
          } else if (_cc() == '"') {
            _initDoctypePublicIdentifier();
            _switch(TokenizerStates::DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED);
          } else if (_cc() == '\'') {
            _initDoctypePublicIdentifier();
            _switch(TokenizerStates::DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED);
          } else if (_cc() == '>') {
            _error(ErrorKinds::MISSING_DOCTYPE_PUBLIC_IDENTIFIER);
            _setDoctypeForceQuirks();
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _error(ErrorKinds::MISSING_QUOTE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER);
            _setDoctypeForceQuirks();
            _reconsume(TokenizerStates::BOGUS_DOCTYPE);
          }
          break;

        case TokenizerStates::DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '"') {
            _switch(TokenizerStates::AFTER_DOCTYPE_PUBLIC_IDENTIFIER);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementDoctypePublicIdentifier();
          } else if (_cc() == '>') {
            _error(ErrorKinds::ABRUPT_DOCTYPE_PUBLIC_IDENTIFIER);
            _setDoctypeForceQuirks();
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _appendDoctypePublicIdentifier();
          }
          break;

        case TokenizerStates::DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\'') {
            _switch(TokenizerStates::AFTER_DOCTYPE_PUBLIC_IDENTIFIER);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementDoctypePublicIdentifier();
          } else if (_cc() == '>') {
            _error(ErrorKinds::ABRUPT_DOCTYPE_PUBLIC_IDENTIFIER);
            _setDoctypeForceQuirks();
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _appendDoctypePublicIdentifier();
          }
          break;

        case TokenizerStates::AFTER_DOCTYPE_PUBLIC_IDENTIFIER:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            _switch(
                TokenizerStates::BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS
                );
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else if (_cc() == '"') {
            _error(
                ErrorKinds::
                MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS
                );
            _initDoctypeSystemIdentifier();
            _switch(TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED);
          } else if (_cc() == '\'') {
            _error(
                ErrorKinds::
                MISSING_WHITESPACE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS
                );
            _initDoctypeSystemIdentifier();
            _switch(TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED);
          } else {
            _error(ErrorKinds::MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER);
            _setDoctypeForceQuirks();
            _reconsume(TokenizerStates::BOGUS_DOCTYPE);
          }
          break;

        case TokenizerStates::BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            // ignore
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else if (_cc() == '"') {
            _initDoctypeSystemIdentifier();
            _switch(TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED);
          } else if (_cc() == '\'') {
            _initDoctypeSystemIdentifier();
            _switch(TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED);
          } else {
            _error(ErrorKinds::MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER);
            _setDoctypeForceQuirks();
            _reconsume(TokenizerStates::BOGUS_DOCTYPE);
          }
          break;

        case TokenizerStates::AFTER_DOCTYPE_SYSTEM_KEYWORD:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            _switch(TokenizerStates::BEFORE_DOCTYPE_SYSTEM_IDENTIFIER);
          } else if (_cc() == '"') {
            _error(
                ErrorKinds::MISSING_WHITESPACE_AFTER_DOCTYPE_SYSTEM_KEYWORD);
            _initDoctypeSystemIdentifier();
            _switch(TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED);
          } else if (_cc() == '\'') {
            _error(
                ErrorKinds::MISSING_WHITESPACE_AFTER_DOCTYPE_SYSTEM_KEYWORD);
            _initDoctypeSystemIdentifier();
            _switch(TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED);
          } else if (_cc() == '>') {
            _error(ErrorKinds::MISSING_DOCTYPE_SYSTEM_IDENTIFIER);
            _setDoctypeForceQuirks();
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _error(ErrorKinds::MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER);
            _setDoctypeForceQuirks();
            _reconsume(TokenizerStates::BOGUS_DOCTYPE);
          }
          break;

        case TokenizerStates::BEFORE_DOCTYPE_SYSTEM_IDENTIFIER:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            // ignore
          } else if (_cc() == '"') {
            _initDoctypeSystemIdentifier();
            _switch(TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED);
          } else if (_cc() == '\'') {
            _initDoctypeSystemIdentifier();
            _switch(TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED);
          } else if (_cc() == '>') {
            _error(ErrorKinds::MISSING_DOCTYPE_SYSTEM_IDENTIFIER);
            _setDoctypeForceQuirks();
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _error(ErrorKinds::MISSING_QUOTE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER);
            _setDoctypeForceQuirks();
            _reconsume(TokenizerStates::BOGUS_DOCTYPE);
          }
          break;

        case TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '"') {
            _switch(TokenizerStates::AFTER_DOCTYPE_SYSTEM_IDENTIFIER);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementDoctypeSystemIdentifier();
          } else if (_cc() == '>') {
            _error(ErrorKinds::ABRUPT_DOCTYPE_SYSTEM_IDENTIFIER);
            _setDoctypeForceQuirks();
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _appendDoctypeSystemIdentifier();
          }
          break;

        case TokenizerStates::DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\'') {
            _switch(TokenizerStates::AFTER_DOCTYPE_SYSTEM_IDENTIFIER);
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
            _appendReplacementDoctypeSystemIdentifier();
          } else if (_cc() == '>') {
            _error(ErrorKinds::ABRUPT_DOCTYPE_SYSTEM_IDENTIFIER);
            _setDoctypeForceQuirks();
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _appendDoctypeSystemIdentifier();
          }
          break;

        case TokenizerStates::AFTER_DOCTYPE_SYSTEM_IDENTIFIER:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_DOCTYPE);
            _setDoctypeForceQuirks();
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '\t' || _cc() == '\n' || _cc() == '\f' ||
              _cc() == ' ') {
            // ignore
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else {
            _error(
                ErrorKinds::
                UNEXPECTED_CHARACTER_AFTER_DOCTYPE_SYSTEM_IDENTIFIER
                );
            _reconsume(TokenizerStates::BOGUS_DOCTYPE);
          }
          break;

        case TokenizerStates::BOGUS_DOCTYPE:
          if (_eof()) {
            _emitDoctype();
            _emitEof();
          } else if (_cc() == '>') {
            _switch(TokenizerStates::DATA);
            _emitDoctype();
          } else if (_cc() == '\0') {
            _error(ErrorKinds::UNEXPECTED_NULL_CHARACTER);
          }
          break;

        case TokenizerStates::CDATA_SECTION:
          if (_eof()) {
            _error(ErrorKinds::EOF_IN_CDATA);
            _emitEof();
          } else if (_cc() == ']') {
            _switch(TokenizerStates::CDATA_SECTION_BRACKET);
          } else {
            _emitChar();
          }
          break;

        case TokenizerStates::CDATA_SECTION_BRACKET:
          if (!_eof() && _cc() == ']') {
            _switch(TokenizerStates::CDATA_SECTION_END);
          } else {
            _emitChar(']');
            _reconsume(TokenizerStates::CDATA_SECTION);
          }
          break;

        case TokenizerStates::CDATA_SECTION_END:
          if (!_eof()) {
            if (_cc() == ']') {
              _emitChar(']');
              break;
            }
            if (_cc() == '>') {
              _switch(TokenizerStates::DATA);
              break;
            }
          }
          _emitChar(']');
          _emitChar(']');
          _reconsume(TokenizerStates::CDATA_SECTION);
          break;

        case TokenizerStates::CHARACTER_REFERENCE:
          _resetTempBuffer();
          _appendTempBuffer('&');
          if (!_eof()) {
            if (_isAsciiAlphaNum()) {
              _reconsume(TokenizerStates::NAMED_CHARACTER_REFERENCE);
              break;
            }
            if (_cc() == '#') {
              _appendTempBuffer();
              _switch(TokenizerStates::NUMERIC_CHARACTER_REFERENCE);
              break;
            }
          }
          _flushTempBufferChars();
          _reconsumeReturnState();
          break;

        case TokenizerStates::NAMED_CHARACTER_REFERENCE:
          _macthNamedCharacterReference();
          break;

        case TokenizerStates::AMBIGUOUS_AMPERSAND:
          if (!_eof()) {
            if (_isAsciiAlphaNum()) {
              if (_inAttribute()) {
                _appendAttributeValue();
              } else {
                _emitChar();
              }
              break;
            }
            if (_cc() == ';') {
              _error(ErrorKinds::UNKNOWN_NAMED_CHARACTER_REFERENCE);
              _reconsumeReturnState();
              break;
            }
          }
          _reconsumeReturnState();
          break;

        case TokenizerStates::NUMERIC_CHARACTER_REFERENCE:
          _initReferenceCode();
          if (!_eof() && (_cc() == 'X' || _cc() == 'x')) {
            _appendTempBuffer();
            _switch(TokenizerStates::HEXADECIMAL_CHARACTER_REFERENCE_START);
          } else {
            _reconsume(TokenizerStates::DECIMAL_CHARACTER_REFERENCE_START);
          }
          break;

        case TokenizerStates::HEXADECIMAL_CHARACTER_REFERENCE_START:
          if (!_eof() && _isAsciiHexDigit()) {
            _reconsume(TokenizerStates::HEXADECIMAL_CHARACTER_REFERENCE);
          } else {
            _error(
                ErrorKinds::ABSENCE_OF_DIGITS_IN_NUMERIC_CHARACTER_REFERENCE);
            _flushTempBufferChars();
            _reconsumeReturnState();
          }
          break;

        case TokenizerStates::DECIMAL_CHARACTER_REFERENCE_START:
          if (!_eof() && _isAsciiDigit()) {
            _reconsume(TokenizerStates::DECIMAL_CHARACTER_REFERENCE);
          } else {
            _error(
                ErrorKinds::ABSENCE_OF_DIGITS_IN_NUMERIC_CHARACTER_REFERENCE);
            _flushTempBufferChars();
            _reconsumeReturnState();
          }
          break;

        case TokenizerStates::HEXADECIMAL_CHARACTER_REFERENCE:
          if (!_eof()) {
            if (_isAsciiHexDigit()) {
              _addHexReferenceCode();
              break;
            }
            if (_cc() == ';') {
              _switch(TokenizerStates::NUMERIC_CHARACTER_REFERENCE_END);
              break;
            }
          }
          _error(ErrorKinds::MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE);
          _reconsume(TokenizerStates::NUMERIC_CHARACTER_REFERENCE_END);
          break;

        case TokenizerStates::DECIMAL_CHARACTER_REFERENCE:
          if (!_eof()) {
            if (_isAsciiDigit()) {
              _addReferenceCode();
              break;
            }
            if (_cc() == ';') {
              _switch(TokenizerStates::NUMERIC_CHARACTER_REFERENCE_END);
              break;
            }
          }
          _error(ErrorKinds::MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE);
          _reconsume(TokenizerStates::NUMERIC_CHARACTER_REFERENCE_END);
          break;

        case TokenizerStates::NUMERIC_CHARACTER_REFERENCE_END:
          _emitFromReferenceCode();
          // As we consume by default, reconsume is needed
          _reconsumeReturnState();
          break;
      }
    }

  private:
    void _macthNamedCharacterReference() {
      auto matcher = Entities::matcher();
      auto res = matcher.match(_cc());
      _appendTempBuffer();

      while (res == EntityMatcher::CONTINUE) {
        _consume();
        res = matcher.match(_cc());
        _appendTempBuffer();
      }

      if (res == EntityMatcher::FOUND) {
        if (_inAttribute() && _cc() != ';' && _isNextEqualOrAsciiAlpha()) {
          _flushTempBufferChars();

          _switchReturnState();
        } else {
          if (_cc() != ';') {
            _error(ErrorKinds::MISSING_SEMICOLON_AFTER_CHARACTER_REFERENCE);
          }
          _resetTempBuffer();

          char16_t code1, code2;
          matcher.setFound(code1, code2);

          _appendTempBuffer(code1);
          if (code2) {
            _appendTempBuffer(code2);
          }

          _flushTempBufferChars();
          _switchReturnState();
        }
      } else {
        _flushTempBufferChars();
        _switch(TokenizerStates::AMBIGUOUS_AMPERSAND);
      }
    }

    void _reconsume(TokenizerStates state) {
      _switch(state);
      _tokenize();
    }

    void _reconsumeReturnState() {
      _switchReturnState();
      _tokenize();
    }
  };

}
}

#endif
