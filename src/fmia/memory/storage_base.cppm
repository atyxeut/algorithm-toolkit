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

export module fmia.memory.storage_base;

import std;

import fmia.math.integer.twos_complement;
import fmia.memory.core;

export namespace fmia {

enum class storage_location { heap, inplace };

} // export namespace fmia

namespace fmia {

template <meta::size_integral T>
constexpr T dynamic_capacity = static_cast<T>(-1);
  
template <typename Size, Size Capacity>
class heap_capacity;

template <typename Size, Size Capacity>
  requires (Capacity > 0)
class heap_capacity<Size, Capacity>
{
protected:
  static constexpr Size capacity_ = Capacity;
};

template <typename Size, Size Capacity>
  requires (Capacity == dynamic_capacity<Size>)
class heap_capacity<Size, Capacity>
{
protected:
  Size capacity_;

protected:
  constexpr heap_capacity() noexcept = default;
  constexpr heap_capacity(Size capacity) noexcept : capacity_ {capacity} {}
  constexpr heap_capacity(heap_capacity&& other) noexcept : capacity_ {std::exchange(other.capacity_, 0)} {}

  constexpr auto& operator =(heap_capacity&& other) noexcept
  {
    if (this != std::addressof(other))
      capacity_ = std::exchange(other.capacity_, 0);
    return *this;
  }

  constexpr heap_capacity(const heap_capacity&) noexcept = default;
  constexpr auto& operator =(const heap_capacity&) noexcept = default;
};

template <typename T, typename Allocator, typename Size, Size Capacity, exception_safety ExceptionSafety>
class heap_storage_base : public heap_capacity<Size, Capacity>
{
private:
  using base_ = heap_capacity<Size, Capacity>;
  using alt_ = std::allocator_traits<Allocator>;

public:
  using allocator_type = Allocator;
  using size_type = Size;
  using difference_type = meta::make_signed_t<Size>;
  using value_type = alt_::value_type;
  using pointer = alt_::pointer;

protected:
  size_type size_;
  pointer buffer_;

private:
  [[no_unique_address]] allocator_type allocator_;

  constexpr void deallocate_() noexcept
  {
    if (buffer_)
      alt_::deallocate(allocator_, buffer_, this->capacity_);
    buffer_ = nullptr;
  }

public:
  [[nodiscard]] consteval bool has_dynamic_capacity() const noexcept { return Capacity == dynamic_capacity<size_type>; }
  [[nodiscard]] constexpr size_type capacity() const noexcept { return this->capacity_; }

  constexpr void clear() noexcept
  {
    while (size_ > 0) {
      --size_;
      alt_::destroy(allocator_, buffer_ + size_);
    }
  }

protected:
  constexpr heap_storage_base()
    requires (!has_dynamic_capacity())
    : size_ {}
  {
    buffer_ = alt_::allocate(allocator_, Capacity);
  }

  constexpr explicit heap_storage_base(size_type capacity)
    requires (has_dynamic_capacity())
    : base_(capacity), size_ {}
  {
    buffer_ = capacity == 0 ? nullptr : alt_::allocate(allocator_, capacity);
  }

  constexpr heap_storage_base(heap_storage_base&& other) noexcept
    : base_(std::move(other)), size_ {std::exchange(other.size_, 0)}, buffer_ {std::exchange(other.buffer_, nullptr)},
      allocator_ {std::move(other.allocator_)}
  {}

  constexpr auto& operator =(heap_storage_base&& other) noexcept(
    alt_::is_always_equal::value || alt_::propagate_on_container_move_assignment::value
  )
  {
    if (this == std::addressof(other))
      return *this;

    constexpr bool equal_v = alt_::is_always_equal::value;
    constexpr bool pocma_v = alt_::propagate_on_container_move_assignment::value;

    if (equal_v || pocma_v || allocator_ == other.allocator_) {
      clear();
      deallocate_();
      base_::operator =(std::move(other));
      size_ = std::exchange(other.size_, 0);
      buffer_ = std::exchange(other.buffer_, nullptr);
      if constexpr (pocma_v)
        allocator_ = std::move(other.allocator_);
      return *this;
    }

    if constexpr (std::is_nothrow_move_constructible_v<T>) {
      if (buffer_ == nullptr || this->capacity_ < other.size_) {
        const auto tmp_buffer = alt_::allocate(allocator_, other.size_);
        clear();
        deallocate_();
        if constexpr (has_dynamic_capacity())
          this->capacity_ = other.size_;
        buffer_ = tmp_buffer;
      } else {
        clear();
      }
      size_ = other.size_;
      while (other.size_ > 0) {
        --other.size_;
        alt_::construct(allocator_, buffer_ + other.size_, std::move(other.buffer_[other.size_]));
        alt_::destroy(allocator_, other.buffer_ + other.size_);
      }
    } else if constexpr (ExceptionSafety == exception_safety::strong) {
      if (other.size_ == 0) {
        clear();
        return *this;
      }

      const auto tmp_buffer = alt_::allocate(allocator_, other.size_);
      size_type tmp_size = 0;
      try {
        for (; tmp_size < other.size_; ++tmp_size)
          alt_::construct(allocator_, tmp_buffer + tmp_size, other.buffer_[tmp_size]);
      } catch (...) {
        while (tmp_size > 0) {
          --tmp_size;
          alt_::destroy(allocator_, tmp_buffer + tmp_size);
        }
        alt_::deallocate(allocator_, tmp_buffer, other.size_);
        throw;
      }
      clear();
      deallocate_();
      if constexpr (has_dynamic_capacity())
        this->capacity_ = other.size_;
      size_ = other.size_;
      buffer_ = tmp_buffer;
    } else {
      if (buffer_ == nullptr || this->capacity_ < other.size_) {
        const auto tmp_buffer = alt_::allocate(allocator_, other.size_);
        size_type tmp_size = 0;
        try {
          for (; tmp_size < other.size_; ++tmp_size)
            alt_::construct(allocator_, tmp_buffer + tmp_size, std::move(other.buffer_[tmp_size]));
          other.clear();
        } catch (...) {
          while (tmp_size > 0) {
            --tmp_size;
            alt_::destroy(allocator_, tmp_buffer + tmp_size);
          }
          alt_::deallocate(allocator_, tmp_buffer, other.size_);
          throw;
        }
        clear();
        deallocate_();
        if constexpr (has_dynamic_capacity())
          this->capacity_ = tmp_size;
        size_ = tmp_size;
        buffer_ = tmp_buffer;
      } else {
        clear();
        try {
          for (; size_ < other.size_; ++size_)
            alt_::construct(allocator_, buffer_ + size_, std::move(other.buffer_[size_]));
          other.clear();
        } catch (...) {
          clear();
          throw;
        }
      }
    }
    return *this;
  }

