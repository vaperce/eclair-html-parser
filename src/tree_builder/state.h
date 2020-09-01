#ifndef ECLAIR_HTML_TREE_BUILDER_STATE
#define ECLAIR_HTML_TREE_BUILDER_STATE

#include <memory>
#include <string>

#include "common/tokens.h"
#include "eclair-html/node.h"
#include "tokenizer/config.h"
#include "tree_builder/open_elements.h"

namespace eclair_html {
namespace html_parser {

class State : public TokenizerConfig {
public:
  State(Node& document, const OpenElements& openElements)
      : document(document), head(nullptr), form(nullptr), scriptEnabled(false),
        framesetOk(true), ignoreNextLineFeed(false),
        _openElements(openElements) {}
  virtual ~State() {}

  virtual bool isHTMLNamespace() const {
    if (_openElements.empty()) {
      return true;
    }
    return _openElements.top().name().nameSpace() == Namespaces::HTML;
  }

  Node& document;
  Node* head;
  Node* form;

  bool scriptEnabled;
  bool framesetOk;
  bool ignoreNextLineFeed;

  std::vector<Token> pendingTableTextTokens;

private:
  const OpenElements& _openElements;
};

} // namespace html_parser
} // namespace eclair_html

#endif
