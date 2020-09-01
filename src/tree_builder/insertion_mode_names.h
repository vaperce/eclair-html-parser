#ifndef ECLAIR_HTML_TREE_BUILDER_INSERTION_MODE_NAMES
#define ECLAIR_HTML_TREE_BUILDER_INSERTION_MODE_NAMES

#include <string>

#include "tree_builder/insertion_modes.h"

namespace eclair_html {
namespace html_parser {

  struct InsertionModeNames {
  public:
    static std::string value(InsertionModes mode) {
      switch (mode) {
        case InsertionModes::INITIAL:
          return "INITIAL";
        case InsertionModes::BEFORE_HTML:
          return "BEFORE_HTML";
        case InsertionModes::BEFORE_HEAD:
          return "BEFORE_HEAD";
        case InsertionModes::IN_HEAD:
          return "IN_HEAD";
        case InsertionModes::IN_HEAD_NOSCRIPT:
          return "IN_HEAD_NOSCRIPT";
        case InsertionModes::AFTER_HEAD:
          return "AFTER_HEAD";
        case InsertionModes::IN_BODY:
          return "IN_BODY";
        case InsertionModes::TEXT:
          return "TEXT";
        case InsertionModes::IN_TABLE:
          return "IN_TABLE";
        case InsertionModes::IN_TABLE_TEXT:
          return "IN_TABLE_TEXT";
        case InsertionModes::IN_CAPTION:
          return "IN_CAPTION";
        case InsertionModes::IN_COLUMN_GROUP:
          return "IN_COLUMN_GROUP";
        case InsertionModes::IN_TABLE_BODY:
          return "IN_TABLE_BODY";
        case InsertionModes::IN_ROW:
          return "IN_ROW";
        case InsertionModes::IN_CELL:
          return "IN_CELL";
        case InsertionModes::IN_SELECT:
          return "IN_SELECT";
        case InsertionModes::IN_SELECT_IN_TABLE:
          return "IN_SELECT_IN_TABLE";
        case InsertionModes::IN_TEMPLATE:
          return "IN_TEMPLATE";
        case InsertionModes::AFTER_BODY:
          return "AFTER_BODY";
        case InsertionModes::IN_FRAMESET:
          return "IN_FRAMESET";
        case InsertionModes::AFTER_FRAMESET:
          return "AFTER_FRAMESET";
        case InsertionModes::AFTER_AFTER_BODY:
          return "AFTER_AFTER_BODY";
        case InsertionModes::AFTER_AFTER_FRAMESET:
          return "AFTER_AFTER_FRAMESET";
        default:
          return "<ERROR>";
      }
    }
  };

}
}

#endif
