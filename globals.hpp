#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <memory>
#include <vector>

template <class T> using uptr = std::unique_ptr<T>;
template <class T> using vec = std::vector<T>;

#endif