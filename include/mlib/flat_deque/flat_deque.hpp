/* ================================== *\
 @file     flat_deque.hpp
 @project  mlib_flat_deque
 @author   moosm
 @date     12/16/2025
*\ ================================== */

#ifndef MLIB_FLAT_DEQUE_FLAT_DEQUE_HPP
#define MLIB_FLAT_DEQUE_FLAT_DEQUE_HPP

#include <assert.h>
#include <memory>
#include <utility>
#include <cstddef>
#include <cstdint>

namespace mlib
{
    enum class align : uint8_t
    {
        front,
        center,
        back
    };

    enum class shrink_mode
    {
        conservative,
        aggressive
    };

    template< typename type_t,
              size_t initial_grow = 4,
              size_t grow_factor = 2,
              size_t shrink_factor = 2 >
    class flat_deque
    {
        static_assert(initial_grow > 0, "initial_grow must be greater than 0");
    private:
        std::allocator<type_t> _alloc;
        type_t*                _data = nullptr;
        size_t                 _size = 0;
        size_t                 _capacity = 0;
        size_t                 _head = 0;

    public:
        class iterator;

    public:
        flat_deque() = default;

        ~flat_deque()
        {
            clear();
            if (_data)
            {
                _alloc.deallocate(_data, _capacity);
            }
        }

        iterator begin() { return iterator(_data + _head); }

        iterator end() { return iterator(_data + _head + _size); }

        type_t* get(const size_t index) const
        {
            assert(index < _size);
            return _data + _head + index;
        }

        size_t size() const { return _size; }

        size_t capacity() const { return _capacity; }

        bool empty() const { return _size == 0; }

        size_t head() const { return _head; }

        type_t* raw_data() { return _data; }

        type_t& operator[](const size_t index) const
        {
            assert(index < _size);
            return _data[_data + _head + index];
        }

        void reserve(
            const size_t size,
            const align align = align::center)
        {
            switch (align)
            {
                case align::front:
                    if (_head < size)
                    {
                        _grow(size, align);
                    }
                    break;
                case align::back:
                    if ((_capacity - _size) < size)
                    {
                        _grow(size, align);
                    }
                    break;
                case align::center:
                    if (_head < size/2 || (_capacity - _size) < size/2)
                    {
                        _grow(size, align);
                    }
                    break;
            }
        }

        void push_back(const type_t& value)
        {
            if (_head + _size >= _capacity)
            {
                _grow(1, align::back);
            }

            _alloc.construct(_data + _head + _size, value);
            ++_size;
        }

        template <typename... Args>
        void emplace_back(Args... args)
        {
            if (_head + _size >= _capacity)
            {
                _grow(1, align::back);
            }

            _alloc.construct(_data + _head + _size, std::forward<Args>(args)...);
            ++_size;
        }

        void push_front(const type_t& value)
        {
            if (_head == 0)
            {
                _grow(1, align::front);
            }

            assert(_head > 0);
            _head--;
            _alloc.construct(_data + _head, value);
            ++_size;
        }

        template <typename... Args>
        void emplace_front(Args... args)
        {
            if (_head == 0)
            {
                _grow(1, align::front);
            }

            assert(_head > 0);
            _head--;
            _alloc.construct(_data + _head, std::forward<Args>(args)...);
            ++_size;
        }

        void pop_back()
        {
            if (_size < 1) return;

            // std::cout << "DEBUG "
            //     << "size: " << _size
            //     << " capacity: " << _capacity
            //     << " head: " << _head
            // << "\n";

            _alloc.destroy(_data + _head + _size - 1);
            --_size;

            _shrink(align::back);
            _shrink(align::front);
            _shrink(align::center);
        }

        void pop_front()
        {
            if (_size < 1) return;

            _alloc.destroy(_data + _head);
            ++_head;
            --_size;

            _shrink(align::front);
            _shrink(align::back);
            _shrink(align::center);
        }

        void pop_from_back(size_t count)
        {
            if (count == 0 || _size == 0)
                return;

            if (count > _size)
                count = _size;

            for (size_t i = 0; i < count; ++i)
                _alloc.destroy(_data + _head + _size - 1 - i);


            _size -= count;

            _shrink(align::back, shrink_mode::conservative);
        }

