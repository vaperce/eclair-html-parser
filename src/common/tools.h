#ifndef ECLAIR_HTML_COMMON_TOOLS
#define ECLAIR_HTML_COMMON_TOOLS

#include <algorithm>
#include <string>

namespace eclair_html {
namespace html_parser {
namespace Tools {

bool isSpace(char16_t c) {
  return (c == ' ' || c == '\t' || c == '\f' || c == '\r' || c == '\n');
}

char16_t toLowercase(char16_t c) {
  if (c >= 'A' && c <= 'Z') {
    return c + 0x20;
  }
  return c;
}

std::u16string toLowercaseStr(const std::u16string& str) {
  std::u16string res;
  res.resize(str.size());
  std::transform(str.begin(), str.end(), res.begin(), toLowercase);
  return res;
}

bool utf32ToUtf16(char32_t in, char16_t& out1, char16_t& out2) {
  // FROM ICU (Utf32 -> Utf16)
  if (in <= 0xffff) {
    out1 = in;
    out2 = 0;
    return true;
  }
  if (in <= 0x10ffff) {
    out1 = (in >> 10) + 0xd7c0;
    out2 = (in & 0x3ff) | 0xdc00;
    return true;
  }
  return false;
}

} // namespace Tools
} // namespace html_parser
} // namespace eclair_html

#endif
