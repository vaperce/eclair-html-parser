#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_FRAMESET
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_FRAMESET

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct InFrameset : public BaseStep {
    explicit InFrameset(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InFrameset() {
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

          case TagKinds::FRAMESET:
            _root.nodeInserter.insertElement(token);
            return;

          case TagKinds::FRAME:
            _selfClosing(token);
            return;

          case TagKinds::NOFRAMES:
            _root.reprocess(InsertionModes::IN_HEAD, token);
            return;

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG &&
          token.name == TagKinds::FRAMESET) {
        if (_root.openElements.top().name() == TagKinds::HTML) {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          return;
        }
        _root.openElements.pop();
        // FIXME: fragment
        if (_root.openElements.top().name() != TagKinds::FRAMESET) {
          _root.insertionMode.set(InsertionModes::AFTER_FRAMESET);
        }
        return;
      }
      if (token.kind == TokenKinds::END_OF_FILE) {
        if (_root.openElements.top().name() != TagKinds::HTML) {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        }
        return;
      }

      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
    }
  };

}
}
}

#endif
