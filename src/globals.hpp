#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <memory>
#include <vector>

template <class T> using shptr = std::shared_ptr<T>;
template <class T> using vec = std::vector<T>;

typedef std::pair<unsigned int, unsigned int> source_position_t;

#endif