#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_BODY
#define ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_BODY

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct AfterBody : public BaseStep {
    explicit AfterBody(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~AfterBody() {
    }

    virtual void process(Token& token) {
      if (_isSpace(token)) {
        _root.reprocess(InsertionModes::IN_BODY, token);
        return;
      }
      if (token.kind == TokenKinds::COMMENT) {
        _root.nodeInserter.insertComment(token, &_root.openElements.bottom());
        return;
      }
      if (token.kind == TokenKinds::DOC_TYPE) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return;
      }
      if (token.kind == TokenKinds::START_TAG &&
          token.name == TagKinds::HTML) {
        _root.reprocess(InsertionModes::IN_BODY, token);
        return;
      }
      if (token.kind == TokenKinds::END_TAG &&
          token.name == TagKinds::HTML) {
        // FIXME: fragment
        _root.insertionMode.set(InsertionModes::AFTER_AFTER_BODY);
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
