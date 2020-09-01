#ifndef ECLAIR_HTML_TOKENIZER_BASE_STATE_HANDLER
#define ECLAIR_HTML_TOKENIZER_BASE_STATE_HANDLER

#include "common/error_manager.h"
#include "common/token_receiver.h"
#include "common/tokens.h"
#include "common/tools.h"
#include "eclair-html/errors.h"
#include "tokenizer/config.h"
#include "tokenizer/state_logger.h"
#include "tokenizer/states.h"

namespace eclair_html {
namespace html_parser {

class BaseStateHandler {
public:
  BaseStateHandler(TokenReceiver& tokenReceiver, const std::u16string& data,
                   TokenizerConfig& config, ErrorManager& errorManager)
      : _tokenReceiver(tokenReceiver), _config(config),
        _errorManager(errorManager), _eofValue(false),
        _stateValue(TokenizerStates::DATA), _returnState(TokenizerStates::DATA),
        _begin(data.begin()), _end(data.end()), _iterator(data.begin()),
        _startTag(true), _selfClosing(false), _docTypeInfo(new DocTypeInfo()),
        _referenceCode(0) {
    StateLogger::log(TokenizerStates::DATA);
  }

protected:
  void _checkTextInsertionMode() {
    switch (_config.readingMode) {
    case TextReadingModes::RAW_TEXT:
      _switch(TokenizerStates::RAWTEXT);
      break;

    case TextReadingModes::RCDATA:
      _switch(TokenizerStates::RCDATA);
      break;

    case TextReadingModes::SCRIPT_DATA:
      _switch(TokenizerStates::SCRIPT_DATA);
      break;

    case TextReadingModes::PLAINTEXT:
      _switch(TokenizerStates::PLAINTEXT);
      break;

    default:
      return;
    }

    _config.readingMode = TextReadingModes::NORMAL;
  }

  void _consume() {
    if (_iterator == _end) {
      _eofValue = true;
      return;
    }

    _ccValue = *(_iterator++);
    if (_ccValue == '\r') {
      if (_iterator != _end && *_iterator == '\n') {
        _iterator++;
      }
      _ccValue = '\n';
      _position.line++;
      _position.lineOffset = 0;
    } else if (_ccValue == '\n') {
      _position.line++;
      _position.lineOffset = 0;
    } else {
      _position.lineOffset++;
    }
  }

  inline bool _eof() const { return _eofValue; }

  inline char16_t _cc() const { return _ccValue; }

  inline TokenizerStates _state() const { return _stateValue; }

  void _consume(int nr) {
    // We always have consumed one already
    for (int i = 0; i < nr - 1; ++i) {
      _consume();
    }
  }

  bool _isAsciiAlpha() { return _isAsciiAlpha(_cc()); }

  bool _isAsciiDigit() { return _cc() >= '0' && _cc() <= '9'; }

  bool _isAsciiAlphaNum() { return _isAsciiAlpha() || _isAsciiDigit(); }

  bool _isAsciiHexDigit() {
    return _isAsciiDigit() || (_cc() >= 'a' && _cc() <= 'f') ||
           (_cc() >= 'A' && _cc() <= 'F');
  }

  bool _isAppropriateEndTag() {
    return _genericBuffer == _config.expectedEndTag;
  }

  bool _isTempBuffer(const char16_t* content) { return _tempBuffer == content; }

  bool _areNextCharacters(const std::u16string& value, bool caseSensitive) {
    if (_iterator == _begin) {
      return false;
    }

    auto it = _iterator - 1;
    unsigned i = 0;
    if (caseSensitive) {
      for (; it != _end && i < value.size(); i++, ++it) {
        if (*it != value[i]) {
          return false;
        }
      }
    } else {
      for (; it != _end && i < value.size(); i++, ++it) {
        if (Tools::toLowercase(*it) != Tools::toLowercase(value[i])) {
          return false;
        }
      }
    }

    return i == value.size();
  }

  bool _isNextEqualOrAsciiAlpha() {
    if (_iterator != _end) {
      if (*_iterator == '=') {
        return true;
      }
      if (_isAsciiAlpha(*_iterator)) {
        return true;
      }
    }

    return false;
  }

  bool _isHTMLNamespace() { return _config.isHTMLNamespace(); }

