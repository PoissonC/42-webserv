# Compiler and flags
CC 			= g++ -g
CFLAGS 		= -std=c++98 -Wall -Wextra -Werror #-fsanitize=address

# Directories
OBJ_DIR		= obj/

# Sources and objects
SRC 		= main.cpp Server.cpp Server_helper.cpp Settings.cpp \
				ServerConfig.cpp LocationConfig.cpp \
				parser.cpp httpRequestParser.cpp processRequest.cpp
OBJ 		= $(SRC:%.cpp=$(OBJ_DIR)%.o)
HDR 		= Server.hpp Server_helper.hpp Settings.hpp State.hpp ServerConfig.hpp \
				LocationConfig.hpp parser.hpp httpRequestParser.hpp

NAME = webserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)%.o: %.cpp $(HDR) Makefile
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

# Linter
lint:
	clang-format -i *.cpp *.hpp

# Clean up object files
clean:
	rm -fr $(OBJ_DIR)

# Clean up object files and the library
fclean: clean
	rm -f $(NAME)

# Rebuild everything
re: fclean all

# By default, just run 'make all'
.DEFAULT_GOAL := all