
#include <array>
#include <iostream>

#include "iterzip.hpp"

using iterzip::zip;

int main() {
	std::array<int, 5> a{1,2,3,4,5};
    std::array<int, 5> b{6,7,8,9,10};

    for(auto [x,y] : zip(a,b)) {
        std::cout << x << ", " << y << std::endl;
        x = 5;
    }
    for(auto [x,y] : zip(a,b)) {
        std::cout << x << ", " << y << std::endl;
    }


    std::array<int, 3> d{1,2,3};
    std::array<int, 2> e{3,4};

    for(auto [x,y] : zip(d,e)) {
        std::cout << x << ", " << y << std::endl;
    }
}