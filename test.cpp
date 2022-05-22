// #include "includes/UtilsUtils.hpp"
#include "includes/Parsing.hpp"
long int get_current_time(){

	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int us = tp.tv_sec * 1000000 + tp.tv_usec;
	return us;
}

int main() {
   	long int now = get_current_time();
    std::string s =  std::to_string(now);
    std::cout << s;
}

