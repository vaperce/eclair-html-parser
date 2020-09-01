#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_FRAMESET
#define ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_FRAMESET

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct AfterFrameset : public BaseStep {
    explicit AfterFrameset(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~AfterFrameset() {
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

          case TagKinds::NOFRAMES:
            _root.reprocess(InsertionModes::IN_HEAD, token);
            return;

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG &&
          token.name == TagKinds::HTML) {
        _root.insertionMode.set(InsertionModes::AFTER_AFTER_FRAMESET);
      }
      if (token.kind == TokenKinds::END_OF_FILE) {
        return;
      }

      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
    }
  };

}
}
}

#endif
