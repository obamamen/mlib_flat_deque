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

namespace mlib
{
    template< typename type_t,
              size_t initial_grow = 4,
              size_t grow_factor = 2,
              size_t shrink_factor = 2 >
    class flat_deque
    {
    private:
        std::allocator<type_t> _alloc;
        type_t*                _data = nullptr;
        size_t                 _size = 0;
        size_t                 _capacity = 0;
        size_t                 _head = 0;

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

        type_t* get(const size_t index) const
        {
            assert(index < _size);
            return _data + _head + index;
        }

        void push_back(const type_t& value)
        {
            if (_head + _size >= _capacity)
            {
                _grow();
            }

            _alloc.construct(_data + _head + _size, value);
            ++_size;
        }

        size_t size() const { return _size; }
        size_t capacity() const { return _capacity; }
        size_t head() const { return _head; }
        type_t* raw_data() { return _data; }

        void clear()
        {
            _free_members();
            _reset_member_state();
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
        }

        void _grow()
        {
            const size_t old_capacity = _capacity;
            type_t* old_data = _data;
            const size_t old_head = _head;

            size_t new_capacity = _capacity == 0 ? initial_grow : _capacity * grow_factor;
            while (new_capacity < _size)
            {
                new_capacity *= grow_factor;
            }

            type_t* new_data = _alloc.allocate(new_capacity);
            size_t new_head  = (new_capacity - _size) / 2;

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
    };
}

#endif //MLIB_FLAT_DEQUE_FLAT_DEQUE_HPP