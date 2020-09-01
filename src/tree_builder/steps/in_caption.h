#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_CAPTION
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_CAPTION

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace eclair_html {
namespace html_parser {
namespace steps {

class InCaption : public BaseStep {
public:
  explicit InCaption(RootBuilder& rootBuilder) : BaseStep(rootBuilder) {}

  virtual ~InCaption() {}

  virtual void process(Token& token) {
    if (token.kind == TokenKinds::START_TAG) {
      switch (token.name.kind()) {
      case TagKinds::CAPTION:
      case TagKinds::COL:
      case TagKinds::COLGROUP:
      case TagKinds::TBODY:
      case TagKinds::TD:
      case TagKinds::TFOOT:
      case TagKinds::TH:
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
      case TagKinds::CAPTION:
        _handleCommon(token);
        return;

      case TagKinds::TABLE:
        if (_handleCommon(token)) {
          _root.reprocess(token);
        }
        return;

      case TagKinds::BODY:
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

      default:
        break;
      }
    }

    _root.reprocess(InsertionModes::IN_BODY, token);
  }

private:
  bool _handleCommon(Token& token) {
    if (!_root.openElements.hasElementInScope(ScopeKinds::TABLE,
                                              TagKinds::CAPTION)) {
      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      return false;
    }

    _closeGeneric(TagName(TagKinds::CAPTION), token);
    _root.activeFormattingElements.clearLast();
    _root.insertionMode.set(InsertionModes::IN_TABLE);
    return true;
  }
};

} // namespace steps
} // namespace html_parser
} // namespace eclair_html

#endif
