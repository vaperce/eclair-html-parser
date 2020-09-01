#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_BASE_STEP
#define ECLAIR_HTML_TREE_BUILDER_STEPS_BASE_STEP

#include "common/token_receiver.h"
#include "common/tokens.h"
#include "common/tools.h"
#include "tree_builder/steps/root_builder.h"

namespace xyz {
namespace html_parser {
namespace steps {

  class BaseStep : public TokenReceiver {
  public:
    explicit BaseStep(RootBuilder& rootBuilder)
      : _root(rootBuilder) {
    }
    virtual ~BaseStep() {
    }

  protected:
    bool _isSpace(Token& token) {
      return (token.kind == TokenKinds::CHARACTER &&
          Tools::isSpace(token.data[0]));
    }

    void _selfClosing(Token& token) {
      _root.nodeInserter.insertElement(token);
      _root.openElements.pop();
      _ackSelfClosing(token);
    }

    void _ackSelfClosing(Token& token) {
      if (!token.selfClosing) {
        _root.errorManager.add(
            ErrorKinds::NON_VOID_HTML_ELEMENT_START_TAG_WITH_TRAILING_SOLIDUS,
            token
            );
      }
    }

    bool _checkCommentDocId(Token& token) {
      if (token.kind == TokenKinds::COMMENT) {
        _root.nodeInserter.insertComment(token);
        return true;
      }
      if (token.kind == TokenKinds::DOC_TYPE) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return true;
      }
      return false;
    }

    void _toRCDATA(Token& token) {
      _root.nodeInserter.insertElement(token);
      _root.state.readingMode = TextReadingModes::RCDATA;
      _root.state.expectedEndTag = token.name.value();
      _root.insertionMode.set(InsertionModes::TEXT);
    }

    void _toGenericRaw(Token& token) {
      _root.nodeInserter.insertElement(token);
      _root.state.readingMode = TextReadingModes::RAW_TEXT;
      _root.state.expectedEndTag = token.name.value();
      _root.insertionMode.set(InsertionModes::TEXT);
    }

    bool _hasAttributeTypeHidden(Token& token) {
      auto it = token.attributes().find(u"type");
      return it != token.attributes().end() && it->second == u"hidden";
    }

    void _closeGeneric(Token& token) {
      _closeGeneric(token.name, token);
    }

    void _closeGeneric(const TagName& name, Token& token) {
      _root.openElements.popImplied();
      if (_root.openElements.top().name() != name) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      }
      _root.openElements.popUntil(name);
    }

    void _closeGenericExcept(Token& token) {
      _closeGenericExcept(token.name, token);
    }

    void _closeGenericExcept(const TagName& name, Token& token) {
      _root.openElements.popImplied(name);
      if (_root.openElements.top().name() != name) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      }
      _root.openElements.popUntil(name);
    }

    void _closeGenericThoroughly(Token& token) {
      _root.openElements.popImpliedThoroughly();
      if (_root.openElements.top().name() != token.name) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      }
      _root.openElements.popUntil(token.name);
    }

    RootBuilder& _root;
  };

}
}
}

#endif
