#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_FOREIGN_CONTENT
#define ECLAIR_HTML_TREE_BUILDER_STEPS_FOREIGN_CONTENT

#include <unordered_map>

#include "common/tokens.h"
#include "common/tools.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  class ForeignContent : public BaseStep {
  public:
    explicit ForeignContent(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~ForeignContent() {
    }

    static bool isMathMLIntegrationPoint(Node& element) {
      switch (element.name().kind()) {
        case TagKinds::MATHML_MI:
        case TagKinds::MATHML_MO:
        case TagKinds::MATHML_MN:
        case TagKinds::MATHML_MS:
        case TagKinds::MATHML_MTEXT:
          return true;

        default:
          return false;
      }
    }

    static bool isHTMLIntegrationPoint(Node& element) {
      if (element.name() == TagKinds::MATHML_ANNOTATION_XML) {
        auto it = element.attributes.find(u"encoding");
        if (it != element.attributes.end()) {
          auto value = Tools::toLowercaseStr(it->second);
          if (value == u"text/html" || value == u"application/xhtml+xml") {
            return true;
          }
        }
      }

      switch (element.name().kind()) {
        case TagKinds::SVG_FOREIGNOBJECT:
        case TagKinds::SVG_DESC:
        case TagKinds::SVG_TITLE:
          return true;

        default:
          return false;
      }
    }

    virtual void process(Token& token) {
      if (token.kind == TokenKinds::CHARACTER) {
        if (token.data[0] == '\0') {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TEXT, token);
          _root.nodeInserter.insertReplacementCharacter();
        } else {
          _root.nodeInserter.insertCharacter(token);
          if (!_isSpace(token)) {
            _root.state.framesetOk = false;
          }
        }
        return;
      }
      if (_checkCommentDocId(token)) {
        return;
      }
      if (token.kind == TokenKinds::START_TAG) {
        switch (token.name.kind()) {
          case TagKinds::FONT:
            _handleHtmlStartFont(token);
            return;

          case TagKinds::B:
          case TagKinds::BIG:
          case TagKinds::BLOCKQUOTE:
          case TagKinds::BODY:
          case TagKinds::BR:
          case TagKinds::CENTER:
          case TagKinds::CODE:
          case TagKinds::DD:
          case TagKinds::DIV:
          case TagKinds::DL:
          case TagKinds::DT:
          case TagKinds::EM:
          case TagKinds::EMBED:
          case TagKinds::H1:
          case TagKinds::H2:
          case TagKinds::H3:
          case TagKinds::H4:
          case TagKinds::H5:
          case TagKinds::H6:
          case TagKinds::HEAD:
          case TagKinds::HR:
          case TagKinds::I:
          case TagKinds::IMG:
          case TagKinds::LI:
          case TagKinds::LISTING:
          case TagKinds::MENU:
          case TagKinds::META:
          case TagKinds::NOBR:
          case TagKinds::OL:
          case TagKinds::P:
          case TagKinds::PRE:
          case TagKinds::RUBY:
          case TagKinds::S:
          case TagKinds::SMALL:
          case TagKinds::SPAN:
          case TagKinds::STRONG:
          case TagKinds::STRIKE:
          case TagKinds::SUB:
          case TagKinds::SUP:
          case TagKinds::TABLE:
          case TagKinds::TT:
          case TagKinds::U:
          case TagKinds::UL:
          case TagKinds::VAR:
            _handleHtmlStart(token);
            return;

          default:
            _handleAnyStart(token);
            return;
        }
      }
      if (token.kind == TokenKinds::END_TAG) {
        if (token.name.value() == u"script" &&
            _root.openElements.top().name().nameSpace() == Namespaces::SVG &&
            _root.openElements.top().name().value() == u"script") {
          _handleEndScript(token);
        } else {
          _handleAnyEnd(token);
        }
      }
    }

  private:
    void _handleHtmlStartFont(Token& token) {
      if (token.attributes().find(u"color") != token.attributes().end() ||
          token.attributes().find(u"face") != token.attributes().end() ||
          token.attributes().find(u"size") != token.attributes().end()) {
        _handleHtmlStart(token);
      }
    }

    void _handleHtmlStart(Token& token) {
      _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      // FIXME: fragment

      while (!_root.openElements.empty()) {
        _root.openElements.pop();
        Node& currentNode = _root.openElements.top();
        if (isMathMLIntegrationPoint(currentNode) ||
            isHTMLIntegrationPoint(currentNode) ||
            currentNode.name().nameSpace() == Namespaces::HTML) {
          break;
        }
      }
      _root.reprocess(token);
    }

    void _handleAnyStart(Token& token) {
      static const std::unordered_map<std::u16string, std::u16string>
        NAME_MAP = {
          {u"altglyph", u"altGlyph"},
          {u"altglyphdef", u"altGlyphDef"},
          {u"altglyphitem", u"altGlyphItem"},
          {u"animatecolor", u"animateColor"},
          {u"animatemotion", u"animateMotion"},
          {u"animatetransform", u"animateTransform"},
          {u"clippath", u"clipPath"},
          {u"feblend", u"feBlend"},
          {u"fecolormatrix", u"feColorMatrix"},
          {u"fecomponenttransfer", u"feComponentTransfer"},
          {u"fecomposite", u"feComposite"},
          {u"feconvolvematrix", u"feConvolveMatrix"},
          {u"fediffuselighting", u"feDiffuseLighting"},
          {u"fedisplacementmap", u"feDisplacementMap"},
          {u"fedistantlight", u"feDistantLight"},
          {u"fedropshadow", u"feDropShadow"},
          {u"feflood", u"feFlood"},
          {u"fefunca", u"feFuncA"},
          {u"fefuncb", u"feFuncB"},
          {u"fefuncg", u"feFuncG"},
          {u"fefuncr", u"feFuncR"},
          {u"fegaussianblur", u"feGaussianBlur"},
          {u"feimage", u"feImage"},
          {u"femerge", u"feMerge"},
          {u"femergenode", u"feMergeNode"},
          {u"femorphology", u"feMorphology"},
          {u"feoffset", u"feOffset"},
          {u"fepointlight", u"fePointLight"},
          {u"fespecularlighting", u"feSpecularLighting"},
          {u"fespotlight", u"feSpotLight"},
          {u"fetile", u"feTile"},
          {u"feturbulence", u"feTurbulence"},
          {u"foreignobject", u"foreignObject"},
          {u"glyphref", u"glyphRef"},
          {u"lineargradient", u"linearGradient"},
          {u"radialgradient", u"radialGradient"},
          {u"textpath", u"textPath"},
        };

      Namespaces ns = _root.openElements.top().name().nameSpace();
      if (ns == Namespaces::SVG) {
        auto mappedName = NAME_MAP.find(token.name.value());
        if (mappedName != NAME_MAP.end()) {
          token.name.changeName(mappedName->second);
        }
      }
      _root.nodeInserter.insertForeignElement(token, ns);

      if (token.selfClosing) {
        if (ns == Namespaces::SVG && token.name.value() == u"script") {
          _handleEndScript(token);
        } else {
          _root.openElements.pop();
        }
      }
    }

    void _handleEndScript(Token&) {
      _root.openElements.pop();
    }

    void _handleAnyEnd(Token& token) {
      Node& topElement = _root.openElements.top();
      if (Tools::toLowercaseStr(topElement.name().value()) !=
          token.name.value()) {
        _root.errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
      }

      for (auto element = _root.openElements.begin();
          element != _root.openElements.end(); ++element) {
        if (element != _root.openElements.begin() &&
            (*element)->name().nameSpace() == Namespaces::HTML) {
          break;
        }

        auto elementName = Tools::toLowercaseStr((*element)->name().value());
        if (elementName == token.name.value()) {
          _root.openElements.popUntil(element);
          return;
        }
      }

      _root.reprocess(token);
    }
  };

}
}
}

#endif
