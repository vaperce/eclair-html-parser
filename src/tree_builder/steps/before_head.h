#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_BEFORE_HEAD
#define ECLAIR_HTML_TREE_BUILDER_STEPS_BEFORE_HEAD

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct BeforeHead : public BaseStep {
    explicit BeforeHead(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~BeforeHead() {
    }

    virtual void process(Token& token) {
      if (_isSpace(token)) {
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

          case TagKinds::HEAD:
            _root.nodeInserter.insertElement(token);
            _root.state.head = &_root.openElements.top();
            _root.insertionMode.set(InsertionModes::IN_HEAD);
            return;

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        switch (token.name.kind()) {
          case TagKinds::HEAD:
          case TagKinds::BODY:
          case TagKinds::HTML:
          case TagKinds::BR:
            break;

          default:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            return;
        }
      }

      _root.nodeInserter.insertElement(TagKinds::HEAD);
      _root.state.head = &_root.openElements.top();
      _root.insertionMode.set(InsertionModes::IN_HEAD);
      _root.reprocess(token);
    }
  };

}
}
}

#endif
