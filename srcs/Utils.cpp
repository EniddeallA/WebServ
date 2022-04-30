#include "../includes/Utils.hpp"


std::vector<std::string> splinter(const std::string& line, char del) {
    std::vector<std::string>    ret;
    std::string                 tmp;
    size_t     i = 0, start = 0, end = 0;
    while(i <= line.length()) {
        if (line[i] == del || i == line.length()) {
            end = i;
            // tmp = "";
            tmp = line.substr(start, end - start);
            ret.push_back(tmp);
            i = start = end + 1;
        }
        i++;
    }


    return ret;
}

size_t       convertsizeT(std::string& src) {
    std::stringstream ss(src);
    size_t  ret;

    ss >> ret;
    return ret;
    
}

size_t to_hex(std::string &str) {
    std::stringstream ss;
    size_t hex;

    ss << std::hex << str;
    ss >> hex;

    return hex;
}

bool isSpace(unsigned char c) {
    return (c == ' ' ||
        c == '\t' || c == '\f');
}
