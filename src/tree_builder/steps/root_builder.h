#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_ROOT_BUILDER
#define ECLAIR_HTML_TREE_BUILDER_STEPS_ROOT_BUILDER

#include "common/error_manager.h"
#include "common/tokens.h"
#include "tree_builder/active_formatting_elements.h"
#include "tree_builder/adoption_agency.h"
#include "tree_builder/insertion_mode.h"
#include "tree_builder/node_inserter.h"
#include "tree_builder/open_elements.h"
#include "tree_builder/state.h"

namespace eclair_html {
namespace html_parser {
namespace steps {

  struct RootBuilder {
    explicit RootBuilder(Node& document, ErrorManager& errorManager)
      : errorManager(errorManager), state(document, openElements),
        insertionMode(openElements, state), nodeInserter(openElements, state),
        activeFormattingElements(openElements, nodeInserter),
        adoptionAgency(openElements, activeFormattingElements, nodeInserter,
            errorManager) {
    }

    virtual ~RootBuilder() {
    }

    virtual void reprocess(Token& token) = 0;
    virtual void reprocess(InsertionModes mode, Token& token) = 0;

    ErrorManager& errorManager;
    OpenElements openElements;
    State state;
    InsertionMode insertionMode;
    NodeInserter nodeInserter;
    ActiveFormattingElements activeFormattingElements;
    AdoptionAgency adoptionAgency;
  };

}
}
}

#endif
