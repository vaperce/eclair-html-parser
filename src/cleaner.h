#ifndef ECLAIR_HTML_CLEANER
#define ECLAIR_HTML_CLEANER

#include <string>

#include "common/tools.h"
#include "eclair-html/document.h"
#include "eclair-html/node.h"

namespace eclair_html {
namespace html_parser {

  char operator &(char v1, CleanFlags v2) {
    return v1 & static_cast<char>(v2);
  }

  class Cleaner {
  public:
    explicit Cleaner(Node& document, char flags)
      : _document(document), _flags(flags) {
    }

    void process() {
      if (_flags != 0) {
        _processNode(_document, false);
      }
    }

  private:
    void _processNode(Node& node, bool pre) {
      std::vector<Node*> toRemove;

      for (auto& subNode : node) {
        if (subNode->kind() == NodeKinds::ELEMENT) {
          if (_flags & CleanFlags::SCRIPT &&
              subNode->name() == TagKinds::SCRIPT) {
            toRemove.push_back(subNode.get());
          } else if (_flags & CleanFlags::STYLE &&
              subNode->name() == TagKinds::STYLE) {
            toRemove.push_back(subNode.get());
          } else if (_flags & CleanFlags::LINK &&
              subNode->name() == TagKinds::LINK) {
            toRemove.push_back(subNode.get());
          } else if (subNode->name() == TagKinds::PRE) {
            _processNode(*subNode, true);
          } else {
            _processNode(*subNode, pre);
          }
        } else if (_flags & CleanFlags::COMMENT &&
            subNode->kind() == NodeKinds::COMMENT) {
          toRemove.push_back(subNode.get());
        } else if (_flags & CleanFlags::SPACE &&
          subNode->kind() == NodeKinds::TEXT) {
          if (!pre) {
            _handleText(*subNode, toRemove);
          }
        }
      }

      for (auto subNode : toRemove) {
        node.erase(*subNode);
      }
    }

    void _handleText(Node& node, std::vector<Node*>& toRemove) {
      std::u16string content = _removeDoubleSpaces(node.content());
      if (content.empty()) {
        toRemove.push_back(&node);
        return;
      }
      node.setContent(content);
    }

    std::u16string _removeDoubleSpaces(const std::u16string& input) {
      std::u16string res;
      bool foundSpace = false;

      for (char16_t c : input) {
        if (Tools::isSpace(c)) {
          if (!foundSpace) {
            foundSpace = true;
            res += ' ';
          }
        } else {
          foundSpace = false;
          res += c;
        }
      }

      return res;
    }

    Node& _document;
    char _flags;
  };

}
}

#endif
