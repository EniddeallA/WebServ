
SRCS		=	main.cpp srcs/Response.cpp srcs/Utils.cpp srcs/cgi.cpp srcs/request.cpp \
				srcs/Parsing/Parsing.cpp srcs/Parsing/fill_data_in_struct.cpp srcs/Parsing/utils.cpp \
				 srcs/Server/server.cpp srcs/Server/server_utils.cpp srcs/responce_file.cpp srcs/MimeTypes.cpp


   
CPP_VERSION	=	#-std=c++98

COMPILER	=	clang++

FLAGS		=	#-g3 #-Wextra -Wall -Werror

NAME		=	ft_server

all : $(NAME)

$(NAME) : $(SRC) 
	@$(COMPILER) $(CPP_VERSION) $(FLAGS) $(SRCS) -o $(NAME)

run : 
	@$(COMPILER) $(CPP_VERSION)  $(SRCS)  -o $(NAME)

clean :
	@rm -rf $(NAME)  *.dSYM .vscode a.out

fclean : clean

re : fclean all

.PHONY: all clean fclean