#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_TEMPLATE
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_TEMPLATE

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  class InTemplate : public BaseStep {
  public:
    explicit InTemplate(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InTemplate() {
    }

    virtual void process(Token& token) {
      if (token.kind == TokenKinds::CHARACTER ||
          token.kind == TokenKinds::COMMENT ||
          token.kind == TokenKinds::DOC_TYPE) {
        _root.reprocess(InsertionModes::IN_BODY, token);
        return;
      }
      if (token.kind == TokenKinds::START_TAG) {
        switch (token.name.kind()) {
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
            _root.reprocess(InsertionModes::IN_HEAD, token);
            return;

          case TagKinds::CAPTION:
          case TagKinds::COLGROUP:
          case TagKinds::TBODY:
          case TagKinds::TFOOT:
          case TagKinds::THEAD:
            _switchTemplateMode(token, InsertionModes::IN_TABLE);
            return;

          case TagKinds::COL:
            _switchTemplateMode(token, InsertionModes::IN_COLUMN_GROUP);
            return;

          case TagKinds::TR:
            _switchTemplateMode(token, InsertionModes::IN_TABLE_BODY);
            return;

          case TagKinds::TD:
          case TagKinds::TH:
            _switchTemplateMode(token, InsertionModes::IN_ROW);
            return;

          default:
            _switchTemplateMode(token, InsertionModes::IN_BODY);
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        switch (token.name.kind()) {
          case TagKinds::TEMPLATE:
            _root.reprocess(InsertionModes::IN_HEAD, token);
            return;

          default:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            return;
        }
      }
      if (token.kind == TokenKinds::END_OF_FILE) {
        if (!_root.openElements.contains(TagKinds::TEMPLATE)) {
          return;
        }

        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        _root.openElements.popUntil(TagName(TagKinds::TEMPLATE));
        _root.activeFormattingElements.clearLast();
        _root.insertionMode.popTemplate();
        _root.insertionMode.reset();
        _root.reprocess(token);
      }
    }

  private:
    void _switchTemplateMode(Token& token, InsertionModes mode) {
      _root.insertionMode.popTemplate();
      _root.insertionMode.pushTemplate(mode);
      _root.insertionMode.set(mode);
      _root.reprocess(token);
    }
  };

}
}
}

#endif
