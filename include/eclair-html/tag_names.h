#ifndef ECLAIR_HTML_TAG_NAMES
#define ECLAIR_HTML_TAG_NAMES

#include <string>
#include <unordered_map>
#include <vector>

namespace xyz {
namespace html_parser {

  enum class TagKinds {
    UNKNOWN = 0,

    // SPECIAL
    ADDRESS,
    APPLET,
    AREA,
    ARTICLE,
    ASIDE,
    BASE,
    BASEFONT,
    BGSOUND,
    BLOCKQUOTE,
    BODY,
    BR,
    BUTTON,
    CAPTION,
    CENTER,
    COL,
    COLGROUP,
    DD,
    DETAILS,
    DIR,
    DIV,
    DL,
    DT,
    EMBED,
    FIELDSET,
    FIGCAPTION,
    FIGURE,
    FOOTER,
    FORM,
    FRAME,
    FRAMESET,
    H1,
    H2,
    H3,
    H4,
    H5,
    H6,
    HEAD,
    HEADER,
    HGROUP,
    HR,
    HTML,
    IFRAME,
    IMG,
    INPUT,
    KEYGEN,
    LI,
    LINK,
    LISTING,
    MAIN,
    MARQUEE,
    MENU,
    META,
    NAV,
    NOEMBED,
    NOFRAMES,
    NOSCRIPT,
    OBJECT,
    OL,
    P,
    PARAM,
    PLAINTEXT,
    PRE,
    SCRIPT,
    SECTION,
    SELECT,
    SOURCE,
    STYLE,
    SUMMARY,
    TABLE,
    TBODY,
    TD,
    TEMPLATE,
    TEXTAREA,
    TFOOT,
    TH,
    THEAD,
    TITLE,
    TR,
    TRACK,
    UL,
    WBR,
    XMP,
    MATHML_MI,
    MATHML_MO,
    MATHML_MN,
    MATHML_MS,
    MATHML_MTEXT,
    MATHML_ANNOTATION_XML,
    SVG_FOREIGNOBJECT,
    SVG_DESC,
    SVG_TITLE,

    // FORMATTING
    A,
    B,
    BIG,
    CODE,
    EM,
    FONT,
    I,
    NOBR,
    S,
    SMALL,
    STRIKE,
    STRONG,
    TT,
    U,

    // ORDINARY
    ABBR,
    AUDIO,
    BDI,
    BDO,
    CANVAS,
    CITE,
    DATA,
    DATALIST,
    DEL,
    DFN,
    DIALOG,
    INS,
    KBD,
    LABEL,
    LEGEND,
    MAP,
    MARK,
    MATHML_MATH,
    METER,
    OPTGROUP,
    OPTION,
    OUTPUT,
    PICTURE,
    PROGRESS,
    Q,
    RB,
    RP,
    RT,
    RTC,
    RUBY,
    SAMP,
    SLOT,
    SPAN,
    SUB,
    SUP,
    SVG_SVG,
    TIME,
    VAR,
    VIDEO,
  };

  enum class TagCategories {
    SPECIAL,
    FORMATTING,
    ORDINARY,
  };

  enum class Namespaces {
    HTML,
    MATHML,
    SVG,
    XLINK,
    XML,
    XMLNS,
  };

  class TagName {
  public:
    explicit TagName()
      : _ns(Namespaces::HTML), _kind(TagKinds::UNKNOWN) {
    }

    explicit TagName(TagKinds kind)
      : _ns(Namespaces::HTML), _kind(kind) {
    }

    // Accept only lowercase names
    explicit TagName(const std::u16string& name)
      : _ns(Namespaces::HTML), _name(name) {
      auto it = _HTML_STR_MAP.find(name);
      if (it == _HTML_STR_MAP.end()) {
        _kind = TagKinds::UNKNOWN;
      } else {
        _kind = it->second;
      }
    }

    explicit TagName(const TagName& tagName, Namespaces ns)
      : _ns(ns), _kind(tagName._kind) {
      if (ns == Namespaces::HTML) {
        if (_kind == TagKinds::UNKNOWN) {
          _name = tagName._name;
        }
      } else if (ns == Namespaces::SVG) {
        auto it = _SVG_STR_MAP.find(tagName._name);
        if (it == _SVG_STR_MAP.end()) {
          _kind = TagKinds::UNKNOWN;
          _name = tagName._name;
        } else {
          _kind = it->second;
        }
      } else if (ns == Namespaces::MATHML) {
        auto it = _MATHML_STR_MAP.find(tagName._name);
        if (it == _MATHML_STR_MAP.end()) {
          _kind = TagKinds::UNKNOWN;
          _name = tagName._name;
        } else {
          _kind = it->second;
        }
      }
    }

    Namespaces nameSpace() const {
      return _ns;
    }

    TagKinds kind() const {
      return _kind;
    }

    void changeName(const std::u16string& name) {
      _kind = TagKinds::UNKNOWN;
      _name = name;
    }

    std::u16string value() const {
      if (_name.empty()) {
        return _ENUM_MAP[static_cast<int>(_kind)];
      }
      return _name;
    }

    TagCategories category() const {
      return _CATEGORY_MAP[static_cast<int>(_kind)];
    }

    bool empty() const {
      return _kind == TagKinds::UNKNOWN && _name.empty();
    }

    bool operator ==(const TagName& other) const {
      bool res = _kind == other._kind;
      if (res && _kind == TagKinds::UNKNOWN) {
        return _name == other._name;
      }
      return res;
    }

    bool operator ==(TagKinds kind) const {
      if (_kind == TagKinds::UNKNOWN) {
        return false;
      }
      return _kind == kind;
    }

    bool operator !=(const TagName& other) const {
      return !(*this == other);
    }

    bool operator !=(TagKinds kind) const {
      return !(*this == kind);
    }

  public:
    static const std::unordered_map<std::u16string, TagKinds> _HTML_STR_MAP;
    static const std::unordered_map<std::u16string, TagKinds> _SVG_STR_MAP;
    static const std::unordered_map<std::u16string, TagKinds> _MATHML_STR_MAP;
    static const std::vector<std::u16string> _ENUM_MAP;
    static const std::vector<TagCategories> _CATEGORY_MAP;

    Namespaces _ns;
    TagKinds _kind;
    std::u16string _name;
  };

}
}

#endif
