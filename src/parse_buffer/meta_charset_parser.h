#ifndef ECLAIR_HTML_PARSE_BUFFER_META_CHARSET_PARSER
#define ECLAIR_HTML_PARSE_BUFFER_META_CHARSET_PARSER

#include <string>
#include <utility>

namespace xyz {
namespace html_parser {

  class MetaCharsetParser {
  public:
    MetaCharsetParser(const char* data, std::size_t length)
      : _data(data), _length(length < MAX_LENGTH ? length : MAX_LENGTH),
        _position(0) {
    }

    std::string parse() {
      while (_position < _length) {
        if (_data[_position] == '<') {
          if (_isSequence("!--", false)) {
            _position += 2;
            _findCommentEnd();
            continue;
          }
          if (_isSequence("meta", true)) {
            if (_position + 5 < _length && (_data[_position + 5] == '/' ||
                  _isSpace(_data[_position + 5]))) {
              _position += 5;
              std::string res = _parseMetaAttributes();
              if (!res.empty()) {
                return res;
              }
              continue;
            }
          }
          if (_isTag()) {
            _findTagEnd();
            continue;
          }
          if (_position + 1 < _length && (_data[_position + 1] == '!' ||
                _data[_position + 1] == '/' || _data[_position + 1] == '?')) {
            _position += 2;
            _findSimpleTagEnd();
            continue;
          }
        }
        _position++;
      }

      return "";
    }

  private:
    bool _isSpace(char c) {
      return c == '\t' || c == '\r' || c == '\f' || c == '\r' || c == ' ';
    }

    bool _isSequence(const char* sequence, bool caseSensitive) {
      std::size_t i = 0;
      for (; _position + i + 1 < _length && sequence[i] != '\0';
          i++) {
        if (caseSensitive) {
          if (_data[_position + i + 1] != sequence[i]) {
            return false;
          }
        } else {
          if (_data[_position + i + 1] != sequence[i] &&
              _data[_position + i + 1] != sequence[i] - 0x20) {
            return false;
          }
        }
      }

      return _position + i < _length && sequence[i] == '\0';
    }

    bool _isTag() {
      std::size_t position = _position + 1;
      if (position >= _length) {
        return false;
      }

      if (_data[position] == '/') {
        position++;
        if (position < _length) {
          return false;
        }
      }

      if ((_data[position] >= 'A' && _data[position] <= 'Z') ||
          (_data[position] >= 'a' && _data[position] <= 'z')) {
        _position = position;
        return true;
      }
      return false;
    }

    void _findSimpleTagEnd() {
      std::size_t position = _position;
      for (; position < _length; position++) {
        if (_data[position] == '>') {
          break;
        }
      }
      _position = position + 1;
    }

    void _findTagEnd() {
      std::size_t position = _position;
      for (; position < _length; position++) {
        if (_data[position] == '>' || _isSpace(_data[position])) {
          _position = position;
          _parseAllAttributes();
          return;
        }
      }
      _position = position + 1;
    }

    void _findCommentEnd() {
      std::size_t position = _position;
      for (; position < _length; position++) {
        if (_data[position] == '-' && position + 2 < _length &&
            _data[position + 1] == '-' && _data[position + 2] == '>') {
          position += 2;
          break;
        }
      }
      _position = position + 1;
    }

    void _ignoreSpaces() {
      for (;_position < _length && _isSpace(_data[_position]); _position++) {
      }
    }

    char _lowerCase(char c) {
      if (c >= 'A' && c <= 'Z') {
        return c + 0x20;
      }
      return c;
    }

    enum class _ParseResults {
      OK,
      ERROR,
      STOP,
    };

    _ParseResults _parseAttributeName(std::string& name) {
      for (;_position < _length && (_isSpace(_data[_position]) ||
            _data[_position] == '/'); _position++) {
      }

      while (_position < _length) {
        char c = _data[_position];
        if (c == '=') {
          _position++;
          if (name.empty()) {
            return _ParseResults::ERROR;
          }
          return _ParseResults::OK;
        }
        if (_isSpace(c)) {
          break;
        }
        if (c == '/' || c == '>') {
          return _ParseResults::STOP;
        }
        name += _lowerCase(c);
        _position++;
      }

      if (_position >= _length) {
        return _ParseResults::STOP;
      }
      _ignoreSpaces();
      if (_position >= _length) {
        return _ParseResults::STOP;
      }

      if (_data[_position] == '=') {
        _position++;
        return _ParseResults::OK;
      }
      return _ParseResults::ERROR;
    }

