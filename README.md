# iterzip for C++17

Simplifies iteration over multiple containers at once.
Header-only implementation for a python-style `zip` iterator.
Supports all standard iterator categories and conforms to `std::iterator_traits`.

## Usage

Supports all iterator-conforming containers.
This, of course, includes all STL containers such as `std::array`, `std::vector`, `std::list` etc.

```c++
#include "iterzip.hpp"
using iterzip::zip;

std::array<int, 5> a{1,2,3,4,5};
std::array<int, 5> b{6,7,8,9,10};

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
```

### Containers with different sizes
For containers with different sizes, the smallest container determines the length of of the `zip` iteration.

```c++
std::array<int, 4> d{1,2,3,4};
std::array<int, 2> e{5,6};

for(auto [x,y] : zip(d,e)) {
    std::cout << x << ", " << y << std::endl;
}
```
Will produce the following output
```
1, 5
2, 6
```


### Higher categories

If the containers support it, it is possible to use higher iterator categories' functions.
For example, the `iterator::operator[]` on two `std::array`s:
```c++
std::array<int, 2> d{1,2};
std::array<int, 2> e{3,4};

auto zipped = zip(d, e);
// only works with random access iterators
std::cout << std::get<0>(zipped.begin()[1]) << std::endl; // outputs 2 (which is the second element in the first container.)
```

## Iterator traits for zip iterators

The `zip` iterator category depends on the given input containers' iterators.
To achieve that, the `iterzip` namespace provides a natural extension of `std::iterator_traits`, called `iterzip::iterator_traits`.
The chosen iterator category is always the lowest category of all input iterator types.

```c++
using T1 = std::list<int>::iterator;
using T2 = std::array<int>::iterator;
using cat = typename iterzip::iterator_traits<T1,T2>::iterator_category;
// cat == std::bidirectional_iterator_tag;
```

The zipped `value_type` is a `std::tuple<X, ...>` where the `X` are the individual iterators' `value_type`.
The `reference` typedef is `std::tuple<X&, ...>` and `pointer` is mapped to `value_type*`.

## Compiling / Building / Including

In your project, simply include the header file `iterzip.hpp` and make sure you have support for C++17.

To build the example file provided in the repository, run
```bash
$ gcc main.cpp -o main -lstdc++ -std=c++17
$ ./main
```
