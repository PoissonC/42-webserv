# Compiler and flags
CC 			= c++
CFLAGS 		= -std=c++98 -Wall -Wextra -Werror -fsanitize=address

# Directories
OBJ_DIR		= obj/

# Sources and objects
SRC 		= main.cpp ServerPrivateMemberFuncs.cpp ServerPublicMemberFuncs.cpp Server_helper.cpp Settings.cpp \
				ServerConfig.cpp LocationConfig.cpp State.cpp \
				parser.cpp Request.cpp Response.cpp handle_request.cpp handle_stages.cpp\
				read_request.cpp send_response.cpp read_file.cpp read_cgi.cpp save_file.cpp\
				get_mime.cpp handle_error_response.cpp
OBJ 		= $(SRC:%.cpp=$(OBJ_DIR)%.o)
HDR 		= Server.hpp Server_helper.hpp Settings.hpp State.hpp ServerConfig.hpp \
				LocationConfig.hpp parser.hpp Request.hpp Response.hpp MiddleStages.hpp \
				handle_error_response.hpp handle_stages.hpp

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