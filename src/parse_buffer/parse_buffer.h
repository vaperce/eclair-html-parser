#ifndef ECLAIR_HTML_PARSE_BUFFER_PARSE_BUFFER
#define ECLAIR_HTML_PARSE_BUFFER_PARSE_BUFFER

#include <iostream>
#include <memory>
#include <string>

#include "compact_enc_det/compact_enc_det.h"
#include "unicode/ucnv.h"

#include "common/tools.h"
#include "parse_buffer/meta_charset_parser.h"

namespace eclair_html {
namespace html_parser {

  class ParseBuffer {
  public:
    ParseBuffer(const char* data, std::size_t length)
      : _data(data), _length(length) {
    }

    bool make(const char* charset) {
      return _convertData(charset);
    }

    bool make(const char* url, const char* httpCharset) {
      MetaCharsetParser parser(_data, _length);
      auto metaCharset = parser.parse();

      int bytesConsumed = 0;
      bool isReliable = false;
      Encoding encoding = CompactEncDet::DetectEncoding(_data, _length, url,
          httpCharset, metaCharset.c_str(), UNKNOWN_ENCODING, UNKNOWN_LANGUAGE,
          CompactEncDet::WEB_CORPUS, true, &bytesConsumed, &isReliable);

      _log(url, httpCharset, metaCharset, encoding, isReliable);

      if (!isReliable) {
        // TODO
      }
      if (encoding == UNKNOWN_ENCODING) {
        return false;
      }

      return _convertData(MimeEncodingName(encoding));
    }

    std::u16string content;

  private:
    inline void _log(const char* url, const char* httpCharset,
        const std::string& metaCharset, Encoding encoding, bool reliable) {
#ifdef XYZ_PARSER_DEBUG_LOGS
      std::cerr << "[DEBUG] FOUND ENCODING '";
      std::cerr << MimeEncodingName(encoding);
      std::cerr << "'";
      if (!reliable) {
        std::cerr << " (unreliable)";
      }

      std::cerr << " WITH URL: '";
      if (url) {
        std::cerr << url;
      }
      std::cerr << "', HTTP_CHARSET: '";
      if (httpCharset) {
        std::cerr << httpCharset;
      }
      std::cerr << "' AND META_CHARSET: '";
      std::cerr << metaCharset;
      std::cerr << "'" << std::endl;
#endif
    }

    void _addToContent(UChar32 code) {
      char16_t code1, code2;
      if (Tools::utf32ToUtf16(code, code1, code2)) {
        content.push_back(code1);
        if (code2) {
          content.push_back(code2);
        }
      }
    }

    bool _convertData(const char* charset) {
      UErrorCode status = U_ZERO_ERROR;

      UConverter *conv = ucnv_open(charset, &status);
      if (status != U_ZERO_ERROR) {
        return false;
      }

      const char* sourcePtr = _data;
      const char* sourceLimit = _data + _length;
      while (true) {
        UChar32 code = ucnv_getNextUChar(conv, &sourcePtr, sourceLimit,
            &status);
        if (status == U_ZERO_ERROR) {
          _addToContent(code);
        } else {
          break;
        }
      }

      ucnv_close(conv);
      if (status == U_INDEX_OUTOFBOUNDS_ERROR) {
        return true;
      }

      return false;
    }

    const char* _data;
    std::size_t _length;
  };

}
}

#endif
