#ifndef ECLAIR_HTML_RENDERER
#define ECLAIR_HTML_RENDERER

#include <string>
#include <vector>

#include "eclair-html/node.h"

namespace eclair_html {
namespace html_parser {

  class Renderer {
  public:
    explicit Renderer(Node& root, bool prettyPrint, const char* forceCharset)
      : _root(root), _prettyPrint(prettyPrint), _forceCharset(forceCharset),
        _indentLevel(0), _textReplacor(_buffer, {{'<', u"&lt;"}}),
        _attributeValueReplacor(_buffer, {{'<', u"&lt;"}, {'\"', u"&quot;"}}) {
    }

    std::u16string render() {
      _indent(false);
      _buffer += u"<!doctype html>";
      _newline(false);

      for (auto& node : _root) {
        _renderNode(*node, false);
      }
      return _buffer;
    }

  private:
    class _Replacor {
    public:
      struct _Replacement {
        const char16_t character;
        const std::u16string value;
      };

      _Replacor(std::u16string& out, std::vector<_Replacement>&& replacements)
        : _out(out), _replacements(replacements) {
      }

      void process(const std::u16string& input) {
        for (char16_t c : input) {
          bool found = false;
          for (auto replacement : _replacements) {
            if (c == replacement.character) {
              _out += replacement.value;
              found = true;
              break;
            }
          }

          if (!found) {
            _out += c;
          }
        }
      }

    private:
      std::u16string& _out;
      const std::vector<_Replacement> _replacements;
    };

    void _indent(bool pre) {
      if (_prettyPrint && !pre) {
        for (unsigned i = 0; i < _indentLevel * 2; ++i) {
          _buffer += ' ';
        }
      }
    }

    void _newline(bool pre) {
      if (_prettyPrint && !pre) {
        _buffer += '\n';
      }
    }

    void _renderAttributes(Node& element) {
      for (auto attribute : element.attributes) {
        _buffer += u" " + attribute.first;
        _appendAttributeValue(attribute.second);
      }
    }

    void _renderStartTag(Node& element, bool selfClosing) {
      _buffer += u"<" + element.name().value();

      _renderAttributes(element);

      if (selfClosing) {
        _buffer += u" />";
      } else {
        _buffer += u">";
      }
    }

    void _addForcedCharset(Node& element) {
      if (_forceCharset && element.name() == TagKinds::HEAD) {
        _indent(false);
        _buffer += u"<meta charset=\"";
        for (char c : std::string(_forceCharset)) {
          _buffer += c;
        }
        _buffer += u"\"/>";
        _newline(false);
      }
    }

    void _renderElement(Node& element, bool pre) {
      if (_forceCharset && element.name() == TagKinds::META) {
        auto& attributes = element.attributes;
        if (attributes.find(u"charset") != attributes.end()) {
          return;
        }

        auto httpEquiv = attributes.find(u"http-equiv");
        if (httpEquiv != attributes.end() &&
            httpEquiv->second == u"content-type") {
          return;
        }
      }

      bool selfClosing = _isSelfClosing(element);
      _indent(pre);
      _renderStartTag(element, selfClosing);
      _newline(pre);

      _indentLevel++;
      _addForcedCharset(element);
      for (auto& subNode : element) {
        _renderNode(*subNode, pre || element.name() == TagKinds::PRE);
      }
      _indentLevel--;

      if (!selfClosing) {
        _indent(pre);
        _buffer += u"</" + element.name().value() + u">";
        _newline(pre);
      }
    }

    void _renderText(const std::u16string& content, bool pre) {
      if (_prettyPrint && !pre) {
        if (content.size() > 0 && content != u" ") {
          _indent(false);
          if (content.front() == ' ' && content.back() == ' ') {
            _textReplacor.process(content.substr(1, content.size() - 2));
          } else if (content.front() == ' ') {
            _textReplacor.process(content.substr(1));
          } else if (content.back() == ' ') {
            _textReplacor.process(content.substr(0, content.size() - 1));
          } else {
            _textReplacor.process(content);
          }
          _newline(false);
        }
      } else {
        _textReplacor.process(content);
      }
    }

    void _renderNode(Node& node, bool pre) {
      if (node.kind() == NodeKinds::ELEMENT) {
        _renderElement(node, pre);
      } else if (node.kind() == NodeKinds::COMMENT) {
        _indent(pre);
        _buffer += u"<!--" + node.content() + u"-->";
        _newline(pre);
      } else {
        _renderText(node.content(), pre);
      }
    }

    bool _isSelfClosing(const Node& element) {
      if (element.name().nameSpace() != Namespaces::HTML) {
        return element.empty();
      }

      switch (element.name().kind()) {
        case TagKinds::AREA:
        case TagKinds::BASE:
        case TagKinds::BASEFONT:
        case TagKinds::BGSOUND:
        case TagKinds::BR:
        case TagKinds::COL:
        case TagKinds::EMBED:
        case TagKinds::FRAME:
        case TagKinds::HR:
        case TagKinds::IMG:
        case TagKinds::INPUT:
        case TagKinds::KEYGEN:
        case TagKinds::LINK:
        case TagKinds::META:
        case TagKinds::PARAM:
        case TagKinds::SOURCE:
        case TagKinds::TRACK:
        case TagKinds::WBR:
          return true;

        default:
          return false;
      }
    }

    void _appendAttributeValue(const std::u16string& content) {
      _buffer += u"=\"";
      if (!content.empty()) {
        _attributeValueReplacor.process(content);
      }
      _buffer += '"';
    }

    Node& _root;
    bool _prettyPrint;
    const char* _forceCharset;

    std::u16string _buffer;
    unsigned _indentLevel;

    _Replacor _textReplacor;
    _Replacor _attributeValueReplacor;
  };

}
}

#endif
