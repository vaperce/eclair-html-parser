#ifndef ECLAIR_HTML_TOKENIZER_ENTITIES
#define ECLAIR_HTML_TOKENIZER_ENTITIES

#include <string>
#include <vector>

namespace xyz {
namespace html_parser {

  class Entries;

  struct _EntityEntry {
    const std::string name;
    const char16_t code1;
    const char16_t code2;
  };

  typedef std::vector<_EntityEntry> EntityEntries;

  class EntityMatcher {
  public:
    enum Enum {
      FOUND,
      CONTINUE,
      STOP,
    };

    Enum match(char c) {
      if (_foundBegin == _end) {
        return STOP;
      }

      EntityEntries::const_iterator itFound = _end;
      for (auto it = _foundBegin; it != _foundEnd; ++it) {
        if (it->name.size() > _offset && it->name[_offset] == c) {
          if (itFound == _end) {
            _foundBegin = it;
          }
          if (it->name.size() == _offset + 1) {
            _found = &(*it);
          }
          itFound = it;
        }
      }

      _foundEnd = itFound + 1;
      _offset++;

      if (itFound == _end) {
        _foundBegin = _end;
        if (_found) {
          return FOUND;
        }
        return STOP;
      }

      if (_foundBegin == _foundEnd - 1) {
        if (_found) {
          return FOUND;
        }
        if (_offset >= _foundBegin->name.size()) {
          return STOP;
        }
      }
      return CONTINUE;
    }

    void setFound(char16_t& code1, char16_t& code2) const {
      if (_found) {
        code1 = _found->code1;
        code2 = _found->code2;
      }
    }

  private:
    explicit EntityMatcher(const EntityEntries& entries)
      : _foundBegin(entries.begin()), _foundEnd(entries.end()),
        _end(entries.end()), _offset(0), _found(nullptr) {
    }

    EntityEntries::const_iterator _foundBegin;
    EntityEntries::const_iterator _foundEnd;
    EntityEntries::const_iterator _end;
    unsigned _offset;

    const _EntityEntry* _found;
    friend class Entities;
  };

  class Entities {
  public:
    static EntityMatcher matcher() {
      return EntityMatcher(_ENTITY_MAP);
    }

  private:
    static const EntityEntries _ENTITY_MAP;
  };

}
}

#endif
