#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_TEXT
#define ECLAIR_HTML_TREE_BUILDER_STEPS_TEXT

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace eclair_html {
namespace html_parser {
namespace steps {

struct Text : public BaseStep {
  explicit Text(RootBuilder& rootBuilder) : BaseStep(rootBuilder) {}

  virtual ~Text() {}

  virtual void process(Token& token) {
    if (token.kind == TokenKinds::CHARACTER) {
      _root.nodeInserter.insertCharacter(token);
    } else if (token.kind == TokenKinds::END_OF_FILE) {
      _root.openElements.pop();
      _root.insertionMode.restoreOriginal();
      _root.reprocess(token);
    } else if (token.kind == TokenKinds::END_TAG) {
      _root.openElements.pop();
      _root.insertionMode.restoreOriginal();
    }
  }
};

} // namespace steps
} // namespace html_parser
} // namespace eclair_html

#endif
