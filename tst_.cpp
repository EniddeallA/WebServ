// #include "includes/Parsing.hpp"


// int main(int argc, char const *argv[])
// {
//     std::fstream * file = new std::fstream();
//     file->open("/Users/kbenlyaz/Desktop/test_webserve/main.cpp");
//     std::cout << file->tellg() << std::endl;
//     char buffer[101] = {0};

//     file->seekg (0, file->end);
//     int length = file->tellg();
//     file->seekg (0, file->beg);
//     std::cout << "lenth is " << length << std::endl;
    
//     while (!file->eof())
//     {
//         bzero(buffer, 100);

//         file->read(buffer, 100);

//         std::cout << buffer;
//         // std::cout << (int)buffer[99] << std::endl;
//         /* code */
//     }
    
//     return 0;
// }
