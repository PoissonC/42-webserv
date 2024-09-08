# Compiler and flags
CC 			= c++ -g
CFLAGS 		= -std=c++98 -Wall -Wextra -Werror #-fsanitize=address

# Directories
UTILS_DIR	= utils/
PAGES_DIR	= pages/
OBJ_DIR		= obj/

# Sources and objects
SRC 		= main.cpp ServerPrivateMemberFuncs.cpp ServerPublicMemberFuncs.cpp Server_helper.cpp Settings.cpp \
				ServerConfig.cpp LocationConfig.cpp State.cpp helper.cpp handle_request.cpp\
				parser.cpp Request.cpp Response.cpp handle_stages.cpp\
				read_request.cpp send_response.cpp read_file.cpp read_cgi.cpp save_file.cpp write_cgi.cpp\
				handle_error.cpp
SRC			+= $(addprefix $(UTILS_DIR), getStatusMessageFromCode.cpp HtmlMarkup.cpp normalizeDirectoryPath.cpp)
SRC			+= $(addprefix $(PAGES_DIR), getIndexPage.cpp getDirectoryPage.cpp getErrorPage.cpp getHeaderContent.cpp)
OBJ 		= $(addprefix $(OBJ_DIR), $(SRC:%.cpp=%.o))
HDR 		= constants.hpp Server.hpp Server_helper.hpp Settings.hpp State.hpp ServerConfig.hpp helper.hpp\
				LocationConfig.hpp parser.hpp Request.hpp Response.hpp MiddleStages.hpp handle_error.hpp

HDR			+= $(addprefix $(UTILS_DIR), utils.hpp HtmlMarkup.hpp)
HDR			+= $(addprefix $(PAGES_DIR), pages.hpp)

NAME = webserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)%.o: %.cpp $(HDR) Makefile
	@mkdir -p $(dir $@)
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