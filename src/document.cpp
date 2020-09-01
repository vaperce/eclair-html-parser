#include "cleaner.h"
#include "eclair-html/document.h"
#include "parser.h"
#include "renderer.h"
#include "text_extractor.h"

namespace eclair_html {
namespace html_parser {

bool Document::parse(const char* data, std::size_t length, const char* charset,
    Errors* errors) {
  _root = Node::newDocument();
  return Parser(*_root, data, length, errors).parse(charset);
}

bool Document::parse(const char* data, std::size_t length, const char* httpUrl,
        const char* httpCharset, Errors* errors) {
  _root = Node::newDocument();
  return Parser(*_root, data, length, errors).parse(httpUrl, httpCharset);
}

void Document::clean(char flags) {
  if (_root) {
    Cleaner(*_root, flags).process();
  }
}

void Document::titleText(std::u16string& value) const {
  if (_root) {
    TextExtractor(*_root).processTitle(value);
  }
}

void Document::contentText(std::vector<std::u16string>& values) const {
  if (_root) {
    TextExtractor(*_root).processContent(values);
  }
}

std::u16string Document::render(bool prettyPrint,
    const char* forceCharset) const {
  if (_root) {
    return Renderer(*_root, prettyPrint, forceCharset).render();
  }
  return u"";
}

}
}
