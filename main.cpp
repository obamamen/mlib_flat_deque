#include <iostream>
#include "include/mlib/flat_deque/flat_deque.hpp"

void* operator new(std::size_t size)
{
    void* ptr = std::malloc(size);
    std::cout << "[new] Allocated " << size << " bytes at " << ptr << std::endl;
    if (!ptr)
        throw std::bad_alloc();
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    std::cout << "[delete] Deallocated memory at " << ptr << std::endl;
    std::free(ptr);
}

void* operator new[](std::size_t size)
{
    void* ptr = std::malloc(size);
    std::cout << "[new[]] Allocated " << size << " bytes at " << ptr << std::endl;
    if (!ptr)
        throw std::bad_alloc();
    return ptr;
}

void operator delete[](void* ptr) noexcept
{
    std::cout << "[delete[]] Deallocated memory at " << ptr << std::endl;
    std::free(ptr);
}

int main()
{
    mlib::flat_deque<int, 50> deque{};
    deque.push_back(0);
    deque.push_back(0);

    for (int i = 0; i < deque.capacity(); ++i)
    {
        if (i >= deque.head() && i < deque.head() + deque.size())
        {
            std::cout << *(deque.raw_data() + i) << std::endl;
        } else
        {
            std::cout << "?" << std::endl;
        }

    }
    return 0;
}