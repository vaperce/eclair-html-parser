#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_SELECT_IN_TABLE
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_SELECT_IN_TABLE

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct InSelectInTable : public BaseStep {
    explicit InSelectInTable(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InSelectInTable() {
    }

    virtual void process(Token& token) {
      if (token.kind == TokenKinds::START_TAG) {
        switch (token.name.kind()) {
          case TagKinds::CAPTION:
          case TagKinds::TABLE:
          case TagKinds::TBODY:
          case TagKinds::TFOOT:
          case TagKinds::THEAD:
          case TagKinds::TR:
          case TagKinds::TD:
          case TagKinds::TH:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            _root.openElements.popUntil(TagName(TagKinds::SELECT));
            _root.insertionMode.reset();
            _root.reprocess(token);
            return;

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        switch (token.name.kind()) {
          case TagKinds::CAPTION:
          case TagKinds::TABLE:
          case TagKinds::TBODY:
          case TagKinds::TFOOT:
          case TagKinds::THEAD:
          case TagKinds::TR:
          case TagKinds::TD:
          case TagKinds::TH:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            if (!_root.openElements.hasElementInScope(ScopeKinds::TABLE,
                  token.name.kind())) {
              return;
            }
            _root.openElements.popUntil(TagName(TagKinds::SELECT));
            _root.insertionMode.reset();
            _root.reprocess(token);
            return;

          default:
            break;
        }
      }

      _root.reprocess(InsertionModes::IN_SELECT, token);
    }
  };

}
}
}

#endif
