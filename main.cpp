#include "includes/Parsing.hpp"



int main(int argc, char const *argv[])
{
    try {
        signal(SIGPIPE, SIG_IGN);
        int check;
        if (argc == 2)

            check = open(argv[1], O_RDONLY);
        else
            check = open("./config_files/default.conf", O_RDONLY);

            if (check < 0)
                throw "Error in file path";
            std::vector<Server_block> all_servers;
             if (argc == 2)
                all_servers = parsse_the_config_file(argv[1]);
            else
                all_servers = parsse_the_config_file("./config_files/default.conf");
            start_servers(all_servers);
    }
    catch (char const *e){
        std::cout << "ERROR HAS BEN FOUND : " << e << std::endl ;
    }
    return 0;
}
