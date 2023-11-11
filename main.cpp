
#include <array>
#include <iostream>
#include <iterator>
#include <list>

#include "iterzip.hpp"

using iterzip::zip;

int main() {
	std::list<int> a{1,2,3,4,5};
    std::list<int> b{6,7,8,9,10};

    using T1 = decltype(a.begin());
    using T2 = decltype(a.begin());
    using cat = typename iterzip::iterator_traits<T1,T2>::iterator_category;

    auto zipped = zip(a, b);
    for(auto iter = zipped.begin(); iter != zipped.end(); iter++) {
        auto x = std::get<0>(*iter);
        auto& y = std::get<1>(*iter);
        std::cout << x << ", " << y << std::endl;
        x = 5;
    }

    std::cout << std::endl;

    for(auto [x,y] : zip(a,b)) {
        std::cout << x << ", " << y << std::endl;
    }

    std::cout << std::endl;

    std::array<int, 3> d{1,2,3};
    std::array<int, 2> e{3,4};

    auto zipped2 = zip(d, e);
    // only works with random access iterators
    std::cout << std::get<0>(zipped2.begin()[1]) << std::endl;

    std::cout << std::endl;

    for(auto [x,y] : zip(d,e)) {
        std::cout << x << ", " << y << std::endl;
    }
}