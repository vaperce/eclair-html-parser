#ifndef ECLAIR_HTML_NODE
#define ECLAIR_HTML_NODE

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include "eclair-html/tag_names.h"

namespace eclair_html {
namespace html_parser {

  class Inserter;

  enum class NodeKinds {
    DOCUMENT = 0,
    ELEMENT,
    TEXT,
    COMMENT,
  };

  class Node {
  public:
    typedef std::unique_ptr<Node> Ptr;
    typedef std::list<std::unique_ptr<Node>> Children;
    typedef Children::iterator ChildrenIt;
    typedef Children::const_iterator ConstChildrenIt;

    Node(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(const Node&) = delete;

    static Ptr newDocument() {
      return Ptr(new Node());
    }

    static Ptr newElement(const TagName& name) {
      return Ptr(new Node(name));
    }

    static Ptr newText(const std::u16string& value) {
      return Ptr(new Node(NodeKinds::TEXT, value));
    }

    static Ptr newComment(const std::u16string& value) {
      return Ptr(new Node(NodeKinds::COMMENT, value));
    }

    NodeKinds kind() const {
      return _kind;
    }

    const TagName& name() const {
      return _name;
    }

    Node* parent() const {
      return _parent;
    }

    const std::u16string& content() const {
      return _content;
    }

    const std::u16string textContent() const;

    void appendContent(const std::u16string& content) {
      if (_kind == NodeKinds::TEXT) {
        _content += content;
      }
    }

    void setContent(const std::u16string& content) {
      if (_kind == NodeKinds::TEXT) {
        _content = content;
      }
    }

    typedef std::unordered_map<std::u16string, std::u16string> Attributes;
    Attributes attributes;

    Ptr copy() const {
      Ptr res;

      switch (_kind) {
        case NodeKinds::ELEMENT:
          res = Node::newElement(_name);
          res->attributes.insert(attributes.begin(), attributes.end());
          return res;

        case NodeKinds::TEXT:
          return Node::newText(_content);

        case NodeKinds::COMMENT:
          return Node::newComment(_content);

        default:
          return nullptr;

      }
    }

    ConstChildrenIt begin() const {
      return _children.begin();
    }

    ConstChildrenIt end() const {
      return _children.end();
    }

    Node& front() const {
      return *_children.front();
    }

    Node& back() const {
      return *_children.back();
    }

    bool empty() const {
      return _children.empty();
    }

    std::size_t size() const {
      return _children.size();
    }

    void pushBack(Ptr node) {
      auto parent = node->parent();
      if (parent) {
        parent->erase(*node);
      }
      node->_parent = this;
      _children.push_back(std::move(node));
    }

    void insert(ChildrenIt where, Ptr node) {
      auto parent = node->parent();
      if (parent) {
        parent->erase(*node);
      }
      node->_parent = this;
      _children.insert(where, std::move(node));
    }

    void moveChildren(Node& node) {
      for (auto&& child : _children) {
        child->_parent = &node;
        node._children.push_back(std::move(child));
      }
      _children.clear();
    }

    Node::Ptr erase(const Node& node) {
      for (auto child = _children.begin(); child != _children.end(); ++child) {
        if (child->get() == &node) {
          return _erase(child);
        }
      }
      return nullptr;
    }

    void erase(ConstChildrenIt first, ConstChildrenIt last) {
      for (auto child = first; child != last; ++child) {
        (*child)->_parent = nullptr;
      }
      _children.erase(first, last);
    }

    Node::Ptr remove() {
      if (_parent) {
        return _parent->erase(*this);
      }
      return nullptr;
    }

    void clear() {
      for (auto& child : _children) {
        child->_parent = nullptr;
      }
      _children.clear();
    }

  private:
    explicit Node()
      : _kind(NodeKinds::DOCUMENT), _parent(nullptr) {
    }

    explicit Node(const TagName& name)
      : _kind(NodeKinds::ELEMENT), _parent(nullptr), _name(name) {
    }

    explicit Node(NodeKinds kind, const std::u16string& content)
      : _kind(kind), _parent(nullptr), _content(content) {
    }

    Node::Ptr _erase(ChildrenIt it) {
      if (it != _children.end()) {
        (*it)->_parent = nullptr;
        auto res = std::move(*it);
        _children.erase(it);
        return res;
      }
      return nullptr;
    }

    NodeKinds _kind;
    Node* _parent;
    TagName _name;
    std::u16string _content;

    Children _children;
  };

}
}

#endif
