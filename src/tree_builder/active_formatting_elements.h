#ifndef ECLAIR_HTML_TREE_BUILDER_ACTIVE_FORMATTING_ELEMENTS
#define ECLAIR_HTML_TREE_BUILDER_ACTIVE_FORMATTING_ELEMENTS

#include <list>

#include "common/tokens.h"
#include "common/tools.h"
#include "eclair-html/node.h"
#include "eclair-html/tag_names.h"
#include "tree_builder/node_inserter.h"
#include "tree_builder/open_elements.h"

namespace eclair_html {
namespace html_parser {

struct _FormattingElementItem {
  explicit _FormattingElementItem() : element(nullptr) {}

  explicit _FormattingElementItem(Node& element) : element(&element) {}

  bool marker() const { return !element; }

  Node* element;
};

class ActiveFormattingElements {
public:
  typedef std::list<_FormattingElementItem>::iterator It;

  ActiveFormattingElements(OpenElements& openElements, NodeInserter& inserter)
      : _openElements(openElements), _inserter(inserter) {}

  void addMarker() { _list.push_front(_FormattingElementItem()); }

  void addElement(Node& element) {
    int count = 0;

    for (auto item = _list.begin(); item != _list.end(); ++item) {
      if (item->marker()) {
        continue;
      }

      if (item->element->name() == element.name() &&
          item->element->attributes == element.attributes) {
        if (++count == MAX_KEPT) {
          _list.erase(item);
          break;
        }
      }
    }

    _list.push_front(_FormattingElementItem(element));
  }

  void reconstruct() {
    auto item = _list.begin();
    for (; item != _list.end(); ++item) {
      if (item->marker() || _openElements.contains(*item->element)) {
        break;
      }
    }

    if (item == _list.begin()) {
      return;
    }

    for (--item; item != _list.begin(); --item) {
      auto element = item->element->copy();
      item->element = element.get();
      _inserter.insertElement(std::move(element));
    }

    auto element = item->element->copy();
    item->element = element.get();
    _inserter.insertElement(std::move(element));
  }

  void clearLast() {
    while (!_list.empty()) {
      if (_list.front().marker()) {
        _list.pop_front();
        break;
      }
      _list.pop_front();
    }
  }

  bool contains(Node& element) {
    for (auto item = _list.begin(); item != _list.end(); ++item) {
      if (!item->marker() || item->element == &element) {
        return true;
      }
    }
    return false;
  }

  Node* findUntilMarker(TagKinds kind) {
    for (auto item = _list.begin(); item != _list.end(); ++item) {
      if (item->marker()) {
        return nullptr;
      }
      if (item->element->name() == kind) {
        return item->element;
      }
    }
    return nullptr;
  }

  void remove(Node& element) {
    for (auto it = _list.begin(); it != _list.end(); ++it) {
      if (it->element == &element) {
        _list.erase(it);
        return;
      }
    }
  }

  It begin() { return _list.begin(); }

  It end() { return _list.end(); }

  void insert(It where, Node& element) {
    _list.insert(where, _FormattingElementItem(element));
  }

  It find(Node& element) {
    for (auto item = _list.begin(); item != _list.end(); ++item) {
      if (item->element == &element) {
        return item;
      }
    }
    return _list.end();
  }

  void erase(It item) { _list.erase(item); }

private:
  OpenElements& _openElements;
  NodeInserter& _inserter;

  const int MAX_KEPT = 3;
  std::list<_FormattingElementItem> _list;
};

} // namespace html_parser
} // namespace eclair_html

#endif
