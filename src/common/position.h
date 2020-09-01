#ifndef ECLAIR_HTML_COMMON_POSITION
#define ECLAIR_HTML_COMMON_POSITION

namespace xyz {
namespace html_parser {

  struct Position {
    Position ()
      : line(1), lineOffset(0) {
    }

    Position (unsigned line, unsigned lineOffset)
      : line(line), lineOffset(lineOffset) {
    }

    unsigned line;
    unsigned lineOffset;
  };

}
}

#endif
