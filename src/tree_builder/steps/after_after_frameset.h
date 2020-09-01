#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_AFTER_FRAMESET
#define ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_AFTER_FRAMESET

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct AfterAfterFrameset : public BaseStep {
    explicit AfterAfterFrameset(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~AfterAfterFrameset() {
    }

    virtual void process(Token& token) {
      if (token.kind == TokenKinds::COMMENT) {
        _root.nodeInserter.insertCommentInDocument(token);
        return;
      }
      if (token.kind == TokenKinds::DOC_TYPE || _isSpace(token)) {
        _root.reprocess(InsertionModes::IN_BODY, token);
        return;
      }
      if (token.kind == TokenKinds::START_TAG) {
        if (token.name == TagKinds::HTML) {
          _root.reprocess(InsertionModes::IN_BODY, token);
          return;
        }
        if (token.name == TagKinds::NOFRAMES) {
          _root.reprocess(InsertionModes::IN_HEAD, token);
          return;
        }
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
