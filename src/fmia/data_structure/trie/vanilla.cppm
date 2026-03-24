// Copyright (C) 2026 atyxeut
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <https://www.gnu.org/licenses/>.

export module fmia.data_structure.trie.vanilla;

import std;

import fmia.math.integer.core;
import fmia.memory.core;
import fmia.util.std_extension.meta;

namespace fmia {

namespace detail {

template <typename Char, typename HashMap, exception_safety E>
class trie_base
{
private:
  struct node_
  {
    usize pass = 0; // count of strings that pass this node
    usize end = 0;  // count of strings that end with this node
    typename HashMap::template type<node_*> next;
  };

  node_* root_ = nullptr;

public:
  constexpr trie_base() noexcept = default;

  constexpr trie_base(const trie_base& other)
  {
    try
    {
      copy_tree_(other.root_, root_);
    }
    catch (...)
    {
      destroy_tree_(root_);
      throw;
    }
  };

  constexpr trie_base& operator =(const trie_base& other)
  {
    if (this != std::addressof(other))
      *this = trie_base(other);
    return *this;
  };

  constexpr trie_base(trie_base&& other) noexcept : root_ {std::exchange(other.root_, nullptr)} {};

  constexpr trie_base& operator =(trie_base&& other) noexcept
  {
    if (this != std::addressof(other))
    {
      destroy_tree_(root_);
      root_ = std::exchange(other.root_, nullptr);
    }
    return *this;
  };

private:
  constexpr void copy_tree_(node_* from, node_*& to)
  {
    if (!from)
    {
      if (to)
      {
        destroy_tree_(to);
        to = nullptr;
      }
      return;
    }

    to = new node_;
    to->pass = from->pass;
    to->end = from->end;
    from->next
      .for_each_invoke([this](node_* child_from, node_*& child_to) { copy_tree_(child_from, child_to); }, to->next);
  }

  constexpr void destroy_tree_recursive_(node_* root) noexcept
  {
    if (!root)
      return;

    root->next.for_each_invoke([this](node_* child) { destroy_tree_recursive_(child); });
    delete root;
  }

  // pending update: using a custom stack instead of std::vector
  constexpr void destroy_tree_iterative_(node_* root) noexcept
  {
    if (!root)
      return;

    std::vector<node_*> stack {root};
    while (!stack.empty())
    {
      auto cur = stack.back();
      stack.pop_back();
      cur->next.for_each_invoke([&stack](node_* ptr) {
        if (ptr)
          stack.push_back(ptr);
      });
      delete cur;
    }
  }

  // a simple wrapper for convenience
  constexpr void destroy_tree_(node_* root) noexcept { destroy_tree_recursive_(root); }

public:
  constexpr ~trie_base() noexcept { destroy_tree_(root_); }

  constexpr void clear() noexcept
  {
    destroy_tree_(root_);
    root_ = nullptr;
  }

  template <meta::input_range_of<Char> T>
  constexpr void insert(T&& str)
  {
    if (!root_)
      root_ = new node_;

    // pending strong exception safety implementation
    ++root_->pass;
    auto cur = root_;
    for (auto&& ch : str)
    {
      auto& next = cur->next[ch];
      if (!next)
        next = new node_;
      cur = next;
      ++cur->pass;
    }
    ++cur->end;
  }

private:
  enum class count_type_ { full_string, prefix };

  template <count_type_ CountType, typename T>
  constexpr usize count_impl_(T&& str) const noexcept
  {
    if (!root_)
      return 0;

    auto cur = root_;
    for (auto&& ch : str)
    {
      auto next = cur->next[ch];
      if (!next)
        return 0;
      cur = next;
    }

    if constexpr (CountType == count_type_::full_string)
      return cur->end;
    else
      return cur->pass;
  }

public:
  template <meta::input_range_of<Char> T>
  constexpr usize count(T&& str) const noexcept
  {
    return count_impl_<count_type_::full_string>(std::forward<T>(str));
  }

  template <meta::input_range_of<Char> T>
  constexpr usize count_has_prefix(T&& str) const noexcept
  {
    return count_impl_<count_type_::prefix>(std::forward<T>(str));
  }

  template <meta::input_range_of<Char> T>
  constexpr void erase(T&& str) noexcept
  {
    if (!root_ || count(str) == 0)
      return;

    if (--root_->pass == 0)
    {
      clear();
      return;
    }

    auto cur = root_;
    for (auto&& ch : str)
    {
      auto& next = cur->next[ch];
      if (--next->pass == 0)
      {
        destroy_tree_(next);
        next = nullptr;
        return;
      }
      cur = next;
    }
    --cur->end;
  }
};

} // namespace detail

namespace detail {

// Hash must map the given character to [0, DistinctCharCount) without any collisions, otherwise the behavior is
// undefined
template <typename Char, usize DistinctCharCount, typename Hash>
struct trie_default_hash_map
{
  template <typename NodePtr>
  class type
  {
  private:
    std::array<NodePtr, DistinctCharCount> map_ {};
    [[no_unique_address]] Hash hash_;

  public:
    constexpr auto size() const noexcept { return DistinctCharCount; }

    constexpr auto& operator [](const Char& ch) noexcept { return map_[hash_(ch)]; };
    constexpr auto& operator [](const Char& ch) const noexcept { return map_[hash_(ch)]; };

    template <typename Fn>
    constexpr void for_each_invoke(Fn&& f) noexcept
    {
      for (auto child : map_)
      {
        if (child)
          f(child);
      }
    }

    template <typename Fn>
    constexpr void for_each_invoke(Fn&& f, type& to) const
    {
      for (auto i = 0uz; auto child : map_)
        f(child, to.map_[i++]);
    }
  };
};

template <typename Char, typename Hash, template <typename...> typename HashMap>
struct trie_normal_hash_map
{
  template <typename NodePtr>
  class type
  {
  private:
    HashMap<Char, NodePtr, Hash> map_;

  public:
    constexpr auto size() const noexcept { return map_.size(); }

    constexpr auto& operator [](const Char& ch) noexcept { return map_[ch]; };
    constexpr auto& operator [](const Char& ch) const noexcept { return map_[ch]; };

    template <typename Fn>
    constexpr void for_each_invoke(Fn&& f) noexcept
    {
      for (auto [_, child] : map_)
      {
        if (child)
          f(child);
      }
    }

    template <typename Fn>
    constexpr void for_each_invoke(Fn&& f, type& to) const
    {
      for (auto [ch, child] : map_)
      {
        if (child)
          f(child, to[ch]);
      }
    }
  };
};

} // namespace detail

export template <
  typename Char, usize DistinctCharCount, std::regular_invocable<Char> Hash,
  exception_safety E = exception_safety::strong
>
using trie = detail::trie_base<Char, detail::trie_default_hash_map<Char, DistinctCharCount, Hash>, E>;

export using binary_trie = trie<int, 2, decltype([](int x) constexpr noexcept { return x; })>;
export using lower_char_trie = trie<char, 26, decltype([](char ch) constexpr noexcept { return ch - 'a'; })>;
export using upper_char_trie = trie<char, 26, decltype([](char ch) constexpr noexcept { return ch - 'A'; })>;

export template <typename Char, std::regular_invocable<Char> Hash, exception_safety E = exception_safety::strong>
using hash_trie = detail::trie_base<Char, detail::trie_normal_hash_map<Char, Hash, std::unordered_map>, E>;

} // namespace fmia
