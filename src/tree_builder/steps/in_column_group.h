#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_COLUMN_GROUP
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_COLUMN_GROUP

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct InColumnGroup : public BaseStep {
    explicit InColumnGroup(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InColumnGroup() {
    }

    virtual void process(Token& token) {
      if (_isSpace(token)) {
        _root.nodeInserter.insertCharacter(token);
        return;
      }
      if (_checkCommentDocId(token)) {
        return;
      }
      if (token.kind == TokenKinds::START_TAG) {
        switch (token.name.kind()) {
          case TagKinds::HTML:
            _root.reprocess(InsertionModes::IN_BODY, token);
            return;

          case TagKinds::COL:
            _selfClosing(token);
            return;

          case TagKinds::TEMPLATE:
            _root.reprocess(InsertionModes::IN_HEAD, token);
            return;

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        switch (token.name.kind()) {
          case TagKinds::COLGROUP:
            if (_root.openElements.top().name() != TagKinds::COLGROUP) {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
              return;
            }
            _root.openElements.pop();
            _root.insertionMode.set(InsertionModes::IN_TABLE);
            return;

          case TagKinds::COL:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            return;

          case TagKinds::TEMPLATE:
            _root.reprocess(InsertionModes::IN_HEAD, token);
            return;

          default:
            break;
        }
      }

      if (_root.openElements.top().name() != TagKinds::COLGROUP) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return;
      }
      _root.openElements.pop();
      _root.insertionMode.set(InsertionModes::IN_TABLE);
      _root.reprocess(token);
    }
  };

}
}
}

#endif