  bool _inAttribute() {
    return _returnState == TokenizerStates::ATTRIBUTE_VALUE_DOUBLE_QUOTED ||
           _returnState == TokenizerStates::ATTRIBUTE_VALUE_SINGLE_QUOTED ||
           _returnState == TokenizerStates::ATTRIBUTE_VALUE_UNQUOTED;
  }

  void _switch(TokenizerStates state) {
    StateLogger::log(state);
    _stateValue = state;
  }

  void _switchReturnState() { _switch(_returnState); }

  void _saveReturnState() { _returnState = _stateValue; }

  const Position& _getPosition() {
    if (_savedPosition) {
      return *_savedPosition;
    }
    return _position;
  }

  void _error(ErrorKinds error) {
    if (_savedPosition) {
      _errorManager.add(error, *_savedPosition);
    } else {
      _errorManager.add(error, _position);
    }
  }

  void _resetTempBuffer() { _tempBuffer.clear(); }

  void _newStartTag() {
    _startTag = true;
    _genericBuffer.clear();
    _selfClosing = false;
    _attributes.clear();
    _attributeNameBuffer.clear();
    _attributeValueBuffer.clear();
    _savedPosition.reset(new Position(_position));
  }

  void _newEndTag() {
    _startTag = false;
    _genericBuffer.clear();
    _selfClosing = false;
    _attributes.clear();
    _attributeNameBuffer.clear();
    _attributeValueBuffer.clear();
    _savedPosition.reset(new Position(_position));
  }

  void _newAttribute() {
    _pushAttribute();
    _attributeNameBuffer.clear();
    _attributeValueBuffer.clear();
  }

  void _newComment() {
    _genericBuffer.clear();
    _savedPosition.reset(new Position(_position));
  }

  void _newDoctype() {
    _docTypeInfo.reset(new DocTypeInfo());
    _savedPosition.reset(new Position(_position));
  }

  void _appendTempBuffer(char16_t value = '\0') {
    if (value == '\0') {
      _tempBuffer += _cc();
    } else {
      _tempBuffer += value;
    }
  }

  void _appendTempBufferLower() { _tempBuffer += Tools::toLowercase(_cc()); }

  void _appendTagName() { _genericBuffer += Tools::toLowercase(_cc()); }

  void _appendAttributeName() {
    _attributeNameBuffer += Tools::toLowercase(_cc());
  }

  void _appendReplacementAttributeName() {
    _attributeNameBuffer += REPLACEMENT_CHAR;
  }

  void _appendAttributeValue() { _attributeValueBuffer += _cc(); }

  void _appendReplacementAttributeValue() {
    _attributeValueBuffer += REPLACEMENT_CHAR;
  }

  void _appendComment(const char16_t* value = nullptr) {
    if (value) {
      _genericBuffer += value;
    } else {
      _genericBuffer.push_back(_cc());
    }
  }

  void _appendReplacementComment() { _genericBuffer += REPLACEMENT_CHAR; }

  void _appendDoctypeName() { _docTypeInfo->name += Tools::toLowercase(_cc()); }

  void _appendReplacementDoctypeName() {
    _docTypeInfo->name += REPLACEMENT_CHAR;
  }

  void _initDoctypePublicIdentifier() {
    _docTypeInfo->publicIdentifier.reset(new std::u16string());
  }

  void _appendDoctypePublicIdentifier() {
    if (_docTypeInfo->publicIdentifier) {
      *_docTypeInfo->publicIdentifier += _cc();
    }
  }

  void _appendReplacementDoctypePublicIdentifier() {
    if (_docTypeInfo->publicIdentifier) {
      *_docTypeInfo->publicIdentifier += REPLACEMENT_CHAR;
    }
  }

  void _initDoctypeSystemIdentifier() {
    _docTypeInfo->systemIdentifier.reset(new std::u16string());
  }

  void _appendDoctypeSystemIdentifier() {
    if (_docTypeInfo->systemIdentifier) {
      *_docTypeInfo->systemIdentifier += _cc();
    }
  }

  void _appendReplacementDoctypeSystemIdentifier() {
    if (_docTypeInfo->systemIdentifier) {
      *_docTypeInfo->systemIdentifier += REPLACEMENT_CHAR;
    }
  }

  void _initReferenceCode() { _referenceCode = 0; }

  void _addReferenceCode() {
    _referenceCode *= 10;
    _referenceCode += _cc() - 0x30;
  }

