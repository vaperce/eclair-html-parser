#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_TABLE
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_TABLE

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace eclair_html {
namespace html_parser {
namespace steps {

class InTable : public BaseStep {
public:
  explicit InTable(RootBuilder& rootBuilder) : BaseStep(rootBuilder) {}

  virtual ~InTable() {}

  virtual void process(Token& token) {
    if (_checkCommentDocId(token)) {
      return;
    }
    if (token.kind == TokenKinds::CHARACTER) {
      auto currentKind = _root.openElements.top().name().kind();
      if (currentKind == TagKinds::TABLE || currentKind == TagKinds::TBODY ||
          currentKind == TagKinds::TFOOT || currentKind == TagKinds::THEAD ||
          currentKind == TagKinds::TR) {
        _root.state.pendingTableTextTokens.clear();
        _root.insertionMode.saveMode();
        _root.insertionMode.set(InsertionModes::IN_TABLE_TEXT);
        _root.reprocess(token);
        return;
      }
    }
    if (token.kind == TokenKinds::START_TAG) {
      switch (token.name.kind()) {
      case TagKinds::CAPTION:
        _cleanOpenElements();
        _root.activeFormattingElements.addMarker();
        _root.nodeInserter.insertElement(token);
        _root.insertionMode.set(InsertionModes::IN_CAPTION);
        return;

      case TagKinds::COLGROUP:
        _cleanOpenElements();
        _root.nodeInserter.insertElement(token);
        _root.insertionMode.set(InsertionModes::IN_COLUMN_GROUP);
        return;

      case TagKinds::COL:
        _cleanOpenElements();
        _root.nodeInserter.insertElement(TagKinds::COLGROUP);
        _root.insertionMode.set(InsertionModes::IN_COLUMN_GROUP);
        _root.reprocess(token);
        return;

      case TagKinds::TBODY:
      case TagKinds::TFOOT:
      case TagKinds::THEAD:
        _cleanOpenElements();
        _root.nodeInserter.insertElement(token);
        _root.insertionMode.set(InsertionModes::IN_TABLE_BODY);
        return;

      case TagKinds::TD:
      case TagKinds::TH:
      case TagKinds::TR:
        _cleanOpenElements();
        _root.nodeInserter.insertElement(TagKinds::TBODY);
        _root.insertionMode.set(InsertionModes::IN_TABLE_BODY);
        _root.reprocess(token);
        return;

      case TagKinds::TABLE:
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        if (!_root.openElements.hasElementInScope(ScopeKinds::TABLE,
                                                  TagKinds::TABLE)) {
          return;
        }
        _root.openElements.popUntil(TagName(TagKinds::TABLE));
        _root.insertionMode.reset();
        _root.reprocess(token);
        return;

      case TagKinds::STYLE:
      case TagKinds::SCRIPT:
      case TagKinds::TEMPLATE:
        _root.reprocess(InsertionModes::IN_HEAD, token);
        return;

      case TagKinds::INPUT:
        if (!_hasAttributeTypeHidden(token)) {
          break;
        }
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        _selfClosing(token);
        return;

      case TagKinds::FORM:
        if (_root.openElements.contains(TagKinds::TEMPLATE) ||
            _root.state.form) {
          return;
        }
        _root.nodeInserter.insertElement(token);
        _root.state.form = &_root.openElements.top();
        _root.openElements.pop();
        return;

      default:
        break;
      }
    }
    if (token.kind == TokenKinds::END_TAG) {
      switch (token.name.kind()) {
      case TagKinds::TABLE:
        if (!_root.openElements.hasElementInScope(ScopeKinds::TABLE,
                                                  TagKinds::TABLE)) {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          return;
        }
        _root.openElements.popUntil(TagName(TagKinds::TABLE));
        _root.insertionMode.reset();
        return;

      case TagKinds::BODY:
      case TagKinds::CAPTION:
      case TagKinds::COL:
      case TagKinds::COLGROUP:
      case TagKinds::HTML:
      case TagKinds::TBODY:
      case TagKinds::TD:
      case TagKinds::TFOOT:
      case TagKinds::TH:
      case TagKinds::THEAD:
      case TagKinds::TR:
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return;

      case TagKinds::TEMPLATE:
        _root.reprocess(InsertionModes::IN_HEAD, token);
        return;

      default:
        break;
      }
    }
    if (token.kind == TokenKinds::END_OF_FILE) {
      _root.reprocess(InsertionModes::IN_BODY, token);
      return;
    }

    if (token.kind == TokenKinds::CHARACTER) {
      _root.errorManager.add(ErrorKinds::UNEXPECTED_TEXT, token);
    } else {
      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
    }
    _root.nodeInserter.enableFosterParenting();
    _root.reprocess(InsertionModes::IN_BODY, token);
    _root.nodeInserter.disableFosterParenting();
  }

private:
  void _cleanOpenElements() {
    _root.openElements.popUntil(
        {TagKinds::TABLE, TagKinds::TEMPLATE, TagKinds::HTML}, false);
  }
};

} // namespace steps
} // namespace html_parser
} // namespace eclair_html

#endif
