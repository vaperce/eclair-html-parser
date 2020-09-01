#ifndef ECLAIR_HTML_TREE_BUILDER_NODE_INSERTER
#define ECLAIR_HTML_TREE_BUILDER_NODE_INSERTER

#include <memory>
#include <vector>

#include "common/tokens.h"
#include "eclair-html/node.h"
#include "eclair-html/tag_names.h"
#include "tree_builder/open_elements.h"
#include "tree_builder/state.h"

namespace xyz {
namespace html_parser {

  class NodeInserter {
  public:
    explicit NodeInserter(OpenElements& openElements, const State& state)
      : _openElements(openElements), _state(state), _fosterParenting(false) {
    }

    void insertElementInDocument(TagKinds kind) {
      _insertElement(kind, _state.document);
    }

    void insertElement(TagKinds kind) {
      _insertElement(kind, _target());
    }

    void insertElementInDocument(const Token& token) {
      _insertElement(token, Namespaces::HTML, _state.document);
    }

    void insertForeignElement(const Token& token, Namespaces ns,
        Node* overrideTarget = nullptr) {
      _insertElement(token, ns, _target(overrideTarget));
    }

    void insertElement(const Token& token, Node* overrideTarget = nullptr) {
      insertForeignElement(token, Namespaces::HTML, overrideTarget);
    }

    void insertElement(Node::Ptr node, Node* overrideTarget = nullptr) {
      _insertElement(std::move(node), _target(overrideTarget));
    }

    void insertCommentInDocument(const Token& token) {
      _state.document.pushBack(Node::newComment(token.data));
    }

    void insertComment(const Token& token, Node* overrideTarget = nullptr) {
      _target(overrideTarget).pushBack(Node::newComment(token.data));
    }

    void insertCharacter(const Token& token) {
      _insertCharacter(token.data);
    }

    void insertReplacementCharacter() {
      _insertCharacter(u"\uFFFD");
    }

    void enableFosterParenting() {
      _fosterParenting = true;
    }

    void disableFosterParenting() {
      _fosterParenting = false;
    }

  private:
    void _insertElement(TagKinds kind, Node& target) {
      _insertElement(Node::newElement(TagName(kind)), target);
    }

    void _insertElement(const Token& token, Namespaces ns, Node& target) {
      auto element = Node::newElement(TagName(token.name, ns));
      element->attributes.insert(token.attributes().begin(),
          token.attributes().end());

      if (element->name().nameSpace() != Namespaces::HTML) {
        _adjustForeign(*element);
      }

      _insertElement(std::move(element), target);
    }

    void _insertElement(Node::Ptr element, Node& target) {
      _openElements.push(*element);
      target.pushBack(std::move(element));
    }

    void _insertCharacter(const std::u16string& value) {
      Node& target = _target();
      if (!target.empty() && target.back().kind() == NodeKinds::TEXT) {
        target.back().appendContent(value);
      } else {
        target.pushBack(Node::newText(value));
      }
    }

    Node& _handleFosterParenting() {
      auto item = _openElements.cbegin();
      for (; item != _openElements.cend(); ++item) {
        if ((*item)->name() == TagKinds::TEMPLATE) {
          return **item;
        }
        if ((*item)->name() == TagKinds::TABLE) {
          break;
        }
      }

      if (item == _openElements.cend()) {
        return _openElements.bottom();
      }

      auto parent = (*item)->parent();
      if (parent) {
        return *parent;
      }

      return **(++item);
    }

    Node& _target(Node* overrideTarget = nullptr) {
      Node& res = overrideTarget ? *overrideTarget : _openElements.top();

      if (_fosterParenting && (res.name() == TagKinds::TABLE ||
            res.name() == TagKinds::TBODY || res.name() == TagKinds::TFOOT ||
            res.name() == TagKinds::THEAD || res.name() == TagKinds::TR)) {
        return _handleFosterParenting();
      }
      return res;
    }

    void _adjustForeign(Node& element) {
      _adjustMathMLAttributes(element);
      _adjustSVGAttributes(element);
      _adjustForeignAttributes(element);
    }

    void _adjustMathMLAttributes(Node& element) {
      _replaceAttribute(element, u"definitionurl", u"definitionURL");
    }

