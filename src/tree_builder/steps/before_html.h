#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_BEFORE_HTML
#define ECLAIR_HTML_TREE_BUILDER_STEPS_BEFORE_HTML

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace eclair_html {
namespace html_parser {
namespace steps {

  struct BeforeHtml : public BaseStep {
    explicit BeforeHtml(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~BeforeHtml() {
    }

    virtual void process(Token& token) {
      if (token.kind == TokenKinds::DOC_TYPE) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return;
      }
      if (_isSpace(token)) {
        return;
      }
      if (token.kind == TokenKinds::COMMENT) {
        _root.nodeInserter.insertCommentInDocument(token);
        return;
      }
      if (token.kind == TokenKinds::START_TAG && token.name == TagKinds::HTML) {
        _root.nodeInserter.insertElementInDocument(token);
        _root.insertionMode.set(InsertionModes::BEFORE_HEAD);
        return;
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

      _root.nodeInserter.insertElementInDocument(TagKinds::HTML);
      _root.insertionMode.set(InsertionModes::BEFORE_HEAD);
      _root.reprocess(token);
    }
  };

}
}
}

#endif
