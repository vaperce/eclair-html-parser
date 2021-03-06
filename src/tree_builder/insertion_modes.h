#ifndef ECLAIR_HTML_TREE_BUILDER_INSERTION_MODES
#define ECLAIR_HTML_TREE_BUILDER_INSERTION_MODES

namespace eclair_html {
namespace html_parser {

enum class InsertionModes {
  INITIAL = 0,
  BEFORE_HTML,
  BEFORE_HEAD,
  IN_HEAD,
  IN_HEAD_NOSCRIPT,
  AFTER_HEAD,
  IN_BODY,
  TEXT,
  IN_TABLE,
  IN_TABLE_TEXT,
  IN_CAPTION,
  IN_COLUMN_GROUP,
  IN_TABLE_BODY,
  IN_ROW,
  IN_CELL,
  IN_SELECT,
  IN_SELECT_IN_TABLE,
  IN_TEMPLATE,
  AFTER_BODY,
  IN_FRAMESET,
  AFTER_FRAMESET,
  AFTER_AFTER_BODY,
  AFTER_AFTER_FRAMESET,
  SIZE,
};

}
} // namespace eclair_html

#endif
