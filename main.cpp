#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

#include "include/mlib/flat_deque/flat_deque.hpp"

// void* operator new(std::size_t size)
// {
//     void* ptr = std::malloc(size);
//     std::cout << "[new] Allocated " << size << " bytes at " << ptr << std::endl;
//     if (!ptr)
//         throw std::bad_alloc();
//     return ptr;
// }
//
// void operator delete(void* ptr) noexcept
// {
//     std::cout << "[delete] Deallocated memory at " << ptr << std::endl;
//     std::free(ptr);
// }
//
// void* operator new[](std::size_t size)
// {
//     void* ptr = std::malloc(size);
//     std::cout << "[new[]] Allocated " << size << " bytes at " << ptr << std::endl;
//     if (!ptr)
//         throw std::bad_alloc();
//     return ptr;
// }
//
// void operator delete[](void* ptr) noexcept
// {
//     std::cout << "[delete[]] Deallocated memory at " << ptr << std::endl;
//     std::free(ptr);
// }


int main()
{
    mlib::flat_deque<std::string, 1> deque{};
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

    srand(1);

    std::vector<
        std::pair<
            std::string,
            std::function<void(decltype(deque)&)>
        >
    >
     ops = {
        {"push front",
            [](decltype(deque)& d){ d.push_front("front");}},
        {"push back",
            [](decltype(deque)&d){ d.push_back("back");}},
        {"pop front",
            [](decltype(deque)&d){ d.pop_front();}},
        {"pop back",
            [](decltype(deque)&d){ d.pop_back();}},
        {"fit",
            [](decltype(deque)&d){ d.fit(mlib::align::center);}},
        {"emplace front",
            [](decltype(deque)&d){ d.emplace_front("front");}},
        {"emplace back",
            [](decltype(deque)&d){ d.emplace_back("front");}},
    };

    for (int i = 0; i < 1024*1024*5; ++i)
    {
        int x = rand() % ops.size();
        ops[x].second(deque);
    }


    //log();
    std::cout << "[main] Deque size: " << deque.size() << std::endl;
    std::cout << "[main] Deque capacity: " << deque.capacity() << std::endl;
    std::cout << "[main] Deque capacity-size: " << deque.capacity() - deque.size() << std::endl;
    deque.fit(mlib::align::center);
    std::cout << "[main] Deque size: " << deque.size() << std::endl;
    std::cout << "[main] Deque capacity: " << deque.capacity() << std::endl;
    std::cout << "[main] Deque capacity-size: " << deque.capacity() - deque.size() << std::endl;
    deque.pop_from_back(deque.size());
    std::cout << "[main] Deque size: " << deque.size() << std::endl;
    std::cout << "[main] Deque capacity: " << deque.capacity() << std::endl;
    std::cout << "[main] Deque capacity-size: " << deque.capacity() - deque.size() << std::endl;

    return 0;
}