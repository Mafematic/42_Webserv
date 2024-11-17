CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Wshadow
TARGET = Webserver
SRCS = main.cpp \
		Networking/SimpleSocket.cpp \
		Networking/BindingSocket.cpp \
		Networking/ConnectingSocket.cpp \
		Networking/ListeningSocket.cpp \
		Servers/Server.cpp \
		Request/Request.cpp \
		RequestRouter/RequestRouter.cpp \
		FileUploader/Uploader.cpp

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(TARGET)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
