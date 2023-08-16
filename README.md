# iterzip for C++17

Simplifies iteration over multiple containers at once.
Header-only implementation for a python-style `zip` iterator.

## Usage

```c++
#include "iterzip.hpp"
using iterzip::zip;

std::array<int, 5> a{1,2,3,4,5};
std::array<int, 5> b{6,7,8,9,10};

for(auto [x,y] : zip(a,b)) {
    std::cout << x << ", " << y << std::endl;
    x = 5;
}

std::cout << "\n" << std::endl;

for(auto [x,y] : zip(a,b)) {
    std::cout << x << ", " << y << std::endl;
}
```
Will produce the following output
```
1, 6
2, 7
3, 8
4, 9
5, 10

5, 6
5, 7
5, 8
5, 9
5, 10
```

--

For containers that have different sizes, the smallest container determines the size of of the `zip` iteration.

```c++
#include "iterzip.hpp"
using iterzip::zip;

std::array<int, 3> d{1,2,3};
std::array<int, 2> e{3,4};

for(auto [x,y] : zip(d,e)) {
    std::cout << x << ", " << y << std::endl;
}
```
Will produce the following output
```
1, 3
2, 4
```

## Building

Simply include the header file `iterzip.hpp`

```bash
$ gcc main.cpp -o main -lstdc++ -std=c++17
$ 
```
