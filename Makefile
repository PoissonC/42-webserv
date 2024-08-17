# Compiler and flags
CC 			= c++
CFLAGS 		= -Wall -Wextra -Werror -std=c++98 -fsanitize=address

#Sources and objects
SRC = main.cpp Server.cpp Settings.cpp State.cpp ServerConfig.cpp LocationConfig.cpp parser.cpp httpRequestParser.cpp processRequest.cpp
OBJ = $(SRC:.cpp=.o)
HDR = Server.hpp Settings.hpp State.hpp ServerConfig.hpp LocationConfig.hpp parser.hpp httpRequestParser.hpp

NAME = webserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp $(HDR) Makefile
	$(CC) $(CFLAGS) -o $@ -c $<

# Clean up object files
clean:
	rm -f $(OBJ)
# Clean up object files and the library
fclean: clean
	rm -f $(NAME)
# Rebuild everything
re: fclean all
# By default, just run 'make all'
.DEFAULT_GOAL := all