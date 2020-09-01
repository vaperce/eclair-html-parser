#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_BODY
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_BODY

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  class InBody : public BaseStep {
  public:
    explicit InBody(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InBody() {
    }

    virtual void process(Token& token) {
      if (_checkCommentDocId(token)) {
        return;
      }
      switch (token.kind) {
        case TokenKinds::CHARACTER:
          if (token.data[0] == '\0') {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TEXT,
                token);
          } else if (Tools::isSpace(token.data[0])) {
            _root.activeFormattingElements.reconstruct();
            _root.nodeInserter.insertCharacter(token);
          } else {
            _root.activeFormattingElements.reconstruct();
            _root.nodeInserter.insertCharacter(token);
            _root.state.framesetOk = false;
          }
          break;

        case TokenKinds::START_TAG:
          _startTag(token);
          break;

        case TokenKinds::END_TAG:
          _endTag(token);
          break;

        case TokenKinds::END_OF_FILE:
          _eof(token);
          break;

        default:
          break;
      }
    }

  private:
    void _startTag(Token& token) {
      if (token.name == TagKinds::UNKNOWN && token.name.value() == u"image") {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        token.name = TagName(TagKinds::IMG);
      }

      switch (token.name.kind()) {
        case TagKinds::HTML:
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          if (!_root.openElements.contains(TagKinds::TEMPLATE)) {
            _mergeAttributes(_root.openElements.bottom(), token);
          }
          break;

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
          break;

        case TagKinds::BODY:
          _handleStartBody(token);
          break;

        case TagKinds::FRAMESET:
          _handleStartFrameset(token);
          break;

        case TagKinds::ADDRESS:
        case TagKinds::ARTICLE:
        case TagKinds::ASIDE:
        case TagKinds::BLOCKQUOTE:
        case TagKinds::CENTER:
        case TagKinds::DETAILS:
        case TagKinds::DIALOG:
        case TagKinds::DIR:
        case TagKinds::DIV:
        case TagKinds::DL:
        case TagKinds::FIELDSET:
        case TagKinds::FIGCAPTION:
        case TagKinds::FIGURE:
        case TagKinds::FOOTER:
        case TagKinds::HEADER:
        case TagKinds::HGROUP:
        case TagKinds::MAIN:
        case TagKinds::MENU:
        case TagKinds::NAV:
        case TagKinds::OL:
        case TagKinds::P:
        case TagKinds::SECTION:
        case TagKinds::SUMMARY:
        case TagKinds::UL:
          if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
                TagKinds::P)) {
            _closeP(token);
          }
          _root.nodeInserter.insertElement(token);
          break;

        case TagKinds::H1:
        case TagKinds::H2:
        case TagKinds::H3:
        case TagKinds::H4:
        case TagKinds::H5:
        case TagKinds::H6:
          if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
                TagKinds::P)) {
            _closeP(token);
          }
          if (_root.openElements.top().name() == TagKinds::H1 ||
              _root.openElements.top().name() == TagKinds::H2 ||
              _root.openElements.top().name() == TagKinds::H3 ||
              _root.openElements.top().name() == TagKinds::H4 ||
              _root.openElements.top().name() == TagKinds::H5 ||
              _root.openElements.top().name() == TagKinds::H6) {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            _root.openElements.pop();
          }
          _root.nodeInserter.insertElement(token);
          break;

        case TagKinds::PRE:
        case TagKinds::LISTING:
          if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
                TagKinds::P)) {
            _closeP(token);
          }
          _root.nodeInserter.insertElement(token);
          _root.state.ignoreNextLineFeed = true;
          _root.state.framesetOk = false;
          break;

        case TagKinds::FORM:
          _handleStartForm(token);
          break;

        case TagKinds::LI:
          _handleStartLi(token);
          break;

        case TagKinds::DD:
        case TagKinds::DT:
          _handleStartDdDt(token);
          break;

        case TagKinds::PLAINTEXT:
          if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
                TagKinds::P)) {
            _closeP(token);
          }
          _root.nodeInserter.insertElement(token);
          _root.state.readingMode = TextReadingModes::PLAINTEXT;
          break;

        case TagKinds::BUTTON:
          if (_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
                TagKinds::BUTTON)) {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            _root.openElements.popImplied();
            _root.openElements.popUntil(TagName(TagKinds::BUTTON));
          }
          _root.activeFormattingElements.reconstruct();
          _root.nodeInserter.insertElement(token);
          _root.state.framesetOk = false;
          break;

        case TagKinds::A:
          _handleStartA(token);
          break;

        case TagKinds::B:
        case TagKinds::BIG:
        case TagKinds::CODE:
        case TagKinds::EM:
        case TagKinds::FONT:
        case TagKinds::I:
        case TagKinds::S:
        case TagKinds::SMALL:
        case TagKinds::STRIKE:
        case TagKinds::STRONG:
        case TagKinds::TT:
        case TagKinds::U:
          _root.activeFormattingElements.reconstruct();
          _root.nodeInserter.insertElement(token);
          _root.activeFormattingElements.addElement(_root.openElements.top());
          break;

        case TagKinds::NOBR:
          _handleStartNobr(token);
          break;

        case TagKinds::APPLET:
        case TagKinds::MARQUEE:
        case TagKinds::OBJECT:
          _root.activeFormattingElements.reconstruct();
          _root.nodeInserter.insertElement(token);
          _root.activeFormattingElements.addMarker();
          _root.state.framesetOk = false;
          break;

        case TagKinds::TABLE:
          // TODO quirk mode
          if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
                TagKinds::P)) {
            _closeP(token);
          }
          _root.nodeInserter.insertElement(token);
          _root.state.framesetOk = false;
          _root.insertionMode.set(InsertionModes::IN_TABLE);
          break;

        case TagKinds::AREA:
        case TagKinds::BR:
        case TagKinds::EMBED:
        case TagKinds::IMG:
        case TagKinds::KEYGEN:
        case TagKinds::WBR:
          _root.activeFormattingElements.reconstruct();
          _selfClosing(token);
          _root.state.framesetOk = false;
          break;

        case TagKinds::INPUT:
          _root.activeFormattingElements.reconstruct();
          _selfClosing(token);
          if (!_hasAttributeTypeHidden(token)) {
            _root.state.framesetOk = false;
          }
          break;

        case TagKinds::PARAM:
        case TagKinds::SOURCE:
        case TagKinds::TRACK:
          _selfClosing(token);
          break;

        case TagKinds::HR:
          if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
                TagKinds::P)) {
            _closeP(token);
          }
          _selfClosing(token);
          _root.state.framesetOk = false;
          break;

        case TagKinds::TEXTAREA:
          _root.state.ignoreNextLineFeed = true;
          _root.state.framesetOk = false;
          _toRCDATA(token);
          break;

        case TagKinds::XMP:
          if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
                TagKinds::P)) {
            _closeP(token);
          }
          _root.activeFormattingElements.reconstruct();
          _root.state.framesetOk = false;
          _toGenericRaw(token);
          break;

        case TagKinds::IFRAME:
          _root.state.framesetOk = false;
          _toGenericRaw(token);
          break;

        case TagKinds::NOEMBED:
          _toGenericRaw(token);
          break;

        case TagKinds::NOSCRIPT:
          if (_root.state.scriptEnabled) {
            _toGenericRaw(token);
          }
          break;

        case TagKinds::SELECT:
          _root.activeFormattingElements.reconstruct();
          _root.nodeInserter.insertElement(token);
          _root.state.framesetOk = false;
          if (_root.insertionMode.get() == InsertionModes::IN_TABLE ||
              _root.insertionMode.get() == InsertionModes::IN_CAPTION ||
              _root.insertionMode.get() == InsertionModes::IN_TABLE_BODY ||
              _root.insertionMode.get() == InsertionModes::IN_ROW ||
              _root.insertionMode.get() == InsertionModes::IN_CELL) {
            _root.insertionMode.set(InsertionModes::IN_SELECT_IN_TABLE);
          } else {
            _root.insertionMode.set(InsertionModes::IN_SELECT);
          }
          break;

        case TagKinds::OPTGROUP:
        case TagKinds::OPTION:
          if (_root.openElements.top().name() == TagKinds::OPTION) {
            _root.openElements.pop();
          }
          _root.activeFormattingElements.reconstruct();
          _root.nodeInserter.insertElement(token);
          break;

        case TagKinds::RB:
        case TagKinds::RTC:
          if (_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
                TagKinds::RUBY)) {
            _root.openElements.popImplied();
            if (_root.openElements.top().name() != TagKinds::RUBY) {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            }
          }
          _root.nodeInserter.insertElement(token);
          break;

        case TagKinds::RP:
        case TagKinds::RT:
          if (_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
                TagKinds::RUBY)) {
            _root.openElements.popImplied(TagName(TagKinds::RTC));
            if (_root.openElements.top().name() != TagKinds::RTC ||
                _root.openElements.top().name() == TagKinds::RUBY) {
              _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            }
          }
          _root.nodeInserter.insertElement(token);
          break;

        case TagKinds::MATHML_MATH:
          _root.activeFormattingElements.reconstruct();
          _root.nodeInserter.insertForeignElement(token, Namespaces::MATHML);

          if (token.selfClosing) {
            _root.openElements.pop();
          }
          break;

        case TagKinds::SVG_SVG:
          _root.activeFormattingElements.reconstruct();
          _root.nodeInserter.insertForeignElement(token, Namespaces::SVG);

          if (token.selfClosing) {
            _root.openElements.pop();
          }
          break;

        case TagKinds::CAPTION:
        case TagKinds::COL:
        case TagKinds::COLGROUP:
        case TagKinds::FRAME:
        case TagKinds::HEAD:
        case TagKinds::TBODY:
        case TagKinds::TD:
        case TagKinds::TFOOT:
        case TagKinds::TH:
        case TagKinds::THEAD:
        case TagKinds::TR:
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          break;

        default:
          _root.activeFormattingElements.reconstruct();
          _root.nodeInserter.insertElement(token);
      }
    }

    void _endTag(Token& token) {
      switch (token.name.kind()) {
        case TagKinds::TEMPLATE:
          _root.reprocess(InsertionModes::IN_HEAD, token);
          break;

        case TagKinds::BODY:
          _handleEndHtmlBody(token);
          break;

        case TagKinds::HTML:
          if (_handleEndHtmlBody(token)) {
            _root.reprocess(token);
          }
          break;

        case TagKinds::ADDRESS:
        case TagKinds::ARTICLE:
        case TagKinds::ASIDE:
        case TagKinds::BLOCKQUOTE:
        case TagKinds::BUTTON:
        case TagKinds::CENTER:
        case TagKinds::DETAILS:
        case TagKinds::DIALOG:
        case TagKinds::DIR:
        case TagKinds::DIV:
        case TagKinds::DL:
        case TagKinds::FIELDSET:
        case TagKinds::FIGCAPTION:
        case TagKinds::FIGURE:
        case TagKinds::FOOTER:
        case TagKinds::HEADER:
        case TagKinds::HGROUP:
        case TagKinds::LISTING:
        case TagKinds::MAIN:
        case TagKinds::MENU:
        case TagKinds::NAV:
        case TagKinds::OL:
        case TagKinds::PRE:
        case TagKinds::SECTION:
        case TagKinds::SUMMARY:
        case TagKinds::UL:
          if (!_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
                token.name.kind())) {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            break;
          }
          _closeGeneric(token);
          break;

        case TagKinds::FORM:
          _handleEndForm(token);
          break;

        case TagKinds::P:
          if (!_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
                TagKinds::P)) {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            _root.nodeInserter.insertElement(TagKinds::P);
          }
          _closeP(token);
          break;

        case TagKinds::LI:
          if (!_root.openElements.hasElementInScope(ScopeKinds::LIST_ITEM,
                TagKinds::LI)) {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            break;
          }
          _closeGenericExcept(token);
          break;

        case TagKinds::DD:
        case TagKinds::DT:
          if (!_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
                token.name.kind())) {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            break;
          }
          _closeGenericExcept(token);
          break;

        case TagKinds::H1:
        case TagKinds::H2:
        case TagKinds::H3:
        case TagKinds::H4:
        case TagKinds::H5:
        case TagKinds::H6:
          if (!_root.openElements.hasElementsInScope(ScopeKinds::STANDARD,
                {TagKinds::H1, TagKinds::H2, TagKinds::H3, TagKinds::H4,
                TagKinds::H5, TagKinds::H6})) {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            break;
          }
          _root.openElements.popImplied(token.name);
          if (_root.openElements.top().name() != token.name.kind()) {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          }
          _root.openElements.popUntil({TagKinds::H1, TagKinds::H2,
              TagKinds::H3, TagKinds::H4, TagKinds::H5, TagKinds::H6});
          break;

        case TagKinds::A:
        case TagKinds::B:
        case TagKinds::BIG:
        case TagKinds::CODE:
        case TagKinds::EM:
        case TagKinds::FONT:
        case TagKinds::I:
        case TagKinds::NOBR:
        case TagKinds::S:
        case TagKinds::SMALL:
        case TagKinds::STRIKE:
        case TagKinds::STRONG:
        case TagKinds::TT:
        case TagKinds::U:
          _root.adoptionAgency.run(token);
          break;

        case TagKinds::APPLET:
        case TagKinds::MARQUEE:
        case TagKinds::OBJECT:
          if (!_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
                token.name.kind())) {
            _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
            break;
          }
          _closeGeneric(token);
          _root.activeFormattingElements.clearLast();
          break;

        case TagKinds::BR:
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          _root.activeFormattingElements.reconstruct();
          _root.nodeInserter.insertElement(token);
          _root.openElements.pop();
          _root.state.framesetOk = false;
          break;

        default:
          _root.adoptionAgency.inBodyHandleAnyEndTag(token);
          break;
      }
    }

    void _eof(Token& token) {
      if (_root.insertionMode.hasTemplates()) {
        _root.reprocess(InsertionModes::IN_TEMPLATE, token);
        return;
      }
      if (_hasOpenedImportantTags()) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      }
    }

    void _mergeAttributes(Node& element, Token& token) {
      for (auto attribute : token.attributes()) {
        element.attributes.insert(attribute);
      }
    }

    void _closeP(Token& token) {
      _closeGenericExcept(TagName(TagKinds::P), token);
    }

    void _handleStartBody(Token& token) {
      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);

      auto secondElement = _root.openElements.secondBottom();
      if (!secondElement || secondElement->name() != TagKinds::BODY ||
          _root.openElements.contains(TagKinds::TEMPLATE)) {
        return;
      }

      _root.state.framesetOk = false;
      _mergeAttributes(*secondElement, token);
    }

    void _handleStartFrameset(Token& token) {
      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);

      auto secondElement = _root.openElements.secondBottom();
      if (!secondElement || secondElement->name() != TagKinds::BODY ||
          _root.openElements.contains(TagKinds::TEMPLATE)) {
        return;
      }

      if (!_root.state.framesetOk) {
        return;
      }

      secondElement->parent()->erase(*secondElement);
      _root.openElements.popUntil(TagName(TagKinds::BODY));
      _root.nodeInserter.insertElement(token);
      _root.insertionMode.set(InsertionModes::IN_FRAMESET);
    }

    void _handleStartForm(Token& token) {
      bool hasTemplate = _root.openElements.contains(TagKinds::TEMPLATE);

      if (_root.state.form && !hasTemplate) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return;
      }
      if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
            TagKinds::P)) {
        _closeP(token);
      }

      _root.nodeInserter.insertElement(token);
      if (!hasTemplate) {
        _root.state.form = &_root.openElements.top();
      }
    }

    void _handleStartLi(Token& token) {
      _root.state.framesetOk = false;

      for (auto element = _root.openElements.cbegin();
          element != _root.openElements.cend(); ++element) {
        if ((*element)->name() == TagKinds::LI) {
          _closeGenericExcept(TagName(TagKinds::LI), token);
          break;
        }
        if ((*element)->name().category() == TagCategories::SPECIAL &&
            (*element)->name() != TagKinds::ADDRESS &&
            (*element)->name() != TagKinds::DIV &&
            (*element)->name() != TagKinds::P) {
          break;
        }
      }

      if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
            TagKinds::P)) {
        _closeP(token);
      }
      _root.nodeInserter.insertElement(token);
    }

    void _handleStartDdDt(Token& token) {
      _root.state.framesetOk = false;

      for (auto element = _root.openElements.cbegin();
          element != _root.openElements.cend(); ++element) {
        if ((*element)->name() == TagKinds::DD) {
          _closeGenericExcept(TagName(TagKinds::DD), token);
          break;
        }
        if ((*element)->name() == TagKinds::DT) {
          _closeGenericExcept(TagName(TagKinds::DT), token);
          break;
        }
        if ((*element)->name().category() == TagCategories::SPECIAL &&
            (*element)->name() != TagKinds::ADDRESS &&
            (*element)->name() != TagKinds::DIV &&
            (*element)->name() != TagKinds::P) {
          break;
        }
      }

      if (_root.openElements.hasElementInScope(ScopeKinds::BUTTON,
            TagKinds::P)) {
        _closeP(token);
      }
      _root.nodeInserter.insertElement(token);
    }

    void _handleStartA(Token& token) {
      auto badElement = _root.activeFormattingElements.findUntilMarker(
          TagKinds::A);
      if (badElement) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        _root.adoptionAgency.run(token);

        _root.openElements.remove(*badElement);
        _root.activeFormattingElements.remove(*badElement);
      }

      _root.activeFormattingElements.reconstruct();
      _root.nodeInserter.insertElement(token);
      _root.activeFormattingElements.addElement(_root.openElements.top());
    }

    void _handleStartNobr(Token& token) {
      _root.activeFormattingElements.reconstruct();

      if (_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
            TagKinds::NOBR)) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        _root.adoptionAgency.run(token);
        _root.activeFormattingElements.reconstruct();
      }

      _root.nodeInserter.insertElement(token);
      _root.activeFormattingElements.addElement(_root.openElements.top());
    }

    bool _hasOpenedImportantTags() {
      for (auto it = _root.openElements.begin();
          it != _root.openElements.end(); ++it) {
        switch ((*it)->name().kind()) {
          case TagKinds::DD:
          case TagKinds::DT:
          case TagKinds::LI:
          case TagKinds::OPTGROUP:
          case TagKinds::OPTION:
          case TagKinds::P:
          case TagKinds::RB:
          case TagKinds::RP:
          case TagKinds::RT:
          case TagKinds::RTC:
          case TagKinds::TBODY:
          case TagKinds::TD:
          case TagKinds::TFOOT:
          case TagKinds::TH:
          case TagKinds::THEAD:
          case TagKinds::TR:
          case TagKinds::BODY:
          case TagKinds::HTML:
            break;

          default:
            return true;
        }
      }
      return false;
    }

    bool _handleEndHtmlBody(Token& token) {
      if (!_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
            TagKinds::BODY)) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        return false;
      }

      if (_hasOpenedImportantTags()) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      }
      _root.insertionMode.set(InsertionModes::AFTER_BODY);

      return true;
    }

    void _handleEndForm(Token& token) {
      if (_root.openElements.contains(TagKinds::TEMPLATE)) {
        if (!_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
            TagKinds::FORM)) {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          return;
        }
        _closeGeneric(token);

      } else {
        if (!_root.state.form ||
            !_root.openElements.hasElementInScope(ScopeKinds::STANDARD,
              *_root.state.form)) {
          _root.state.form = nullptr;
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          return;
        }

        _root.state.form = nullptr;
        _root.openElements.popImplied();
        if (&_root.openElements.top() != _root.state.form) {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        }

        _root.openElements.remove(*_root.state.form);
      }
    }
  };

}
}
}

#endif
