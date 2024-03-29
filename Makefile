SRCS		=	main.cpp srcs/Response/Response.cpp srcs/Utils.cpp srcs/request.cpp \
				srcs/Parsing/Parsing.cpp srcs/Parsing/fill_data_in_struct.cpp srcs/Parsing/utils.cpp \
				 srcs/Server/server.cpp srcs/Server/server_utils.cpp srcs/Response/MimeTypes.cpp


   
CPP_VERSION	=	#-std=c++98

COMPILER	=	clang++

FLAGS		=	-Wextra -Wall -Werror 

NAME		=	ft_server

all : $(NAME)

$(NAME) : $(SRC) 
	@$(COMPILER) $(CPP_VERSION) $(FLAGS) $(SRCS) -o $(NAME)

run : 
	@$(COMPILER) $(CPP_VERSION) $(FLAGS) $(SRCS)  -o $(NAME)

clean :
	@rm -rf $(NAME)  *.dSYM a.out 

fclean : clean

re : fclean all

.PHONY: all clean fclean