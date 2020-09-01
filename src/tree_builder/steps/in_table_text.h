#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_IN_TABLE_TEXT
#define ECLAIR_HTML_TREE_BUILDER_STEPS_IN_TABLE_TEXT

#include "common/tokens.h"
#include "common/tools.h"
#include "tree_builder/steps/base_step.h"

namespace xyz {
namespace html_parser {
namespace steps {

  struct InTableText : public BaseStep {
    explicit InTableText(RootBuilder& rootBuilder)
      : BaseStep(rootBuilder) {
    }

    virtual ~InTableText() {
    }

    virtual void process(Token& token) {
      if (token.kind == TokenKinds::CHARACTER) {
        if (token.data[0] == '\0') {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TEXT, token);
        } else {
          _root.state.pendingTableTextTokens.push_back(std::move(token));
        }
      } else {
        bool errorChar = false;
        for (auto& charToken : _root.state.pendingTableTextTokens) {
          if (!Tools::isSpace(charToken.data[0])) {
            errorChar = true;
            break;
          }
        }

        if (errorChar) {
          _root.errorManager.add(ErrorKinds::UNEXPECTED_TEXT, token);
          _root.nodeInserter.enableFosterParenting();
          for (auto& charToken : _root.state.pendingTableTextTokens) {
            _root.reprocess(InsertionModes::IN_BODY, charToken);
          }
          _root.nodeInserter.disableFosterParenting();
        } else {
          for (auto& charToken : _root.state.pendingTableTextTokens) {
            _root.nodeInserter.insertCharacter(charToken);
          }
        }

        _root.insertionMode.restoreOriginal();
        _root.reprocess(token);
      }
    }
  };

}
}
}

#endif
