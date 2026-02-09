CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

NAME = ircserv

OBJ_DIR = obj

SRC =	srcs/main.cpp \
		srcs/Exception.cpp \
		srcs/Server.cpp \
		srcs/Client.cpp \
		srcs/NickCmd.cpp \
		srcs/UserCmd.cpp 

OBJ = $(SRC:srcs/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: ./srcs/%.cpp
	mkdir -p $(dir $@);
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re

.SILENT: