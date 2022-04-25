#include "Parsing.hpp"

std::vector<std::string> split (const std::string s, std::string delim){
    std::vector<std::string> result;
    size_t index = s.find(delim);
    size_t position = 0;
    std::string str;
    while (index != (size_t)-1){
        str = s.substr(position, index - position);
        if (str.length())
            result.push_back(str);
        position = index + delim.length();
        index = s.find(delim, index + 1);
    }
    str = s.substr(position, s.length() - position);
    result.push_back(str);
    return (result);
}
