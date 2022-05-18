
#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <algorithm>
# include <sstream>
 #include <fcntl.h>
 #include <sys/types.h>
 #include <sys/stat.h>

static const std::string wday_name[7] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const std::string mon_name[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

std::vector<std::string> splinter(const std::string& line, char del = ' ') ;
size_t                   convertsizeT(std::string& src);
size_t                   to_hex(std::string &str);
bool                     isSpace(unsigned char c);
std::string	time_last_modification(struct stat buffer);
std::string              lowercase(std::string src);
#endif