#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_HEAD
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_HEAD

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct InHead : public BaseStep {
    explicit InHead(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InHead() {
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

          case TagKinds::BASE:
          case TagKinds::BASEFONT:
          case TagKinds::BGSOUND:
          case TagKinds::LINK:
          case TagKinds::META:
            _selfClosing(token);
            return;

          case TagKinds::TITLE:
            _toRCDATA(token);
            return;

          case TagKinds::NOFRAMES:
          case TagKinds::STYLE:
            _toGenericRaw(token);
            return;

          case TagKinds::NOSCRIPT:
            if (_root.state.scriptEnabled) {
              _toGenericRaw(token);
            } else {
              _root.nodeInserter.insertElement(token);
              _root.insertionMode.set(InsertionModes::IN_HEAD_NOSCRIPT);
            }
            return;

          case TagKinds::SCRIPT:
            _root.nodeInserter.insertElement(token);
            _root.state.readingMode = TextReadingModes::SCRIPT_DATA;
            _root.state.expectedEndTag = token.name.value();
            _root.insertionMode.set(InsertionModes::TEXT);
            return;

          case TagKinds::TEMPLATE:
            _root.nodeInserter.insertElement(token);
            _root.activeFormattingElements.addMarker();
            _root.state.framesetOk = false;
            _root.insertionMode.set(InsertionModes::IN_TEMPLATE);
            _root.insertionMode.pushTemplate(InsertionModes::IN_TEMPLATE);
            return;

          case TagKinds::HEAD:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            return;

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        switch (token.name.kind()) {
          case TagKinds::HEAD:
            _root.openElements.pop();
            _root.insertionMode.set(InsertionModes::AFTER_HEAD);
            return;

          case TagKinds::BODY:
          case TagKinds::HTML:
          case TagKinds::BR:
            break;

          case TagKinds::TEMPLATE:
            if (_root.openElements.contains(TagKinds::TEMPLATE)) {
              _closeGenericThoroughly(token);
              _root.activeFormattingElements.clearLast();
              _root.insertionMode.popTemplate();
              _root.insertionMode.reset();
            } else {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            }
            return;

          default:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            return;
        }
      }

      _root.openElements.pop();
      _root.insertionMode.set(InsertionModes::AFTER_HEAD);
      _root.reprocess(token);
    }
  };

}
}
}

#endif
