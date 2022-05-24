// #include "includes/UtilsUtils.hpp"
#include "includes/Parsing.hpp"

int main() {
	int fd = open("file.conf", O_RDONLY);
	char buffer[100];
	read(fd, buffer, 100);
	std::cout << buffer << std::endl;
	lseek(fd, -99 ,SEEK_CUR);
	read(fd, buffer, 100);
	std::cout << "****************************" << std::endl;
	std::cout << buffer << std::endl;


}

