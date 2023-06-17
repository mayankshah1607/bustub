#include "primer/trie.h"
#include <memory>
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
    if (root_ == nullptr) {
        return nullptr;
    }

    if (key.empty()) {
        key = " ";
    }

    const TrieNode* next = this->root_.get();

    for (char const &c: key) {
        if (next->children_.find(c) == next->children_.end()) {
            return nullptr;
        }
        next = next->children_.at(c).get();
    }

    auto value_node = dynamic_cast<const TrieNodeWithValue<T> *>(next);

    if (value_node == nullptr) {
        return nullptr;
    }
    return value_node->value_.get();
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {

    Trie clone;
    if (root_ == nullptr) {
        auto children = std::map<char, std::shared_ptr<const TrieNode>>();
        clone = Trie(std::make_shared<const TrieNode>(children));
    } else {
        clone = this->Clone();
    }

    if (key.empty()) {
        key = " ";
    }


    auto next = const_cast<TrieNode*>(clone.root_.get());
    TrieNode* parent = nullptr;

    for (char const &c: key) {
        if (next->children_.find(c) == next->children_.end()) {
            next->children_.insert({c, std::make_shared<const TrieNode>()});
        }
        parent = next;
        next = const_cast<TrieNode*>(next->children_.at(c).get());
    }

    auto value_node = dynamic_cast<TrieNodeWithValue<T>*>(next);
    if (value_node == nullptr) { // No value present here.
        auto new_value_node = std::make_shared<TrieNodeWithValue<T>>(next->children_, std::make_shared<T>(value));
        parent->children_[key.back()] = new_value_node;
    } else {
        value_node->value_ = std::make_shared<T>(value);
    }

    return clone;
}

auto Trie::Remove(std::string_view key) const -> Trie {
    if (root_ == nullptr) {
        return *this;
    }

    auto clone = this->Clone();
    auto next = const_cast<TrieNode*>(clone.root_.get());
    TrieNode* parent = nullptr;

    for (char const &c: key) {
        if (next->children_.find(c) == next->children_.end()) {
            return *this;
        }
        parent = next;
        next = const_cast<TrieNode*>(next->children_.at(c).get());
    }
    
    auto new_node = std::make_shared<TrieNode>(next->children_);
    parent->children_[key.back()] = new_node;
    return clone;
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

/* using Integer = std::unique_ptr<uint32_t>; */

/* template auto Trie::Put(std::string_view key, Integer value) const -> Trie; */
/* template auto Trie::Get(std::string_view key) const -> const Integer *; */

/* template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie; */
/* template auto Trie::Get(std::string_view key) const -> const MoveBlocked *; */

}  // namespace bustub
