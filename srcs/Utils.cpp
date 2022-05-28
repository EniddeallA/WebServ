#include "../includes/Utils.hpp"
#include "../includes/Parsing.hpp"

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

std::vector<std::string> splinter2(const std::string& line , char del) {
    std::vector<std::string>    ret;
    std::string                 tmp;
    size_t     start = line.find(del);
    
    ret.push_back(line.substr(0, start));
    ret.push_back(line.substr(start + 1, line.length() - start));

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

std::string	time_last_modification(struct stat buffer)
{
	tm *ltm = gmtime(&buffer.st_mtime);

	std::ostringstream date;

	date << wday_name[ltm->tm_wday] << ", " << ltm->tm_mday << " "
		<< mon_name[ltm->tm_mon] << " " << (ltm->tm_year + 1900) << " " 
		<< (ltm->tm_hour) % 24 << ":" << ltm->tm_min << ":" << ltm->tm_sec << " GMT";
	return date.str();
}

std::string              lowercase(std::string src) {
    std::string ret = "";

    for (unsigned long i = 0; i < src.length(); i++) {
        if (src[i] >= 'A' && src[i] <= 'Z') {
            ret += char(src[i] + 32);
        } else {
            ret += src[i];
        }
    }
    return ret;
}

std::string				 trim(const std::string& line) {
    std::string::const_iterator start = line.begin(), end = line.end();
    while (start != end && isSpace(*start) == true) {
        start++;
    }
    end--;
    while(std::distance(start, end) > 0 && isSpace(*end) == true) {
        end--;
    }
    return std::string(start, end + 1);
}

bool is_hex_notation(std::string const& s)
{
  return s.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos;
}

