#ifndef ECLAIR_HTML_TEXT_EXTRACTOR
#define ECLAIR_HTML_TEXT_EXTRACTOR

#include <string>
#include <vector>

#include "eclair-html/document.h"
#include "eclair-html/node.h"

namespace eclair_html {
namespace html_parser {

  class TextExtractor {
  public:
    explicit TextExtractor(const Node& document)
      : _document(document) {
    }

    void processTitle(std::u16string& value) {
      _processElementTitle(_document, false, value);
    }

    void processContent(std::vector<std::u16string>& values) {
      _processElementContent(_document, false, values);
    }

    void processSubContent(std::u16string& value) {
      std::vector<std::u16string> values;
      _processElementContent(_document, true, values);
      for (auto& valueItem : values) {
        value += valueItem + u" ";
      }
    }

  private:
    enum class ElementInfo {
      NO_WALK_THROUGH,
      NO_CONTENT,
      HAS_CONTENT,
    };

    void _processElementTitle(const Node& node, bool emit,
        std::u16string& value) {
      for (auto& child : node) {
        if (child->kind() == NodeKinds::ELEMENT) {
            _processElementTitle(*child, child->name() == TagKinds::TITLE,
                value);
        } else if (child->kind() == NodeKinds::TEXT && emit) {
          value += child->content() + u" ";
        }
      }
    }

    void _processElementContent(const Node& node, bool emit,
        std::vector<std::u16string>& values) {
      for (auto& child : node) {
        if (child->kind() == NodeKinds::ELEMENT) {
          ElementInfo info = _elementInfo(*child);
          if (info == ElementInfo::HAS_CONTENT) {
            _processElementContent(*child, true, values);
          } else if (info == ElementInfo::NO_CONTENT) {
            _processElementContent(*child, false, values);
          }
        } else if (child->kind() == NodeKinds::TEXT && emit) {
          values.push_back(child->content());
        }
      }
    }

    ElementInfo _elementInfo(const Node& node) {
      if (node.name().nameSpace() != Namespaces::HTML) {
        // FIXME: can find textual content in both SVG & MATHML
        return ElementInfo::NO_WALK_THROUGH;
      }

      switch (node.name().kind()) {
        case TagKinds::HTML:
        case TagKinds::HEAD:
        case TagKinds::TITLE:
          return ElementInfo::NO_CONTENT;

        case TagKinds::AREA:
        case TagKinds::AUDIO:
        case TagKinds::BASE:
        case TagKinds::BR:
        case TagKinds::CANVAS:
        case TagKinds::EMBED:
        case TagKinds::HR:
        case TagKinds::IFRAME:
        case TagKinds::IMG:
        case TagKinds::LINK:
        case TagKinds::MAP:
        case TagKinds::META:
        case TagKinds::OBJECT:
        case TagKinds::PARAM:
        case TagKinds::PICTURE:
        case TagKinds::SCRIPT:
        case TagKinds::SOURCE:
        case TagKinds::STYLE:
        case TagKinds::TRACK:
        case TagKinds::VIDEO:
        case TagKinds::WBR:
          return ElementInfo::NO_WALK_THROUGH;

        default:
          return ElementInfo::HAS_CONTENT;
      }
    }

    const Node& _document;
  };

}
}

#endif
