#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_HEAD_NOCRIPT
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_HEAD_NOCRIPT

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct InHeadNoscript : public BaseStep {
    explicit InHeadNoscript(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InHeadNoscript() {
    }

    virtual void process(Token& token) {
      if (token.kind == TokenKinds::DOC_TYPE) {
        return;
      }
      if (_isSpace(token) || token.kind == TokenKinds::COMMENT) {
        _root.reprocess(InsertionModes::IN_HEAD, token);
        return;
      }
      if (token.kind == TokenKinds::START_TAG) {
        switch (token.name.kind()) {
          case TagKinds::HTML:
            _root.reprocess(InsertionModes::IN_BODY, token);
            return;

          case TagKinds::BASEFONT:
          case TagKinds::BGSOUND:
          case TagKinds::LINK:
          case TagKinds::META:
          case TagKinds::NOFRAMES:
          case TagKinds::STYLE:
            _root.reprocess(InsertionModes::IN_HEAD, token);
            return;

          case TagKinds::HEAD:
          case TagKinds::NOSCRIPT:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            return;

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        switch (token.name.kind()) {
          case TagKinds::NOSCRIPT:
            _root.openElements.pop();
            _root.insertionMode.set(InsertionModes::IN_HEAD);
            return;

          case TagKinds::BR:
            break;

          default:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            return;
        }
      }

      if (token.kind == TokenKinds::CHARACTER) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TEXT, token);
      } else {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      }
      _root.openElements.pop();
      _root.insertionMode.set(InsertionModes::IN_HEAD);
      _root.reprocess(token);
    }
  };

}
}
}

#endif
