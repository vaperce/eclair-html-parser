#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_CELL
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_CELL

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  class InCell : public BaseStep {
  public:
    explicit InCell(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InCell() {
    }

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
            if (!_root.openElements.hasElementsInScope(ScopeKinds::TABLE,
                  {TagKinds::TD, TagKinds::TH})) {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
              return;
            }
            _closeCell(token);
            _root.reprocess(token);
            return;

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        switch (token.name.kind()) {
          case TagKinds::TD:
          case TagKinds::TH:
            if (!_root.openElements.hasElementInScope(ScopeKinds::TABLE,
                  token.name.kind())) {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
              return;
            }
            _closeGeneric(token);
            _root.activeFormattingElements.clearLast();
            _root.insertionMode.set(InsertionModes::IN_ROW);
            return;

          case TagKinds::BODY:
          case TagKinds::CAPTION:
          case TagKinds::COL:
          case TagKinds::COLGROUP:
          case TagKinds::HTML:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            return;

          case TagKinds::TABLE:
          case TagKinds::TBODY:
          case TagKinds::TFOOT:
          case TagKinds::THEAD:
          case TagKinds::TR:
            if (!_root.openElements.hasElementInScope(ScopeKinds::TABLE,
                  token.name.kind())) {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
              return;
            }
            _closeCell(token);
            _root.reprocess(token);
            return;

          default:
            break;
        }
      }

      _root.reprocess(InsertionModes::IN_BODY, token);
    }

  private:
    void _closeCell(Token& token) {
      _root.openElements.popImplied();
      if (_root.openElements.top().name() != TagKinds::TD &&
          _root.openElements.top().name() != TagKinds::TH) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      }
      _root.openElements.popUntil({TagKinds::TD, TagKinds::TH});
      _root.activeFormattingElements.clearLast();
      _root.insertionMode.set(InsertionModes::IN_ROW);
    }
  };

}
}
}

#endif
