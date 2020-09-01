#ifndef ECLAIR_HTML_COMMON_TOKENS
#define ECLAIR_HTML_COMMON_TOKENS

#include <iostream>
#include <memory>
#include <string>

#include "common/position.h"
#include "common/print_unicode.h"
#include "eclair-html/node.h"
#include "eclair-html/tag_names.h"

namespace eclair_html {
namespace html_parser {

enum class TokenKinds {
  DOC_TYPE,
  START_TAG,
  END_TAG,
  COMMENT,
  CHARACTER,
  END_OF_FILE,
};

struct DocTypeInfo {
  typedef std::unique_ptr<const DocTypeInfo> Ptr;

  DocTypeInfo() : forceQuirks(false) {}

  std::u16string name;
  std::unique_ptr<std::u16string> publicIdentifier;
  std::unique_ptr<std::u16string> systemIdentifier;
  bool forceQuirks;
};

class Token {
public:
  explicit Token(const Position& position, DocTypeInfo::Ptr docTypeInfo)
      : kind(TokenKinds::DOC_TYPE), docTypeInfo(std::move(docTypeInfo)),
        _position(position) {}
  explicit Token(const Position& position, const std::u16string& name,
                 Node::Attributes& attributes, bool selfClosing)
      : kind(TokenKinds::START_TAG), name(name), selfClosing(selfClosing),
        _attributes(std::move(attributes)), _position(position) {}
  explicit Token(const Position& position, const std::u16string& name)
      : kind(TokenKinds::END_TAG), name(name), _position(position) {}
  explicit Token(const std::u16string& data, const Position& position)
      : kind(TokenKinds::COMMENT), data(data), _position(position) {}
  explicit Token(const Position& position, char16_t c)
      : kind(TokenKinds::CHARACTER), data(1, c), _position(position) {}
  explicit Token(const Position& position)
      : kind(TokenKinds::END_OF_FILE), _position(position) {}
  explicit Token(const Token& other) = delete;
  explicit Token(Token&& other) = default;

  const TokenKinds kind;
  const Position& position() const { return _position; }

  TagName name;
  const Node::Attributes& attributes() const { return _attributes; }
  bool selfClosing;

  const std::u16string data;

  DocTypeInfo::Ptr docTypeInfo;

private:
  Node::Attributes _attributes;
  const Position _position;
};

} // namespace html_parser
} // namespace eclair_html

std::ostream& operator<<(std::ostream& stream,
                         const eclair_html::html_parser::Token& token) {
  switch (token.kind) {
  case eclair_html::html_parser::TokenKinds::DOC_TYPE:
    stream << "DOC_TYPE";
    break;
  case eclair_html::html_parser::TokenKinds::START_TAG:
    stream << "START_TAG(" << token.name.value() << ")";
    break;
  case eclair_html::html_parser::TokenKinds::END_TAG:
    stream << "END_TAG(" << token.name.value() << ")";
    break;
  case eclair_html::html_parser::TokenKinds::CHARACTER:
    stream << "CHARACTER(" << token.data << ")";
    break;
  case eclair_html::html_parser::TokenKinds::COMMENT:
    stream << "COMMENT";
    break;
  case eclair_html::html_parser::TokenKinds::END_OF_FILE:
    stream << "END_OF_FILE";
    break;
  }

  return stream;
}

#endif
