NAME	:= cgitest
CC		:= c++ -g
CFLAGS	:= -Wall -Werror -Wextra -std=c++98
FILES 	:=	main Cgi_Controller Cgi_Executor


SRC	= $(FILES:=.cpp)
PATHSRC		= ./
VPATH = $(PATHSRC)
INCLUDES:= -I./ -I../../includes
OBJDIR  := objs/

OBJFNAME = $(SRC:.cpp=.o)
OBJS = $(patsubst %,$(OBJDIR)%,$(OBJFNAME))


$(OBJDIR)%.o: %.cpp
	$(CC) $(CFLAGS) ${INCLUDES} -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
