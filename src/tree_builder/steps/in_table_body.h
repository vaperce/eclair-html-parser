#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_TABLE_BODY
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_TABLE_BODY

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  class InTableBody : public BaseStep {
  public:
    explicit InTableBody(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InTableBody() {
    }

    virtual void process(Token& token) {
      if (token.kind == TokenKinds::START_TAG) {
        switch (token.name.kind()) {
          case TagKinds::TR:
            _cleanOpenElements();
            _root.nodeInserter.insertElement(token);
            _root.insertionMode.set(InsertionModes::IN_ROW);
            return;

          case TagKinds::TH:
          case TagKinds::TD:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            _cleanOpenElements();
            _root.nodeInserter.insertElement(TagKinds::TR);
            _root.insertionMode.set(InsertionModes::IN_ROW);
            _root.reprocess(token);
            return;

          case TagKinds::CAPTION:
          case TagKinds::COL:
          case TagKinds::COLGROUP:
          case TagKinds::TBODY:
          case TagKinds::TFOOT:
          case TagKinds::THEAD:
            _handleCommon(token);
            return;

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        switch (token.name.kind()) {
          case TagKinds::TBODY:
          case TagKinds::TFOOT:
          case TagKinds::THEAD:
            if (!_root.openElements.hasElementInScope(ScopeKinds::TABLE,
                  token.name.kind())) {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
              return;
            }
            _cleanOpenElements();
            _root.openElements.pop();
            _root.insertionMode.set(InsertionModes::IN_TABLE);
            return;

          case TagKinds::TABLE:
            _handleCommon(token);
            return;

          case TagKinds::BODY:
          case TagKinds::CAPTION:
          case TagKinds::COL:
          case TagKinds::COLGROUP:
          case TagKinds::HTML:
          case TagKinds::TD:
          case TagKinds::TH:
          case TagKinds::TR:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            return;

          default:
            break;
        }
      }

      _root.reprocess(InsertionModes::IN_TABLE, token);
    }

  private:
    void _handleCommon(Token& token) {
      if (!_root.openElements.hasElementsInScope(ScopeKinds::TABLE,
            {TagKinds::TBODY, TagKinds::THEAD, TagKinds::TFOOT})) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return;
      }

      _cleanOpenElements();
      _root.openElements.pop();
      _root.insertionMode.set(InsertionModes::IN_TABLE);
      _root.reprocess(token);
    }

    void _cleanOpenElements() {
      _root.openElements.popUntil({TagKinds::TBODY, TagKinds::TFOOT,
          TagKinds::THEAD, TagKinds::TEMPLATE, TagKinds::HTML}, false);
    }
  };

}
}
}

#endif
