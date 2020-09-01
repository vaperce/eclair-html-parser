#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_INITIAL
#define ECLAIR_HTML_TREE_BUILDER_STEPS_INITIAL

#include "common/tokens.h"
#include "tree_builder/steps/base_step.h"

namespace eclair_html {
namespace html_parser {
namespace steps {

struct Initial : public BaseStep {
  explicit Initial(RootBuilder& rootBuilder) : BaseStep(rootBuilder) {}

  virtual ~Initial() {}

  virtual void process(Token& token) {
    if (_isSpace(token)) {
      return;
    }
    if (token.kind == TokenKinds::COMMENT) {
      _root.nodeInserter.insertCommentInDocument(token);
      return;
    }
    if (token.kind == TokenKinds::DOC_TYPE) {
      // TODO: handle properly
      _root.insertionMode.set(InsertionModes::BEFORE_HTML);
      return;
    }

    // FIXME: iframe srcdoc case
    _root.insertionMode.set(InsertionModes::BEFORE_HTML);
    _root.reprocess(token);
  }
};

} // namespace steps
} // namespace html_parser
} // namespace eclair_html

#endif