  constexpr heap_storage_base(const heap_storage_base& other)
    : size_ {}, allocator_ {alt_::select_on_container_copy_construction(other.allocator_)}
  {
    if constexpr (has_dynamic_capacity())
      this->capacity_ = other.size_;
    buffer_ = alt_::allocate(allocator_, this->capacity_);
    try {
      for (; size_ < other.size_; ++size_)
        alt_::construct(allocator_, buffer_ + size_, other.buffer_[size_]);
    } catch (...) {
      while (size_ > 0) {
        --size_;
        alt_::destroy(allocator_, buffer_ + size_);
      }
      alt_::deallocate(allocator_, buffer_, this->capacity_);
      throw;
    }
  }

  constexpr auto& operator =(const heap_storage_base& other)
  {
    if (this == std::addressof(other))
      return *this;

    constexpr bool pocca_v = alt_::propagate_on_container_copy_assignment::value;

    if (other.size_ == 0) {
      clear();
      if constexpr (pocca_v) {
        if (allocator_ != other.allocator_) {
          deallocate_();
          allocator_ = other.allocator_;
          if constexpr (has_dynamic_capacity()) {
            this->capacity_ = 0;
            buffer_ = nullptr;
          } else {
            buffer_ = alt_::allocate(allocator_, Capacity);
          }
        }
      }
      return *this;
    }

    if constexpr (ExceptionSafety == exception_safety::strong) {
      const size_type tmp_capacity = has_dynamic_capacity() ? other.size_ : Capacity;
      size_type tmp_size = 0;
      const auto tmp_allocator = pocca_v ? other.allocator_ : allocator_;
      const auto tmp_buffer = alt_::allocate(tmp_allocator, tmp_capacity);
      try {
        for (; tmp_size < other.size_; ++tmp_size)
          alt_::construct(tmp_allocator, tmp_buffer + tmp_size, other.buffer_[tmp_size]);
        clear();
        deallocate_();
        if constexpr (has_dynamic_capacity())
          this->capacity_ = other.size_;
        size_ = tmp_size;
        buffer_ = tmp_buffer;
        if constexpr (pocca_v)
          allocator_ = other.allocator_;
      } catch (...) {
        while (tmp_size > 0) {
          --tmp_size;
          alt_::destroy(tmp_allocator, tmp_buffer + tmp_size);
        }
        alt_::deallocate(tmp_allocator, tmp_buffer, tmp_capacity);
        throw;
      }
    } else {
      clear();
      if ((pocca_v && allocator_ != other.allocator_) || buffer_ == nullptr || this->capacity_ < other.size_) {
        deallocate_();
        if constexpr (has_dynamic_capacity())
          this->capacity_ = other.size_;
        if constexpr (pocca_v)
          allocator_ = other.allocator_;
        buffer_ = alt_::allocate(allocator_, this->capacity_);
      }
      try {
        for (; size_ < other.size_; ++size_)
          alt_::construct(allocator_, buffer_ + size_, other.buffer_[size_]);
      } catch (...) {
        while (size_ > 0) {
          --size_;
          alt_::destroy(allocator_, buffer_ + size_);
        }
        throw;
      }
    }
    return *this;
  }

  constexpr ~heap_storage_base() noexcept
  {
    clear();
    deallocate_();
  }
};

} // namespace fmia

export namespace fmia {

// normal heap memory, the size of the memory is determined at runtime
template <typename T, meta::size_integral Size = usize, typename Allocator = std::allocator<T>>
using heap_storage = heap_storage_base<T, Allocator, Size, dynamic_capacity<Size>, exception_safety::basic>;

template <typename T, meta::size_integral Size = usize, typename Allocator = std::allocator<T>>
using heap_storage_safer = heap_storage_base<T, Allocator, Size, dynamic_capacity<Size>, exception_safety::strong>;

// on-heap memory whose size is determined at compile-time
template <typename T, usize Capacity, typename Allocator = std::allocator<T>>
using constant_heap_storage = heap_storage_base<T, Allocator, usize, Capacity, exception_safety::basic>;

template <typename T, usize Capacity, typename Allocator = std::allocator<T>>
using constant_heap_storage_safer = heap_storage_base<T, Allocator, usize, Capacity, exception_safety::strong>;

} // export namespace fmia
