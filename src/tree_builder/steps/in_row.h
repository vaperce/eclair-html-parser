#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_ROW
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_ROW

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace eclair_html {
namespace html_parser {
namespace steps {

class InRow : public BaseStep {
public:
  explicit InRow(RootBuilder& rootBuilder) : BaseStep(rootBuilder) {}

  virtual ~InRow() {}

  virtual void process(Token& token) {
    if (token.kind == TokenKinds::START_TAG) {
      switch (token.name.kind()) {
      case TagKinds::TH:
      case TagKinds::TD:
        _cleanOpenElements();
        _root.nodeInserter.insertElement(token);
        _root.insertionMode.set(InsertionModes::IN_CELL);
        _root.activeFormattingElements.addMarker();
        return;

      case TagKinds::CAPTION:
      case TagKinds::COL:
      case TagKinds::COLGROUP:
      case TagKinds::TBODY:
      case TagKinds::TFOOT:
      case TagKinds::THEAD:
      case TagKinds::TR:
        if (_handleCommon(token)) {
          _root.reprocess(token);
        }
        return;

      default:
        break;
      }
    }
    if (token.kind == TokenKinds::END_TAG) {
      switch (token.name.kind()) {
      case TagKinds::TR:
        _handleCommon(token);
        return;

      case TagKinds::TABLE:
        if (_handleCommon(token)) {
          _root.reprocess(token);
        }
        return;

      case TagKinds::TBODY:
      case TagKinds::TFOOT:
      case TagKinds::THEAD:
        if (!_root.openElements.hasElementInScope(ScopeKinds::TABLE,
                                                  token.name.kind())) {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          return;
        }
        if (_handleCommon(token)) {
          _root.reprocess(token);
        }
        return;

      case TagKinds::BODY:
      case TagKinds::CAPTION:
      case TagKinds::COL:
      case TagKinds::COLGROUP:
      case TagKinds::HTML:
      case TagKinds::TD:
      case TagKinds::TH:
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return;

      default:
        break;
      }
    }

    _root.reprocess(InsertionModes::IN_TABLE, token);
  }

private:
  bool _handleCommon(Token& token) {
    if (!_root.openElements.hasElementInScope(ScopeKinds::TABLE,
                                              TagKinds::TR)) {
      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      return false;
    }

    _cleanOpenElements();
    _root.openElements.pop();
    _root.insertionMode.set(InsertionModes::IN_TABLE_BODY);
    return true;
  }

  void _cleanOpenElements() {
    _root.openElements.popUntil(
        {TagKinds::TR, TagKinds::TEMPLATE, TagKinds::HTML}, false);
  }
};

} // namespace steps
} // namespace html_parser
} // namespace eclair_html

#endif
