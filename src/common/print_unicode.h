#ifndef ECLAIR_HTML_COMMON_PRINT_UNICODE
#define ECLAIR_HTML_COMMON_PRINT_UNICODE

#include <iostream>
#include <memory>
#include <string>

#include "unicode/ustring.h"

std::ostream& operator<<(std::ostream& stream, const std::u16string& str) {
  if (str.empty()) {
    return stream;
  }

  unsigned size = str.size() * 2 + 1;
  std::unique_ptr<char[]> buffer(new char[size]);

  UErrorCode status = U_ZERO_ERROR;
  int length = 0;
  u_strToUTF8(buffer.get(), size, &length, str.c_str(), str.size(), &status);

  if (status == U_ZERO_ERROR) {
    stream << buffer.get();
  }

  return stream;
}

#endif
