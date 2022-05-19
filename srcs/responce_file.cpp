#include "../includes/Parsing.hpp"
/*
to explain:
http://localhost:8081/home/route1/file1
http://localhost:8081/conatct

file.conf:
server {
  listen localhost:8081
  ...
  location /home
  { 
    root ./folder1/
    ...
    return 302 /contact
  }
}
mn server block:
you'll try to find /home/route1/file1
wach kayena f chi location
makaynach adouz
/home/route1
w /file1 store it
ma l9itich /home/route1
adouz l /home
w /route1 atzad 3la /file1
w atwali /route1/file1
w at3awed t9aleb 3la /home
atl9aha
so db atmchi atched dir 
root li fiha ./folder1/
w atwali 3ndek ./folder1/route1/file1
w dik sa3a atchouf wach hada li 3tak file wla foler
ila kan folder atchouf wach fih chi index, matalan index.php wla index.html (ila kan ki 9blo ofc)
 "    "    "   atchouf wach fih auto index donc adir autoindex
ila kan file atchouf wach khas idouz 3la cgi wla la
...
did you got it ?
*/

//path can be /home/route1/file1
void    file_to_return(Server_block server, std::string path){

    // if they are return function there
    // std::vector<std::string> splited_path = split(path, "/");
    Location_block l_block;
    int i = 0;
    int count = 0;
    while (i < path.size()){
        if (path[i] == '/')
            count++;
        i++;
    }
    
    std::cout << "ENTER TO FUNCTION size is " << count << std::endl;
    for (int r = 0; r < count; r++){
        for (int i = 0; i < server.all_locations.size(); i++){
            l_block = server.all_locations[i];
            std::cout << "************************************************************************\n";
            std::cout << l_block.path << std::endl;
            std::cout << path << std::endl;
            if (l_block.path == path){ // gennerate file to uplade
                std::cout << "-----------------------------------------------------------\n";
                std::cout << "path is " << path << " compare with " << l_block.path << std::endl;
                std::cout << "-----------------------------------------------------------\n";
                if (l_block.return_path.size()){ //send responce
                    std::cout << "--------------------------------Return function---------------\n";
                    std::cout << "Return code is " << l_block.return_code << "  to path" << l_block.return_path << std::endl;
                    std::cout << "--------------------------------Return function---------------\n";
                }
                break;
            }
         }
        while(path.size() && path[path.size() - 1] != '/') {
            path.pop_back();
        }
        if (path.size() && path[path.size() - 1] == '/')
            path.pop_back();
    }
}