    _ParseResults _parseAttributeValue(std::string& value) {
      _ignoreSpaces();
      if (_position >= _length) {
        return _ParseResults::STOP;
      }

      char c = _data[_position];
      if (c == '\'' || c == '"') {
        _position++;
        while (_position < _length && _data[_position] != c) {
          value += _lowerCase(_data[_position]);
          _position++;
        }
        if (_position >= _length) {
          return _ParseResults::STOP;
        }
        _position++;
        return _ParseResults::OK;
      }

      while (_position < _length) {
        c = _data[_position];
        if (_isSpace(c)) {
          return _ParseResults::OK;
        }
        if (c == '>') {
          return _ParseResults::STOP;
        }
        value += _lowerCase(_data[_position]);
        _position++;
      }
      return _ParseResults::STOP;
    }

    _ParseResults _parseAttribute(std::string& name, std::string& value) {
      name.clear();
      value.clear();

      _ParseResults nameRes = _parseAttributeName(name);
      if (nameRes != _ParseResults::OK) {
        return nameRes;
      }
      return _parseAttributeValue(value);
    }

    void _parseAllAttributes() {
      std::string name;
      std::string value;
      while (_parseAttribute(name, value) != _ParseResults::STOP) {
      }
    }

    enum class _NeedPragmaValues {
      PEMPTY,
      PTRUE,
      PFALSE,
    };

    std::string _parseMetaAttributes() {
      std::string charset;

      std::string name;
      std::string value;

      bool gotPragma = false;
      _NeedPragmaValues needPragma = _NeedPragmaValues::PEMPTY;

      bool httpEquivFound = false;
      bool contentFound = false;
      bool charsetFound = false;

      _ParseResults res = _ParseResults::OK;
      while (res != _ParseResults::STOP) {
        res = _parseAttribute(name, value);
        if (!name.empty() && !value.empty()) {
          if (name == "http-equiv" && !httpEquivFound) {
            if (value == "content-type") {
              gotPragma = true;
            }
            httpEquivFound = true;
          }
          if (name == "content" && !contentFound) {
            if (charset.empty()) {
              charset = _fromContent(value);
              needPragma = _NeedPragmaValues::PTRUE;
            }
            contentFound = true;
          }
          if (name == "charset" && !charsetFound) {
            charset = value;
            needPragma = _NeedPragmaValues::PFALSE;
            charsetFound = true;
          }
        }
      }

      if (needPragma == _NeedPragmaValues::PEMPTY) {
        return "";
      }
      if (needPragma == _NeedPragmaValues::PTRUE && !gotPragma) {
        return "";
      }

      if (charset == "utf-16" || charset == "utf-16be" ||
          charset == "utf-16le") {
        charset = "utf-8";
      } else if (charset == "x-user-defined") {
        charset = "windows-1252";
      }

      return charset;
    }

    std::string _fromContent(const std::string& input) {
      std::size_t pos = 0;
      while (pos < input.size()) {
        pos = input.find("charset", pos);
        if (pos == std::string::npos) {
          return "";
        }

        pos += 7;
        for (;pos < input.size() && _isSpace(pos); ++pos) {
        }
        if (pos >= input.size()) {
          return "";
        }
        if (input[pos] != '=') {
          continue;
        }
        pos++;
        for (;pos < input.size() && _isSpace(pos); ++pos) {
        }
        if (pos >= input.size()) {
          return "";
        }

        std::string res;
        char c = input[pos];
        if (c == '\'' || c == '"') {
          pos++;
          while (pos < input.size() && input[pos] != c) {
            res += input[pos];
            pos++;
          }
          if (pos >= input.size()) {
            return "";
          }
          return res;
        }

        while (pos < input.size() && !_isSpace(input[pos] &&
              input[pos] != ';')) {
          res += input[pos];
          pos++;
        }
        return res;
      }

      return "";
    }

    const std::size_t MAX_LENGTH = 1024;

    const char* _data;
    std::size_t _length;
    std::size_t _position;
  };

}
}

#endif