        void pop_from_front(size_t count)
        {
            if (count == 0 || _size == 0)
                return;

            if (count > _size)
                count = _size;

            for (size_t i = 0; i < count; ++i)
                _alloc.destroy(_data + _head + i);


            _head += count;
            _size -= count;

            _shrink(align::front, shrink_mode::conservative);
        }

        void clear()
        {
            _free_members();
            _reset_member_state();
        }

        void fit(const align alignment)
        {
            _shrink(align::center, shrink_mode::aggressive);
        }

    private:
        type_t* _get(size_t index) const noexcept
        {
            assert(index < _size);

            return _data + _head + index;
        }

        void _free_members()
        {
            for (size_t i = 0; i < _size; ++i)
            {
                _alloc.destroy(_data + _head + i);
            }
        }

        void _reset_member_state()
        {
            _size = 0;
            _head = 0;
            _capacity = 0;
        }

        void _grow(
            const size_t object_count = 1,
            const align alignment = align::center)
        {
            const size_t old_capacity = _capacity;
            type_t* old_data = _data;
            const size_t old_head = _head;

            const size_t min_required = _size + object_count;
            size_t new_capacity = _capacity == 0 ? initial_grow : _capacity * grow_factor;
            while (new_capacity < min_required)
            {
                new_capacity *= grow_factor;
            }

            type_t* new_data = _alloc.allocate(new_capacity);
            size_t new_head  = (new_capacity - _size) / 2;
            switch (alignment)
                // all the cases do need testing ngl.
            {
                case align::front:
                    new_head = object_count;
                    break;
                case align::center:
                    new_head = (new_capacity - _size) / 2;
                    break;
                case align::back:
                    new_head = _head;
                    break;
            }

            for (size_t i = 0; i < _size; ++i)
            {
                _alloc.construct(
                    new_data + new_head + i,
                    std::move(old_data[_head + i]));
            }

            if (old_data)
            {
                for (size_t i = 0; i < _size; ++i)
                {
                    _alloc.destroy(old_data + old_head + i);
                }

                _alloc.deallocate(old_data, old_capacity);
            }

            _data = new_data;
            _capacity = new_capacity;
            _head = new_head;
        }

        void _shrink(
            const align alignment,
            const shrink_mode mode = shrink_mode::conservative)
        {
            if (_capacity <= initial_grow)
                return;

            size_t new_capacity = _capacity;

            while (true)
            {
                const size_t candidate
                    = new_capacity / shrink_factor;

                if (candidate < initial_grow)
                    break;

                if (mode == shrink_mode::conservative)
                {
                    if (candidate < _size * 2)
                        break;
                }
                else
                {
                    if (candidate < _size)
                        break;
                }

                new_capacity = candidate;
            }

            if (new_capacity == _capacity)
                return;

            type_t* new_data = _alloc.allocate(new_capacity);

            size_t new_head;
            switch (alignment)
            {
                case align::front:
                    new_head = 0;
                    break;
                case align::center:
                    new_head = (new_capacity - _size) / 2;
                    break;
                case align::back:
                    new_head = new_capacity - _size;
                    break;
            }

            for (size_t i = 0; i < _size; ++i)
            {
                _alloc.construct(
                    new_data + new_head + i,
                    std::move(_data[_head + i]));
            }

            for (size_t i = 0; i < _size; ++i)
                _alloc.destroy(_data + _head + i);

            _alloc.deallocate(_data, _capacity);

            _data = new_data;
            _capacity = new_capacity;
            _head = new_head;
        }

    public:
        class iterator
        {
        public:
            using value_type = type_t;
            using pointer = type_t*;
            using reference = type_t&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            iterator(const pointer ptr) : _ptr(ptr) {}

            reference operator*() const { return *_ptr; }
            pointer operator->() { return _ptr; }

            iterator& operator++() { ++_ptr; return *this; }
            iterator operator++(int) { iterator tmp = *this; ++_ptr; return tmp; }

            iterator& operator--() { --_ptr; return *this; }
            iterator operator--(int) { iterator tmp = *this; --_ptr; return tmp; }

            bool operator==(const iterator& other) const { return _ptr == other._ptr; }
            bool operator!=(const iterator& other) const { return _ptr != other._ptr; }

        private:
            pointer _ptr;
        };
    };
}

#endif //MLIB_FLAT_DEQUE_FLAT_DEQUE_HPP