#ifndef ECLAIR_HTML_TREE_BUILDER_OPEN_ELEMENTS
#define ECLAIR_HTML_TREE_BUILDER_OPEN_ELEMENTS

#include <list>
#include <memory>
#include <vector>

#include "common/tokens.h"
#include "eclair-html/node.h"
#include "eclair-html/tag_names.h"

namespace xyz {
namespace html_parser {

  enum class ScopeKinds {
    STANDARD,
    LIST_ITEM,
    BUTTON,
    TABLE,
    SELECT,
  };

  // NOTE: inverted order from whatwg.org because it is easier
  class OpenElements {
  public:
    typedef std::list<Node*>::iterator It;
    typedef std::list<Node*>::const_iterator CIt;

    void push(Node& element) {
      _elements.push_front(&element);
    }

    void insert(It where, Node& element) {
      _elements.insert(where, &element);
    }

    Node& bottom() const {
      return *_elements.back();
    }

    Node* secondBottom() const {
      if (_elements.size() >= 2) {
        return *(++_elements.rbegin());
      }
      return nullptr;
    }

    Node& top() const {
      return *_elements.front();
    }

    Node* secondTop() const {
      if (_elements.size() >= 2) {
        return *(++_elements.begin());
      }
      return nullptr;
    }

    CIt cbegin() const {
      return _elements.cbegin();
    }

    CIt cend() const {
      return _elements.cend();
    }

    It begin() {
      return _elements.begin();
    }

    It end() {
      return _elements.end();
    }

    It find(const Node& element) {
      for (auto openElement = _elements.begin();
          openElement != _elements.end(); ++openElement) {
        if (*openElement == &element) {
          return openElement;
        }
      }
      return _elements.end();
    }

    bool contains(TagKinds kind) const {
      for (auto openElement = _elements.begin();
          openElement != _elements.end(); ++openElement) {
        if ((*openElement)->name() == kind) {
          return true;
        }
      }
      return false;
    }

    bool contains(Node& element) const {
      for (auto openElement = _elements.begin();
          openElement != _elements.end(); ++openElement) {
        if (*openElement == &element) {
          return true;
        }
      }
      return false;
    }

    void pop() {
      _elements.pop_front();
    }

    void popImplied(const TagName& notClosed = TagName()) {
      while (!_elements.empty()) {
        if (top().name() == notClosed) {
          return;
        }

        switch (top().name().kind()) {
          case TagKinds::DD:
          case TagKinds::DT:
          case TagKinds::LI:
          case TagKinds::OPTGROUP:
          case TagKinds::OPTION:
          case TagKinds::RB:
          case TagKinds::RP:
          case TagKinds::RT:
          case TagKinds::RTC:
          case TagKinds::P:
            break;

          default:
            return;
        }
        _elements.pop_front();
      }
    }

    void popImpliedThoroughly() {
      while (!_elements.empty()) {
        switch (top().name().kind()) {
          case TagKinds::CAPTION:
          case TagKinds::COLGROUP:
          case TagKinds::DD:
          case TagKinds::DT:
          case TagKinds::LI:
          case TagKinds::OPTGROUP:
          case TagKinds::OPTION:
          case TagKinds::P:
          case TagKinds::RB:
          case TagKinds::RP:
          case TagKinds::RT:
          case TagKinds::RTC:
          case TagKinds::TBODY:
          case TagKinds::TD:
          case TagKinds::TFOOT:
          case TagKinds::TH:
          case TagKinds::THEAD:
          case TagKinds::TR:
            break;

          default:
            return;
        }
        _elements.pop_front();
      }
    }

    void popUntil(const TagName& tagName = TagName()) {
      while (!_elements.empty()) {
        if (top().name() == tagName) {
          _elements.pop_front();
          return;
        }
        _elements.pop_front();
      }
    }

    void popUntil(const std::vector<TagKinds>& kinds,
        bool included = true) {
      while (!_elements.empty()) {
        for (auto kind : kinds) {
          if (top().name() == kind) {
            if (included) {
              _elements.pop_front();
            }
            return;
          }
        }
        _elements.pop_front();
      }
    }

    void popUntil(It it) {
      _elements.erase(_elements.begin(), std::next(it));
    }

    void remove(Node& element) {
      for (auto openElement = _elements.begin();
          openElement != _elements.end(); ++openElement) {
        if (*openElement == &element) {
          _elements.erase(openElement);
          return;
        }
      }
    }

    It erase(It it) {
      return _elements.erase(it);
    }

    bool hasElementsInScope(ScopeKinds scope,
        const std::vector<TagKinds>& kinds) {
      for (auto element = _elements.begin(); element != _elements.end();
          ++element) {
        auto currentKind = (*element)->name().kind();
        for (auto kind : kinds) {
          if (currentKind == kind) {
            return true;
          }
        }
        if (_isScopedTag(scope, currentKind)) {
          return false;
        }
      }
      return false;
    }

    bool hasElementInScope(ScopeKinds scope, TagKinds kind) {
      for (auto element = _elements.begin(); element != _elements.end();
          ++element) {
        auto currentKind = (*element)->name().kind();
        if (currentKind == kind) {
          return true;
        }
        if (_isScopedTag(scope, currentKind)) {
          return false;
        }
      }
      return false;
    }

    bool hasElementInScope(ScopeKinds scope, Node& element) {
      for (auto openElement = _elements.begin();
          openElement != _elements.end(); ++openElement) {
        if (*openElement == &element) {
          return true;
        }
        if (_isScopedTag(scope, (*openElement)->name().kind())) {
          return false;
        }
      }
      return false;
    }

    bool empty() const {
      return _elements.empty();
    }

  private:
    bool _isScopedTagStandard(TagKinds tagKind) {
      switch (tagKind) {
        case TagKinds::APPLET:
        case TagKinds::CAPTION:
        case TagKinds::HTML:
        case TagKinds::TABLE:
        case TagKinds::TD:
        case TagKinds::TH:
        case TagKinds::MARQUEE:
        case TagKinds::OBJECT:
        case TagKinds::TEMPLATE:
        case TagKinds::MATHML_MI:
        case TagKinds::MATHML_MO:
        case TagKinds::MATHML_MN:
        case TagKinds::MATHML_MS:
        case TagKinds::MATHML_MTEXT:
        case TagKinds::MATHML_ANNOTATION_XML:
        case TagKinds::SVG_FOREIGNOBJECT:
        case TagKinds::SVG_DESC:
        case TagKinds::SVG_TITLE:
          return true;

        default:
          return false;
      }
    }

    bool _isScopedTag(ScopeKinds scope, TagKinds tagKind) {
      switch (scope) {
        case ScopeKinds::STANDARD:
          return _isScopedTagStandard(tagKind);

        case ScopeKinds::LIST_ITEM:
          if (_isScopedTagStandard(tagKind) || tagKind == TagKinds::OL ||
              tagKind == TagKinds::UL) {
            return true;
          }
          break;

        case ScopeKinds::BUTTON:
          if (_isScopedTagStandard(tagKind) || tagKind == TagKinds::BUTTON) {
            return true;
          }
          break;

        case ScopeKinds::TABLE:
          if (tagKind == TagKinds::HTML || tagKind == TagKinds::TABLE ||
              tagKind == TagKinds::TEMPLATE) {
            return true;
          }
          break;

        case ScopeKinds::SELECT:
          if (tagKind == TagKinds::OPTGROUP || tagKind == TagKinds::OPTION) {
            return true;
          }
          break;
      }

      return false;
    }

    std::list<Node*> _elements;
  };

}
}

#endif
