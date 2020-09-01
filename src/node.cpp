#include "eclair-html/node.h"
#include "text_extractor.h"

namespace xyz {
namespace html_parser {

const std::u16string Node::textContent() const {
  std::u16string res;
  TextExtractor(*this).processSubContent(res);
  return res;
}

}
}
