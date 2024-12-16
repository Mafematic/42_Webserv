NAME	= webserv
CC		= c++ -g
CFLAGS	= -Wall -Werror -Wextra #-std=c++98
FILES =	main \
			Client \
			Route \
			Server \
			Serverhandler \
			ServerManager \
			Logger
FILES_PARSER =	Config_Parser \
				Server_Parser \
				Location_Parser \
				Directive \
				Directive_Root \
				util \
				Directive_Alias \
				Directive_Index \
				Directive_Error_Page \
				Directive_Client_Max_Body_Size \
				Directive_Allowed_Methods \
				Directive_Autoindex \
				Directive_Listen \
				Directive_Server_Name \
				Directive_Return \
				Directive_Cgi \
				Server_Creator \
				Route_Creator
REQUEST_PARSER =	Request \
					RequestRouter \
					Uploader
CGI = Cgi_Controller Cgi_Executor Path_Analyser

SRC	= $(FILES:=.cpp) $(FILES_PARSER:=.cpp) $(REQUEST_PARSER:=.cpp) $(CGI:=.cpp)

PATHSRC		= ./ ./src/ ./src/config_parser ./src/utils/ ./src/request_parser ./src/cgi
VPATH = $(PATHSRC)
INCLUDES= -I./ -I./includes -I./src/config_parser -I./src/utils -I./src/request_parser -I./src/cgi
OBJDIR  = objs/

OBJFNAME = $(SRC:.cpp=.o)
OBJS = $(patsubst %,$(OBJDIR)%,$(OBJFNAME))

# $(info SRC=$(SRC))
# $(info VPATH=$(VPATH))
# $(info OBJS=$(OBJS))

$(OBJDIR)%.o: %.cpp
	@echo "Compiling $< -> $@"
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