    void _adjustSVGAttributes(Node& element) {
      _replaceAttribute(element, u"attributename", u"attributeName");
      _replaceAttribute(element, u"attributetype", u"attributeType");
      _replaceAttribute(element, u"basefrequency", u"baseFrequency");
      _replaceAttribute(element, u"baseprofile", u"baseProfile");
      _replaceAttribute(element, u"calcmode", u"calcMode");
      _replaceAttribute(element, u"clippathunits", u"clipPathUnits");
      _replaceAttribute(element, u"diffuseconstant", u"diffuseConstant");
      _replaceAttribute(element, u"edgemode", u"edgeMode");
      _replaceAttribute(element, u"filterunits", u"filterUnits");
      _replaceAttribute(element, u"glyphref", u"glyphRef");
      _replaceAttribute(element, u"gradienttransform", u"gradientTransform");
      _replaceAttribute(element, u"gradientunits", u"gradientUnits");
      _replaceAttribute(element, u"kernelmatrix", u"kernelMatrix");
      _replaceAttribute(element, u"kernelunitlength", u"kernelUnitLength");
      _replaceAttribute(element, u"keypoints", u"keyPoints");
      _replaceAttribute(element, u"keysplines", u"keySplines");
      _replaceAttribute(element, u"keytimes", u"keyTimes");
      _replaceAttribute(element, u"lengthadjust", u"lengthAdjust");
      _replaceAttribute(element, u"limitingconeangle", u"limitingConeAngle");
      _replaceAttribute(element, u"markerheight", u"markerHeight");
      _replaceAttribute(element, u"markerunits", u"markerUnits");
      _replaceAttribute(element, u"markerwidth", u"markerWidth");
      _replaceAttribute(element, u"maskcontentunits", u"maskContentUnits");
      _replaceAttribute(element, u"maskunits", u"maskUnits");
      _replaceAttribute(element, u"numoctaves", u"numOctaves");
      _replaceAttribute(element, u"pathlength", u"pathLength");
      _replaceAttribute(element, u"patterncontentunits", u"patternContentUnits");
      _replaceAttribute(element, u"patterntransform", u"patternTransform");
      _replaceAttribute(element, u"patternunits", u"patternUnits");
      _replaceAttribute(element, u"pointsatx", u"pointsAtX");
      _replaceAttribute(element, u"pointsaty", u"pointsAtY");
      _replaceAttribute(element, u"pointsatz", u"pointsAtZ");
      _replaceAttribute(element, u"preservealpha", u"preserveAlpha");
      _replaceAttribute(element, u"preserveaspectratio", u"preserveAspectRatio");
      _replaceAttribute(element, u"primitiveunits", u"primitiveUnits");
      _replaceAttribute(element, u"refx", u"refX");
      _replaceAttribute(element, u"refy", u"refY");
      _replaceAttribute(element, u"repeatcount", u"repeatCount");
      _replaceAttribute(element, u"repeatdur", u"repeatDur");
      _replaceAttribute(element, u"requiredextensions", u"requiredExtensions");
      _replaceAttribute(element, u"requiredfeatures", u"requiredFeatures");
      _replaceAttribute(element, u"specularconstant", u"specularConstant");
      _replaceAttribute(element, u"specularexponent", u"specularExponent");
      _replaceAttribute(element, u"spreadmethod", u"spreadMethod");
      _replaceAttribute(element, u"startoffset", u"startOffset");
      _replaceAttribute(element, u"stddeviation", u"stdDeviation");
      _replaceAttribute(element, u"stitchtiles", u"stitchTiles");
      _replaceAttribute(element, u"surfacescale", u"surfaceScale");
      _replaceAttribute(element, u"systemlanguage", u"systemLanguage");
      _replaceAttribute(element, u"tablevalues", u"tableValues");
      _replaceAttribute(element, u"targetx", u"targetX");
      _replaceAttribute(element, u"targety", u"targetY");
      _replaceAttribute(element, u"textlength", u"textLength");
      _replaceAttribute(element, u"viewbox", u"viewBox");
      _replaceAttribute(element, u"viewtarget", u"viewTarget");
      _replaceAttribute(element, u"xchannelselector", u"xChannelSelector");
      _replaceAttribute(element, u"ychannelselector", u"yChannelSelector");
      _replaceAttribute(element, u"zoomandpan", u"zoomAndPan");
    }

    void _adjustForeignAttributes(Node& element) {
      // FIXME: not supported
    }

    void _replaceAttribute(Node& element, const std::u16string& name,
        const std::u16string& newName) {
      auto attributes = element.attributes;
      auto attribute = attributes.find(name);
      if (attribute != attributes.end()) {
        auto value = attribute->second;
        attributes.erase(attribute);
        attributes.insert(std::make_pair(newName, value));
      }
    }

    OpenElements& _openElements;
    const State& _state;
    bool _fosterParenting;
  };

}
}

#endif
