#ifndef ECLAIR_HTML_TREE_BUILDER_INSERTION_MODE
#define ECLAIR_HTML_TREE_BUILDER_INSERTION_MODE

#include <vector>

#include "tree_builder/insertion_modes.h"
#include "tree_builder/open_elements.h"
#include "tree_builder/state.h"

namespace eclair_html {
namespace html_parser {

class InsertionMode {
public:
  InsertionMode(const OpenElements& openElements, const State& state)
      : _openElements(openElements), _state(state),
        _mode(InsertionModes::INITIAL), _originalMode(InsertionModes::INITIAL) {
  }

  InsertionModes get() const { return _mode; }

  void set(InsertionModes mode) {
    if (mode == InsertionModes::TEXT || mode == InsertionModes::IN_TABLE_TEXT) {
      _originalMode = _mode;
    }
    _set(mode);
  }

  void saveMode() { _originalMode = _mode; }

  void reset() {
    for (auto element = _openElements.cbegin(); element != _openElements.cend();
         ++element) {
      bool last = element == _openElements.cend();

      switch ((*element)->name().kind()) {
      case TagKinds::SELECT:
        _adjustInsertionModeSelect(element);
        return;

      case TagKinds::TD:
      case TagKinds::TH:
        if (!last) {
          _set(InsertionModes::IN_CELL);
          return;
        }
        break;

      case TagKinds::TR:
        _set(InsertionModes::IN_ROW);
        return;

      case TagKinds::TBODY:
      case TagKinds::THEAD:
      case TagKinds::TFOOT:
        _set(InsertionModes::IN_TABLE_BODY);
        return;

      case TagKinds::CAPTION:
        _set(InsertionModes::IN_CAPTION);
        return;

      case TagKinds::COLGROUP:
        _set(InsertionModes::IN_COLUMN_GROUP);
        return;

      case TagKinds::TABLE:
        _set(InsertionModes::IN_TABLE);
        return;

      case TagKinds::TEMPLATE:
        _set(_templateModes.back());
        return;

      case TagKinds::HEAD:
        _set(InsertionModes::IN_HEAD);
        return;

      case TagKinds::BODY:
        _set(InsertionModes::IN_BODY);
        return;

      case TagKinds::FRAMESET:
        _set(InsertionModes::IN_FRAMESET);
        return;

      case TagKinds::HTML:
        if (_state.head) {
          _set(InsertionModes::BEFORE_HEAD);
        } else {
          _set(InsertionModes::AFTER_HEAD);
        }
        return;

      default:
        break;
      }
    }
    _set(InsertionModes::IN_BODY);
  }

  void restoreOriginal() { _set(_originalMode); }

  InsertionModes getTemplate() const { return _templateModes.back(); }

  void pushTemplate(InsertionModes mode) { _templateModes.push_back(mode); }

  InsertionModes popTemplate() {
    InsertionModes res = _templateModes.back();
    _templateModes.pop_back();
    return res;
  }

  bool hasTemplates() { return !_templateModes.empty(); }

private:
  inline void _set(InsertionModes mode) { _mode = mode; }

  void _adjustInsertionModeSelect(OpenElements::CIt element) {
    for (; element != _openElements.cend(); ++element) {
      if ((*element)->name() == TagKinds::TEMPLATE) {
        break;
      }
      if ((*element)->name() == TagKinds::TABLE) {
        _set(InsertionModes::IN_SELECT_IN_TABLE);
        return;
      }
    }

    _set(InsertionModes::IN_SELECT);
  }

  const OpenElements& _openElements;
  const State& _state;

  InsertionModes _mode;
  InsertionModes _originalMode;
  std::vector<InsertionModes> _templateModes;
};

} // namespace html_parser
} // namespace eclair_html

#endif
