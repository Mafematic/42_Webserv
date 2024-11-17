#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <string>

inline std::string to_string(int value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

bool ends_with(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

#endif
