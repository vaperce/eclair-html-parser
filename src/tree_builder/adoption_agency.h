#ifndef ECLAIR_HTML_TREE_BUILDER_STEPS_ADOPTION_AGENCY
#define ECLAIR_HTML_TREE_BUILDER_STEPS_ADOPTION_AGENCY

#include "common/error_manager.h"
#include "common/tokens.h"
#include "tree_builder/active_formatting_elements.h"
#include "tree_builder/node_inserter.h"
#include "tree_builder/open_elements.h"

namespace eclair_html {
namespace html_parser {

  class AdoptionAgency {
  public:
    AdoptionAgency(OpenElements& openElements,
        ActiveFormattingElements& activeFormattingElements,
        NodeInserter& nodeInserter, ErrorManager& errorManager)
      : _openElements(openElements),
        _activeFormattingElements(activeFormattingElements),
        _nodeInserter(nodeInserter), _errorManager(errorManager) {
    }

    void run(Token& token) {
      if (!_runInternal(token)) {
        inBodyHandleAnyEndTag(token);
      }
    }

    void  inBodyHandleAnyEndTag(Token& token) {
      for (auto it = _openElements.begin();
          it != _openElements.end(); ++it) {
        if ((*it)->name() == token.name) {
          _openElements.popImplied(token.name);
          if (_openElements.top().name() != token.name) {
            _errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          }
          _openElements.popUntil(token.name);
          break;
        }

        if ((*it)->name().category() == TagCategories::SPECIAL) {
          _errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          break;
        }
      }
    }

  private:
    /*
     * OE = Open Elements
     * AFE = Active Formatting Elements
     * fE = Formatting Element
     * fB = Furthest Block
     * cA = Common Ancestor
     */
    bool _runInternal(Token& token) {
      // 1
      auto subject = token.name;

      // 2
      if (_openElements.top().name() == subject &&
          !_activeFormattingElements.contains(_openElements.top())) {
        _openElements.pop();
        return true;
      }

      // 3, 4, 5, 21
      for (int i = 0; i < MAX_ITER; ++i) {
        // 6
        auto fEInAFE = _formattingElement(subject);
        if (fEInAFE == _activeFormattingElements.end()) {
          return false;
        }
        Node& fE = *fEInAFE->element;

        // 7
        OpenElements::It fEInOE = _openElements.find(fE);
        if (fEInOE == _openElements.end()) {
          _activeFormattingElements.erase(fEInAFE);
          _errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          return true;
        }

        // 8
        if (!_openElements.hasElementInScope(ScopeKinds::STANDARD, fE)) {
          _errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
          return true;
        }

        // 9
        if (fEInOE != _openElements.begin()) {
          _errorManager.add(ErrorKinds::UNEXPECTED_TAG, token);
        }

        // 10
        auto fBInOE = _furthestBlock(fEInOE);

        // 11
        if (fBInOE == _openElements.end()) {
          _openElements.popUntil(fEInOE);
          _activeFormattingElements.erase(fEInAFE);
          return true;
        }
        Node& fB = **fBInOE;

        // 12
        auto cA = *std::next(fEInOE);

        // 13
        auto bookmark = fEInAFE;

        // 14.1, 14.2, 14.4, 14.11
        auto nodeInOE = fBInOE;
        Node::Ptr lastNode = (*nodeInOE)->remove();
        ++nodeInOE; // 14.3
        for (int j = 0; *nodeInOE != &fE; ++j) {
          auto nodeInAFE = _activeFormattingElements.find(**nodeInOE);

          // 14.5
          if (nodeInAFE != _activeFormattingElements.end() && j > 3) {
            if (bookmark == nodeInAFE) {
              bookmark = std::prev(bookmark);
            }
            _activeFormattingElements.erase(nodeInAFE);
            nodeInAFE = _activeFormattingElements.end();
          }

          // 14.6
          if (nodeInAFE == _activeFormattingElements.end()) {
            nodeInOE = _openElements.erase(nodeInOE); // 14.3
            continue;
          }

          // 14.7
          Node::Ptr newElement = (*nodeInOE)->copy();
          *nodeInOE = newElement.get();
          nodeInAFE->element = newElement.get();

          // 14.8
          if (lastNode.get() == &fB) {
            bookmark = std::prev(nodeInAFE);
          }

          // 14.9
          newElement->pushBack(std::move(lastNode));

          // 14.10
          lastNode = std::move(newElement);

          // 14.3
          ++nodeInOE;
        }

        // 15
        _nodeInserter.insertElement(std::move(lastNode), cA);

        // 16
        Node::Ptr newFE = fE.copy();

        // 17
        fB.moveChildren(*newFE);

        // 19
        _activeFormattingElements.insert(bookmark, *newFE);
        _activeFormattingElements.erase(fEInAFE);

        // 20
        _openElements.remove(fE);
        _openElements.insert(std::prev(fBInOE), *newFE);

        // 18
        fB.pushBack(std::move(newFE));
      }

      return true;
    }

    ActiveFormattingElements::It _formattingElement(const TagName& name) {
      for (auto it = _activeFormattingElements.begin();
          it != _activeFormattingElements.end(); ++it) {
        if (it->marker()) {
          return _activeFormattingElements.end();
        }
        if (it->element->name() == name) {
          return it;
        }
      }

      return _activeFormattingElements.end();
    }

    OpenElements::It _furthestBlock(OpenElements::It fEInOE) {
      if (fEInOE == _openElements.begin()) {
        return _openElements.end();
      }

      for (auto it = std::prev(fEInOE); it != _openElements.begin(); --it) {
        if ((*it)->name().category() == TagCategories::SPECIAL) {
          return it;
        }
      }
      if (_openElements.top().name().category() == TagCategories::SPECIAL) {
        return _openElements.begin();
      }

      return _openElements.end();
    }

    const int MAX_ITER = 8;

    OpenElements& _openElements;
    ActiveFormattingElements& _activeFormattingElements;
    NodeInserter& _nodeInserter;
    ErrorManager& _errorManager;
  };

}
}

#endif
