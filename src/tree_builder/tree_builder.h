#ifndef ECLAIR_HTML_TREE_BUILDER_TREE_BUILDER
#define ECLAIR_HTML_TREE_BUILDER_TREE_BUILDER

#include <iostream>
#include <memory>
#include <vector>

#include "common/error_manager.h"
#include "common/token_receiver.h"
#include "common/tokens.h"
#include "common/tools.h"
#include "tokenizer/config.h"
#include "tree_builder/insertion_mode.h"
#include "tree_builder/insertion_mode_names.h"
#include "tree_builder/steps/root_builder.h"
#include "tree_builder/steps/steps.h"

namespace eclair_html {
namespace html_parser {

  class TreeBuilder : public steps::RootBuilder, public TokenReceiver {
  public:
    explicit TreeBuilder(Node& document, ErrorManager& errorManager)
      : RootBuilder(document, errorManager), _foreignContent(*this) {
      _steps.resize(static_cast<int>(InsertionModes::SIZE));

      _setMode<InsertionModes::INITIAL, steps::Initial>();
      _setMode<InsertionModes::BEFORE_HTML, steps::BeforeHtml>();
      _setMode<InsertionModes::BEFORE_HEAD, steps::BeforeHead>();
      _setMode<InsertionModes::IN_HEAD, steps::InHead>();
      _setMode<InsertionModes::IN_HEAD_NOSCRIPT, steps::InHeadNoscript>();
      _setMode<InsertionModes::AFTER_HEAD, steps::AfterHead>();
      _setMode<InsertionModes::IN_BODY, steps::InBody>();
      _setMode<InsertionModes::TEXT, steps::Text>();
      _setMode<InsertionModes::IN_TABLE, steps::InTable>();
      _setMode<InsertionModes::IN_TABLE_TEXT, steps::InTableText>();
      _setMode<InsertionModes::IN_CAPTION, steps::InCaption>();
      _setMode<InsertionModes::IN_COLUMN_GROUP, steps::InColumnGroup>();
      _setMode<InsertionModes::IN_TABLE_BODY, steps::InTableBody>();
      _setMode<InsertionModes::IN_ROW, steps::InRow>();
      _setMode<InsertionModes::IN_CELL, steps::InCell>();
      _setMode<InsertionModes::IN_SELECT, steps::InSelect>();
      _setMode<InsertionModes::IN_SELECT_IN_TABLE, steps::InSelectInTable>();
      _setMode<InsertionModes::IN_TEMPLATE, steps::InTemplate>();
      _setMode<InsertionModes::AFTER_BODY, steps::AfterBody>();
      _setMode<InsertionModes::IN_FRAMESET, steps::InFrameset>();
      _setMode<InsertionModes::AFTER_FRAMESET, steps::AfterFrameset>();
      _setMode<InsertionModes::AFTER_AFTER_BODY, steps::AfterAfterBody>();
      _setMode<InsertionModes::AFTER_AFTER_FRAMESET,
        steps::AfterAfterFrameset>();
    }
    virtual ~TreeBuilder() {
    }

    virtual void process(Token& token) {
      if (_checkLineFeed(token)) {
        auto mode = insertionMode.get();

        if (_isHtmlContent(token)) {
          _log(token, mode, false, false);
          _processSpecificHtml(mode, token);
        } else {
          _log(token, mode, true, false);
          _foreignContent.process(token);
        }
      }
    }

    TokenizerConfig& tokenizerConfig() {
      return state;
    }

  private:
    virtual void reprocess(Token& token) {
      reprocess(insertionMode.get(), token);
    }

    virtual void reprocess(InsertionModes mode, Token& token) {
      if (_checkLineFeed(token)) {
        _log(token, mode, false, true);
        _processSpecificHtml(mode, token);
      }
    }

    void _processSpecificHtml(InsertionModes mode, Token& token) {
      _steps[static_cast<int>(mode)]->process(token);
    }

    bool _checkLineFeed(Token& token) {
      if (state.ignoreNextLineFeed) {
        if (token.kind == TokenKinds::CHARACTER && token.data[0] == '\n') {
          return false;
        }
        state.ignoreNextLineFeed = false;
      }
      return true;
    }

    inline void _log(Token& token, InsertionModes mode, bool foreignContent,
        bool reprocess) {
#ifdef XYZ_PARSER_DEBUG_LOGS
      std::cerr << "[DEBUG] ";
      if (reprocess) {
        std::cerr << "RE-PROCESSING";
      } else {
        std::cerr << "PROCESSING";
      }
      std::cerr << " TOKEN " << token << " IN MODE ";
      if (foreignContent) {
        std::cerr << "FOREIGN_CONTENT";
      } else {
        std::cerr << InsertionModeNames::value(mode);
      }
      std::cerr << std::endl;
#endif
    }

    bool _isHtmlContent(Token& token) {
      if (openElements.empty()) {
        return true;
      }

      // FIXME fragment
      Node& currentNode = openElements.top();
      if (currentNode.name().nameSpace() == Namespaces::HTML) {
        return true;
      }

      bool isMathML = steps::ForeignContent::isMathMLIntegrationPoint(
          currentNode);
      if (isMathML && token.kind == TokenKinds::START_TAG &&
          token.name.value() != u"mglyph" &&
          token.name.value() != u"malignmark") {
        return true;
      }
      if (isMathML && token.kind == TokenKinds::CHARACTER) {
        return true;
      }
      if (currentNode.name() == TagKinds::MATHML_ANNOTATION_XML &&
          token.kind == TokenKinds::START_TAG &&
          token.name.value() == u"svg") {
        return true;
      }

      bool isHTML = steps::ForeignContent::isHTMLIntegrationPoint(currentNode);
      if (isHTML && token.kind == TokenKinds::START_TAG) {
        return true;
      }
      if (isHTML && token.kind == TokenKinds::CHARACTER) {
        return true;
      }

      if (token.kind == TokenKinds::END_OF_FILE) {
        return true;
      }

      return false;
    }

    template <InsertionModes MODE, class TYPE>
    inline void _setMode() {
      _steps[static_cast<int>(MODE)].reset(new TYPE(*this));
    }

    std::vector<std::unique_ptr<TokenReceiver>> _steps;
    steps::ForeignContent _foreignContent;
  };

}
}

#endif
