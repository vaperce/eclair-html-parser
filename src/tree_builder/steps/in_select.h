#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_SELECT
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_SELECT

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace eclair_html {
namespace html_parser {
namespace steps {

  struct InSelect : public BaseStep {
    explicit InSelect(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InSelect() {
    }

    virtual void process(Token& token) {
      if (token.kind == TokenKinds::CHARACTER) {
        if (token.data[0] == '\0') {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TEXT, token);
        } else {
          _root.nodeInserter.insertCharacter(token);
        }
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

          case TagKinds::OPTION:
            if (_root.openElements.top().name() == TagKinds::OPTION) {
              _root.openElements.pop();
            }
            _root.nodeInserter.insertElement(token);
            return;

          case TagKinds::OPTGROUP:
            if (_root.openElements.top().name() == TagKinds::OPTION) {
              _root.openElements.pop();
            }
            if (_root.openElements.top().name() == TagKinds::OPTGROUP) {
              _root.openElements.pop();
            }
            _root.nodeInserter.insertElement(token);
            return;

          case TagKinds::SELECT:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            if (!_root.openElements.hasElementInScope(ScopeKinds::SELECT,
                  TagKinds::SELECT)) {
              return;
            }
            _root.openElements.popUntil(TagName(TagKinds::SELECT));
            _root.insertionMode.reset();
            return;

          case TagKinds::INPUT:
          case TagKinds::KEYGEN:
          case TagKinds::TEXTAREA:
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            if (!_root.openElements.hasElementInScope(ScopeKinds::SELECT,
                  TagKinds::SELECT)) {
              return;
            }
            _root.openElements.popUntil(TagName(TagKinds::SELECT));
            _root.insertionMode.reset();
            _root.reprocess(token);
            return;

          case TagKinds::SCRIPT:
          case TagKinds::TEMPLATE:
            _root.reprocess(InsertionModes::IN_HEAD, token);

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        switch (token.name.kind()) {
          case TagKinds::OPTGROUP:
            if (_root.openElements.top().name() == TagKinds::OPTION &&
                _root.openElements.secondTop() &&
                _root.openElements.secondTop()->name() == TagKinds::OPTGROUP) {
              _root.openElements.pop();
            } else if (_root.openElements.top().name() == TagKinds::OPTGROUP) {
              _root.openElements.pop();
            } else {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            }
            return;

          case TagKinds::OPTION:
            if (_root.openElements.top().name() == TagKinds::OPTION) {
              _root.openElements.pop();
            } else {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            }
            return;

          case TagKinds::SELECT:
            if (!_root.openElements.hasElementInScope(ScopeKinds::SELECT,
                  TagKinds::SELECT)) {
              return;
            }
            _root.openElements.popUntil(TagName(TagKinds::SELECT));
            _root.insertionMode.reset();
            return;

          case TagKinds::TEMPLATE:
            _root.reprocess(InsertionModes::IN_HEAD, token);

          default:
            break;
        }
      }
      if (token.kind == TokenKinds::END_OF_FILE) {
        _root.reprocess(InsertionModes::IN_BODY, token);
        return;
      }

      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
    }
  };

}
}
}

#endif