  void _addHexReferenceCode() {
    _referenceCode *= 16;
    if (_cc() >= '0' && _cc() <= '9') {
      _referenceCode += _cc() - 0x30;
    } else if (_cc() >= 'A' && _cc() <= 'F') {
      _referenceCode += _cc() - 0x37;
    } else if (_cc() >= 'a' && _cc() <= 'f') {
      _referenceCode += _cc() - 0x57;
    }
  }

  void _setSelfClosing() { _selfClosing = true; }

  void _setDoctypeForceQuirks() {
    if (_docTypeInfo->systemIdentifier) {
      _docTypeInfo->forceQuirks = true;
    }
  }

  void _flushTempBufferChars() {
    if (_inAttribute()) {
      _attributeValueBuffer += _tempBuffer;
    } else {
      _emitTempBufferChars();
    }
  }

  void _emitEof() {
    Token token(_position);
    _tokenReceiver.process(token);
  }

  void _emitTag() {
    _pushAttribute();

    if (_startTag) {
      Token token(_getPosition(), _genericBuffer, _attributes, _selfClosing);
      _tokenReceiver.process(token);
    } else {
      if (!_attributes.empty()) {
        _error(ErrorKinds::END_TAG_WITH_ATTRIBUTES);
      }
      if (_selfClosing) {
        _error(ErrorKinds::END_TAG_WITH_TRAILING_SOLIDUS);
      }
      Token token(_getPosition(), _genericBuffer);
      _tokenReceiver.process(token);
    }

    _savedPosition.reset();
  }

  void _emitChar() {
    Token token(_getPosition(), _cc());
    _tokenReceiver.process(token);
    _savedPosition.reset();
  }

  void _emitChar(char16_t c) {
    Token token(_getPosition(), c);
    _tokenReceiver.process(token);
    _savedPosition.reset();
  }

  void _emitFromReferenceCode() {
    if (_referenceCode == 0) {
      _error(ErrorKinds::NULL_CHARACTER_REFERENCE);
      _referenceCode = REPLACEMENT_CHAR;
    } else if (_referenceCode > 0x10FFFF) {
      _error(ErrorKinds::CHARACTER_REFERENCE_OUTSIDE_UNICODE_RANGE);
      _referenceCode = REPLACEMENT_CHAR;
    } else if (_referenceCode >= 0xD800 && _referenceCode <= 0xDFFF) {
      _error(ErrorKinds::SURROGATE_CHARACTER_REFERENCE);
      _referenceCode = REPLACEMENT_CHAR;
    } else if (_referenceCodeNonCharacter()) {
      _error(ErrorKinds::NONCHARACTER_CHARACTER_REFERENCE);
      _referenceCode = REPLACEMENT_CHAR;
    } else if (_referenceCode == 0xD || _referenceCodeControlAndNotSpace()) {
      _error(ErrorKinds::CONTROL_CHARACTER_REFERENCE);
      if (!_replaceReferenceCodeControl()) {
        _referenceCode = REPLACEMENT_CHAR;
      }
    }

    _resetTempBuffer();
    char16_t code1, code2;
    Tools::utf32ToUtf16(_referenceCode, code1, code2);

    _appendTempBuffer(code1);
    if (code2) {
      _appendTempBuffer(code2);
    }

    _flushTempBufferChars();
  }

  void _emitReplacementChar() {
    Token token(_position, REPLACEMENT_CHAR);
    _tokenReceiver.process(token);
  }

  void _emitTempBufferChars() {
    for (auto c : _tempBuffer) {
      _emitChar(c);
    }
  }

  void _emitComment() {
    Token token(_genericBuffer, _getPosition());
    _tokenReceiver.process(token);
    _savedPosition.reset();
  }

  void _emitDoctype() {
    Token token(_getPosition(), std::move(_docTypeInfo));
    _tokenReceiver.process(token);
    _savedPosition.reset();
  }

private:
  static bool _isAsciiAlpha(char16_t c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }

