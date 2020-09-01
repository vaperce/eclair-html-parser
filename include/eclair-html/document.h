#ifndef ECLAIR_HTML_DOCUMENT
#define ECLAIR_HTML_DOCUMENT

#include <memory>
#include <string>
#include <vector>

#include "eclair-html/errors.h"
#include "eclair-html/node.h"

// Last updated to follow Whatwg spec at commit:
// 1e920001839f255bf5ea3ba687b74457188246ed
// on 12/11/2019

namespace eclair_html {
namespace html_parser {

enum class CleanFlags : char {
  SPACE = 0x1,
  SCRIPT = 0x2,
  STYLE = 0x4,
  LINK = 0x8,
  COMMENT = 0x10,
};

class Document {
public:
  explicit Document() : _root(Node::newDocument()) {}

  bool parse(const char* data, std::size_t length, const char* charset,
             Errors* errors);
  bool parse(const char* data, std::size_t length, const char* httpUrl,
             const char* httpCharset, Errors* errors);

  void clean(char flags);

  void titleText(std::u16string& value) const;
  void contentText(std::vector<std::u16string>& values) const;

  Node& root() { return *_root; }
  const Node& root() const { return *_root; }

  std::u16string render(bool prettyPrint, const char* forceCharset) const;

private:
  Node::Ptr _root;
};

} // namespace html_parser
} // namespace eclair_html

#endif
