#include "eclair-html/tag_names.h"

namespace eclair_html {
namespace html_parser {

const std::unordered_map<std::u16string, TagKinds> TagName::_HTML_STR_MAP = {
    {u"address", TagKinds::ADDRESS},
    {u"applet", TagKinds::APPLET},
    {u"area", TagKinds::AREA},
    {u"article", TagKinds::ARTICLE},
    {u"aside", TagKinds::ASIDE},
    {u"base", TagKinds::BASE},
    {u"basefont", TagKinds::BASEFONT},
    {u"bgsound", TagKinds::BGSOUND},
    {u"blockquote", TagKinds::BLOCKQUOTE},
    {u"body", TagKinds::BODY},
    {u"br", TagKinds::BR},
    {u"button", TagKinds::BUTTON},
    {u"caption", TagKinds::CAPTION},
    {u"center", TagKinds::CENTER},
    {u"col", TagKinds::COL},
    {u"colgroup", TagKinds::COLGROUP},
    {u"dd", TagKinds::DD},
    {u"details", TagKinds::DETAILS},
    {u"dir", TagKinds::DIR},
    {u"div", TagKinds::DIV},
    {u"dl", TagKinds::DL},
    {u"dt", TagKinds::DT},
    {u"embed", TagKinds::EMBED},
    {u"fieldset", TagKinds::FIELDSET},
    {u"figcaption", TagKinds::FIGCAPTION},
    {u"figure", TagKinds::FIGURE},
    {u"footer", TagKinds::FOOTER},
    {u"form", TagKinds::FORM},
    {u"frame", TagKinds::FRAME},
    {u"frameset", TagKinds::FRAMESET},
    {u"h1", TagKinds::H1},
    {u"h2", TagKinds::H2},
    {u"h3", TagKinds::H3},
    {u"h4", TagKinds::H4},
    {u"h5", TagKinds::H5},
    {u"h6", TagKinds::H6},
    {u"head", TagKinds::HEAD},
    {u"header", TagKinds::HEADER},
    {u"hgroup", TagKinds::HGROUP},
    {u"hr", TagKinds::HR},
    {u"html", TagKinds::HTML},
    {u"iframe", TagKinds::IFRAME},
    {u"img", TagKinds::IMG},
    {u"input", TagKinds::INPUT},
    {u"keygen", TagKinds::KEYGEN},
    {u"li", TagKinds::LI},
    {u"link", TagKinds::LINK},
    {u"listing", TagKinds::LISTING},
    {u"main", TagKinds::MAIN},
    {u"marquee", TagKinds::MARQUEE},
    {u"menu", TagKinds::MENU},
    {u"meta", TagKinds::META},
    {u"nav", TagKinds::NAV},
    {u"noembed", TagKinds::NOEMBED},
    {u"noframes", TagKinds::NOFRAMES},
    {u"noscript", TagKinds::NOSCRIPT},
    {u"object", TagKinds::OBJECT},
    {u"ol", TagKinds::OL},
    {u"p", TagKinds::P},
    {u"param", TagKinds::PARAM},
    {u"plaintext", TagKinds::PLAINTEXT},
    {u"pre", TagKinds::PRE},
    {u"script", TagKinds::SCRIPT},
    {u"section", TagKinds::SECTION},
    {u"select", TagKinds::SELECT},
    {u"source", TagKinds::SOURCE},
    {u"style", TagKinds::STYLE},
    {u"summary", TagKinds::SUMMARY},
    {u"table", TagKinds::TABLE},
    {u"tbody", TagKinds::TBODY},
    {u"td", TagKinds::TD},
    {u"template", TagKinds::TEMPLATE},
    {u"textarea", TagKinds::TEXTAREA},
    {u"tfoot", TagKinds::TFOOT},
    {u"th", TagKinds::TH},
    {u"thead", TagKinds::THEAD},
    {u"title", TagKinds::TITLE},
    {u"tr", TagKinds::TR},
    {u"track", TagKinds::TRACK},
    {u"ul", TagKinds::UL},
    {u"wbr", TagKinds::WBR},
    {u"xmp", TagKinds::XMP},
    {u"a", TagKinds::A},
    {u"b", TagKinds::B},
    {u"big", TagKinds::BIG},
    {u"code", TagKinds::CODE},
    {u"em", TagKinds::EM},
    {u"font", TagKinds::FONT},
    {u"i", TagKinds::I},
    {u"nobr", TagKinds::NOBR},
    {u"s", TagKinds::S},
    {u"small", TagKinds::SMALL},
    {u"strike", TagKinds::STRIKE},
    {u"strong", TagKinds::STRONG},
    {u"tt", TagKinds::TT},
    {u"u", TagKinds::U},
    {u"abbr", TagKinds::ABBR},
    {u"audio", TagKinds::AUDIO},
    {u"bdi", TagKinds::BDI},
    {u"bdo", TagKinds::BDO},
    {u"canvas", TagKinds::CANVAS},
    {u"cite", TagKinds::CITE},
    {u"data", TagKinds::DATA},
    {u"datalist", TagKinds::DATALIST},
    {u"del", TagKinds::DEL},
    {u"dfn", TagKinds::DFN},
    {u"dialog", TagKinds::DIALOG},
    {u"ins", TagKinds::INS},
    {u"kbd", TagKinds::KBD},
    {u"label", TagKinds::LABEL},
    {u"legend", TagKinds::LEGEND},
    {u"map", TagKinds::MAP},
    {u"mark", TagKinds::MARK},
    {u"math", TagKinds::MATHML_MATH},
    {u"meter", TagKinds::METER},
    {u"optgroup", TagKinds::OPTGROUP},
    {u"option", TagKinds::OPTION},
    {u"output", TagKinds::OUTPUT},
    {u"picture", TagKinds::PICTURE},
    {u"progress", TagKinds::PROGRESS},
    {u"q", TagKinds::Q},
    {u"rb", TagKinds::RB},
    {u"rp", TagKinds::RP},
    {u"rt", TagKinds::RT},
    {u"rtc", TagKinds::RTC},
    {u"ruby", TagKinds::RUBY},
    {u"samp", TagKinds::SAMP},
    {u"slot", TagKinds::SLOT},
    {u"span", TagKinds::SPAN},
    {u"sub", TagKinds::SUB},
    {u"sup", TagKinds::SUP},
    {u"svg", TagKinds::SVG_SVG},
    {u"time", TagKinds::TIME},
    {u"var", TagKinds::VAR},
    {u"video", TagKinds::VIDEO},
};

const std::unordered_map<std::u16string, TagKinds> TagName::_SVG_STR_MAP = {
    {u"foreignObject", TagKinds::SVG_FOREIGNOBJECT},
    {u"desc", TagKinds::SVG_DESC},
    {u"title", TagKinds::SVG_TITLE},
};

const std::unordered_map<std::u16string, TagKinds> TagName::_MATHML_STR_MAP = {
    {u"mi", TagKinds::MATHML_MI},
    {u"mo", TagKinds::MATHML_MO},
    {u"mn", TagKinds::MATHML_MN},
    {u"ms", TagKinds::MATHML_MS},
    {u"mtext", TagKinds::MATHML_MTEXT},
    {u"annotation-xml", TagKinds::MATHML_ANNOTATION_XML},
};

const std::vector<std::u16string> TagName::_ENUM_MAP = {
    u"",
    u"address",
    u"applet",
    u"area",
    u"article",
    u"aside",
    u"base",
    u"basefont",
    u"bgsound",
    u"blockquote",
    u"body",
    u"br",
    u"button",
    u"caption",
    u"center",
    u"col",
    u"colgroup",
    u"dd",
    u"details",
    u"dir",
    u"div",
    u"dl",
    u"dt",
    u"embed",
    u"fieldset",
    u"figcaption",
    u"figure",
    u"footer",
    u"form",
    u"frame",
    u"frameset",
    u"h1",
    u"h2",
    u"h3",
    u"h4",
    u"h5",
    u"h6",
    u"head",
    u"header",
    u"hgroup",
    u"hr",
    u"html",
    u"iframe",
    u"img",
    u"input",
    u"keygen",
    u"li",
    u"link",
    u"listing",
    u"main",
    u"marquee",
    u"menu",
    u"meta",
    u"nav",
    u"noembed",
    u"noframes",
    u"noscript",
    u"object",
    u"ol",
    u"p",
    u"param",
    u"plaintext",
    u"pre",
    u"script",
    u"section",
    u"select",
    u"source",
    u"style",
    u"summary",
    u"table",
    u"tbody",
    u"td",
    u"template",
    u"textarea",
    u"tfoot",
    u"th",
    u"thead",
    u"title",
    u"tr",
    u"track",
    u"ul",
    u"wbr",
    u"xmp",
    u"mi",
    u"mo",
    u"mn",
    u"ms",
    u"mtext",
    u"annotation-xml",
    u"foreignobject",
    u"desc",
    u"title",
    u"a",
    u"b",
    u"big",
    u"code",
    u"em",
    u"font",
    u"i",
    u"nobr",
    u"s",
    u"small",
    u"strike",
    u"strong",
    u"tt",
    u"u",
    u"abbr",
    u"audio",
    u"bdi",
    u"bdo",
    u"canvas",
    u"cite",
    u"data",
    u"datalist",
    u"del",
    u"dfn",
    u"dialog",
    u"ins",
    u"kbd",
    u"label",
    u"legend",
    u"map",
    u"mark",
    u"math",
    u"meter",
    u"optgroup",
    u"option",
    u"output",
    u"picture",
    u"progress",
    u"q",
    u"rb",
    u"rp",
    u"rt",
    u"rtc",
    u"ruby",
    u"samp",
    u"slot",
    u"span",
    u"sub",
    u"sup",
    u"svg",
    u"time",
    u"var",
    u"video",
};

const std::vector<TagCategories> TagName::_CATEGORY_MAP = {
    TagCategories::ORDINARY,   // UNKNOWN
    TagCategories::SPECIAL,    // ADDRESS
    TagCategories::SPECIAL,    // APPLET
    TagCategories::SPECIAL,    // AREA
    TagCategories::SPECIAL,    // ARTICLE
    TagCategories::SPECIAL,    // ASIDE
    TagCategories::SPECIAL,    // BASE
    TagCategories::SPECIAL,    // BASEFONT
    TagCategories::SPECIAL,    // BGSOUND
    TagCategories::SPECIAL,    // BLOCKQUOTE
    TagCategories::SPECIAL,    // BODY
    TagCategories::SPECIAL,    // BR
    TagCategories::SPECIAL,    // BUTTON
    TagCategories::SPECIAL,    // CAPTION
    TagCategories::SPECIAL,    // CENTER
    TagCategories::SPECIAL,    // COL
    TagCategories::SPECIAL,    // COLGROUP
    TagCategories::SPECIAL,    // DD
    TagCategories::SPECIAL,    // DETAILS
    TagCategories::SPECIAL,    // DIR
    TagCategories::SPECIAL,    // DIV
    TagCategories::SPECIAL,    // DL
    TagCategories::SPECIAL,    // DT
    TagCategories::SPECIAL,    // EMBED
    TagCategories::SPECIAL,    // FIELDSET
    TagCategories::SPECIAL,    // FIGCAPTION
    TagCategories::SPECIAL,    // FIGURE
    TagCategories::SPECIAL,    // FOOTER
    TagCategories::SPECIAL,    // FORM
    TagCategories::SPECIAL,    // FRAME
    TagCategories::SPECIAL,    // FRAMESET
    TagCategories::SPECIAL,    // H1
    TagCategories::SPECIAL,    // H2
    TagCategories::SPECIAL,    // H3
    TagCategories::SPECIAL,    // H4
    TagCategories::SPECIAL,    // H5
    TagCategories::SPECIAL,    // H6
    TagCategories::SPECIAL,    // HEAD
    TagCategories::SPECIAL,    // HEADER
    TagCategories::SPECIAL,    // HGROUP
    TagCategories::SPECIAL,    // HR
    TagCategories::SPECIAL,    // HTML
    TagCategories::SPECIAL,    // IFRAME
    TagCategories::SPECIAL,    // IMG
    TagCategories::SPECIAL,    // INPUT
    TagCategories::SPECIAL,    // KEYGEN
    TagCategories::SPECIAL,    // LI
    TagCategories::SPECIAL,    // LINK
    TagCategories::SPECIAL,    // LISTING
    TagCategories::SPECIAL,    // MAIN
    TagCategories::SPECIAL,    // MARQUEE
    TagCategories::SPECIAL,    // MENU
    TagCategories::SPECIAL,    // META
    TagCategories::SPECIAL,    // NAV
    TagCategories::SPECIAL,    // NOEMBED
    TagCategories::SPECIAL,    // NOFRAMES
    TagCategories::SPECIAL,    // NOSCRIPT
    TagCategories::SPECIAL,    // OBJECT
    TagCategories::SPECIAL,    // OL
    TagCategories::SPECIAL,    // P
    TagCategories::SPECIAL,    // PARAM
    TagCategories::SPECIAL,    // PLAINTEXT
    TagCategories::SPECIAL,    // PRE
    TagCategories::SPECIAL,    // SCRIPT
    TagCategories::SPECIAL,    // SECTION
    TagCategories::SPECIAL,    // SELECT
    TagCategories::SPECIAL,    // SOURCE
    TagCategories::SPECIAL,    // STYLE
    TagCategories::SPECIAL,    // SUMMARY
    TagCategories::SPECIAL,    // TABLE
    TagCategories::SPECIAL,    // TBODY
    TagCategories::SPECIAL,    // TD
    TagCategories::SPECIAL,    // TEMPLATE
    TagCategories::SPECIAL,    // TEXTAREA
    TagCategories::SPECIAL,    // TFOOT
    TagCategories::SPECIAL,    // TH
    TagCategories::SPECIAL,    // THEAD
    TagCategories::SPECIAL,    // TITLE
    TagCategories::SPECIAL,    // TR
    TagCategories::SPECIAL,    // TRACK
    TagCategories::SPECIAL,    // UL
    TagCategories::SPECIAL,    // WBR
    TagCategories::SPECIAL,    // XMP
    TagCategories::SPECIAL,    // MATHML_MI
    TagCategories::SPECIAL,    // MATHML_MO
    TagCategories::SPECIAL,    // MATHML_MN
    TagCategories::SPECIAL,    // MATHML_MS
    TagCategories::SPECIAL,    // MATHML_MTEXT
    TagCategories::SPECIAL,    // MATHML_ANNOTATION_XML
    TagCategories::SPECIAL,    // SVG_FOREIGNOBJECT
    TagCategories::SPECIAL,    // SVG_DESC
    TagCategories::SPECIAL,    // SVG_TITLE
    TagCategories::FORMATTING, // A
    TagCategories::FORMATTING, // B
    TagCategories::FORMATTING, // BIG
    TagCategories::FORMATTING, // CODE
    TagCategories::FORMATTING, // EM
    TagCategories::FORMATTING, // FONT
    TagCategories::FORMATTING, // I
    TagCategories::FORMATTING, // NOBR
    TagCategories::FORMATTING, // S
    TagCategories::FORMATTING, // SMALL
    TagCategories::FORMATTING, // STRIKE
    TagCategories::FORMATTING, // STRONG
    TagCategories::FORMATTING, // TT
    TagCategories::FORMATTING, // U
    TagCategories::ORDINARY,   // ABBR
    TagCategories::ORDINARY,   // AUDIO
    TagCategories::ORDINARY,   // BDI
    TagCategories::ORDINARY,   // BDO
    TagCategories::ORDINARY,   // CANVAS
    TagCategories::ORDINARY,   // CITE
    TagCategories::ORDINARY,   // DATA
    TagCategories::ORDINARY,   // DATALIST
    TagCategories::ORDINARY,   // DEL
    TagCategories::ORDINARY,   // DFN
    TagCategories::ORDINARY,   // DIALOG
    TagCategories::ORDINARY,   // INS
    TagCategories::ORDINARY,   // KBD
    TagCategories::ORDINARY,   // LABEL
    TagCategories::ORDINARY,   // LEGEND
    TagCategories::ORDINARY,   // MAP
    TagCategories::ORDINARY,   // MARK
    TagCategories::ORDINARY,   // MATHML_MATH
    TagCategories::ORDINARY,   // METER
    TagCategories::ORDINARY,   // OPTGROUP
    TagCategories::ORDINARY,   // OPTION
    TagCategories::ORDINARY,   // OUTPUT
    TagCategories::ORDINARY,   // PICTURE
    TagCategories::ORDINARY,   // PROGRESS
    TagCategories::ORDINARY,   // Q
    TagCategories::ORDINARY,   // RB
    TagCategories::ORDINARY,   // RP
    TagCategories::ORDINARY,   // RT
    TagCategories::ORDINARY,   // RTC
    TagCategories::ORDINARY,   // RUBY
    TagCategories::ORDINARY,   // SAMP
    TagCategories::ORDINARY,   // SLOT
    TagCategories::ORDINARY,   // SPAN
    TagCategories::ORDINARY,   // SUB
    TagCategories::ORDINARY,   // SUP
    TagCategories::ORDINARY,   // SVG_SVG
    TagCategories::ORDINARY,   // TIME
    TagCategories::ORDINARY,   // VAR
    TagCategories::ORDINARY,   // VIDEO
};

} // namespace html_parser
} // namespace eclair_html
