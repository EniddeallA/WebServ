
#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <algorithm>
# include <sstream>

std::vector<std::string> splinter(const std::string& line, char del = ' ') ;
size_t                   convertsizeT(std::string& src);
size_t                   to_hex(std::string &str);
bool                     isSpace(unsigned char c);
#endif