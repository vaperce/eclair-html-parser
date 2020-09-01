#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_AFTER_BODY
#define ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_AFTER_BODY

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct AfterAfterBody : public BaseStep {
    explicit AfterAfterBody(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~AfterAfterBody() {
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
      if (token.kind == TokenKinds::START_TAG &&
          token.name == TagKinds::HTML) {
        _root.reprocess(InsertionModes::IN_BODY, token);
        return;
      }
      if (token.kind == TokenKinds::END_OF_FILE) {
        return;
      }

      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      _root.insertionMode.set(InsertionModes::IN_BODY);
      _root.reprocess(token);
    }
  };

}
}
}

#endif
