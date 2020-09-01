#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_HEAD
#define ECLAIR_HTML_TREE_BUILDER_STEPS_AFTER_HEAD

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace eclair_html {
namespace html_parser {
namespace steps {

struct AfterHead : public BaseStep {
  explicit AfterHead(RootBuilder& rootBuilder) : BaseStep(rootBuilder) {}

  virtual ~AfterHead() {}

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

      case TagKinds::BODY:
        _root.nodeInserter.insertElement(token);
        _root.state.framesetOk = false;
        _root.insertionMode.set(InsertionModes::IN_BODY);
        return;

      case TagKinds::FRAMESET:
        _root.nodeInserter.insertElement(token);
        _root.insertionMode.set(InsertionModes::IN_FRAMESET);
        return;

      case TagKinds::BASE:
      case TagKinds::BASEFONT:
      case TagKinds::BGSOUND:
      case TagKinds::LINK:
      case TagKinds::META:
      case TagKinds::NOFRAMES:
      case TagKinds::SCRIPT:
      case TagKinds::STYLE:
      case TagKinds::TEMPLATE:
      case TagKinds::TITLE:
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        _root.openElements.push(*_root.state.head);
        _root.reprocess(InsertionModes::IN_HEAD, token);
        _root.openElements.remove(*_root.state.head);
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
      case TagKinds::TEMPLATE:
        _root.reprocess(InsertionModes::IN_HEAD, token);
        return;

      case TagKinds::BODY:
      case TagKinds::HTML:
      case TagKinds::BR:
        break;

      default:
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return;
      }
    }

    _root.nodeInserter.insertElement(TagKinds::BODY);
    _root.insertionMode.set(InsertionModes::IN_BODY);
    _root.reprocess(token);
  }
};

} // namespace steps
} // namespace html_parser
} // namespace eclair_html

#endif