  bool _referenceCodeNonCharacter() {
    if (_referenceCode >= 0xFDD0 && _referenceCode <= 0xFDEF) {
      return true;
    }

    switch (_referenceCode) {
    case 0xFFFE:
    case 0xFFFF:
    case 0x1FFFE:
    case 0x1FFFF:
    case 0x2FFFE:
    case 0x2FFFF:
    case 0x3FFFE:
    case 0x3FFFF:
    case 0x4FFFE:
    case 0x4FFFF:
    case 0x5FFFE:
    case 0x5FFFF:
    case 0x6FFFE:
    case 0x6FFFF:
    case 0x7FFFE:
    case 0x7FFFF:
    case 0x8FFFE:
    case 0x8FFFF:
    case 0x9FFFE:
    case 0x9FFFF:
    case 0xAFFFE:
    case 0xAFFFF:
    case 0xBFFFE:
    case 0xBFFFF:
    case 0xCFFFE:
    case 0xCFFFF:
    case 0xDFFFE:
    case 0xDFFFF:
    case 0xEFFFE:
    case 0xEFFFF:
    case 0xFFFFE:
    case 0xFFFFF:
    case 0x10FFFE:
    case 0x10FFFF:
      return true;

    default:
      return false;
    }
  }

  bool _referenceCodeControlAndNotSpace() {
    return (_referenceCode != 0x20 && _referenceCode >= 0 &&
            _referenceCode <= 0x1F) ||
           (_referenceCode >= 0x7F && _referenceCode <= 0x9F);
  }

  bool _replaceReferenceCodeControl() {
    switch (_referenceCode) {
    case 0x80:
      _referenceCode = 0x20AC;
      break;
    case 0x82:
      _referenceCode = 0x201A;
      break;
    case 0x83:
      _referenceCode = 0x0192;
      break;
    case 0x84:
      _referenceCode = 0x201E;
      break;
    case 0x85:
      _referenceCode = 0x2026;
      break;
    case 0x86:
      _referenceCode = 0x2020;
      break;
    case 0x87:
      _referenceCode = 0x2021;
      break;
    case 0x88:
      _referenceCode = 0x02C6;
      break;
    case 0x89:
      _referenceCode = 0x2030;
      break;
    case 0x8A:
      _referenceCode = 0x0160;
      break;
    case 0x8B:
      _referenceCode = 0x2039;
      break;
    case 0x8C:
      _referenceCode = 0x0152;
      break;
    case 0x8E:
      _referenceCode = 0x017D;
      break;
    case 0x91:
      _referenceCode = 0x2018;
      break;
    case 0x92:
      _referenceCode = 0x2019;
      break;
    case 0x93:
      _referenceCode = 0x201C;
      break;
    case 0x94:
      _referenceCode = 0x201D;
      break;
    case 0x95:
      _referenceCode = 0x2022;
      break;
    case 0x96:
      _referenceCode = 0x2013;
      break;
    case 0x97:
      _referenceCode = 0x2014;
      break;
    case 0x98:
      _referenceCode = 0x02DC;
      break;
    case 0x99:
      _referenceCode = 0x2122;
      break;
    case 0x9A:
      _referenceCode = 0x0161;
      break;
    case 0x9B:
      _referenceCode = 0x203A;
      break;
    case 0x9C:
      _referenceCode = 0x0153;
      break;
    case 0x9E:
      _referenceCode = 0x017E;
      break;
    case 0x9F:
      _referenceCode = 0x0178;
      break;
    default:
      return false;
    }
    return true;
  }

  void _pushAttribute() {
    if (_attributeNameBuffer.empty()) {
      return;
    }

    auto insertRet = _attributes.insert(
        std::make_pair(_attributeNameBuffer, _attributeValueBuffer));
    if (!insertRet.second) {
      _error(ErrorKinds::DUPLICATE_ATTRIBUTE);
    }
  }

  const char16_t REPLACEMENT_CHAR = 0xfffd;

  TokenReceiver& _tokenReceiver;
  TokenizerConfig& _config;
  ErrorManager& _errorManager;

  bool _eofValue;
  char16_t _ccValue;
  TokenizerStates _stateValue;
  TokenizerStates _returnState;

  std::u16string::const_iterator _begin;
  std::u16string::const_iterator _end;
  std::u16string::const_iterator _iterator;

  std::u16string _tempBuffer;
  std::u16string _genericBuffer;

  bool _startTag;
  bool _selfClosing;
  Node::Attributes _attributes;
  std::u16string _attributeNameBuffer;
  std::u16string _attributeValueBuffer;

  std::unique_ptr<DocTypeInfo> _docTypeInfo;

  int _referenceCode;

  Position _position;
  std::unique_ptr<Position> _savedPosition;
};

} // namespace html_parser
} // namespace eclair_html

#endif
