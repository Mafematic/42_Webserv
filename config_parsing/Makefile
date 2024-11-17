# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: smatthes <smatthes@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/21 10:44:02 by smatthes          #+#    #+#              #
#    Updated: 2024/11/17 12:12:11 by smatthes         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX 		= c++
CXXFLAGS 	= -Werror -Wextra -Wall -Wpedantic -fsanitize=address,undefined -Wshadow -g -std=c++98 
# CXXFLAGS 	= -Werror -Wextra -Wall -std=c++98
# CXXFLAGS 	= -std=c++98

TARGET 		= test
FILES 		= main Config_Parser Server_Parser Location_Parser Directive Directive_Root util Directive_Alias Directive_Index Directive_Error_Page Directive_Client_Max_Body_Size Directive_Allowed_Methods Directive_Autoindex Directive_Listen Directive_Server_Name Directive_Return Server_Creator Route_Creator Route Server
INCLUDES 	= -I./ -I./result/
SRC			= $(FILES:=.cpp)
OBJ 		= $(SRC:.cpp=.o)
PATHSRC		= ./ ./result/

# specifies the path, where the compiler will look for files (e.g. *.c, *.h files)
# that way, you dont have to specify full filepath when listing source files below
VPATH = $(PATHSRC) 


.PHONY: all clean fclean re

all: ${TARGET}

${TARGET}: ${OBJ}
	${CXX} ${CXXFLAGS} ${OBJ} -o ${TARGET}

%.o: %.cpp
	${CXX} $(CXXFLAGS) ${INCLUDES} -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(TARGET)

re: fclean all