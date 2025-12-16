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
    mlib::flat_deque<int, 1> deque{};
    auto log = [&]()
    {
        std::cout << "[main] Deque size: " << deque.size() << std::endl;
        for (int i = 0; i < deque.capacity(); ++i)
        {
            if (i >= deque.head() && i < deque.head() + deque.size())
            {
                std::cout << " - " << *(deque.raw_data() + i) << std::endl;
            } else
            {
                std::cout << " - " << "?" << std::endl;
            }
        }
    };

    constexpr size_t obj_c = 10;

    deque.reserve( obj_c , mlib::align::back);

    log();

    for (int i = 0; i < obj_c; i++)
        deque.push_back( i );

    log();


    for (int i = 0; i < deque.size(); i++)
        deque.clear();

    log();

    deque.push_front( 67 );

    log();

    return 0;
}