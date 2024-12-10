# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: htouil <htouil@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/15 23:36:12 by htouil            #+#    #+#              #
#    Updated: 2024/12/10 14:41:56 by htouil           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= ircserv

SRCS	= main.cpp Server/Server.cpp Client/Client.cpp Channel/Channel.cpp Commands.cpp Helper.cpp

OBJS	= ${SRCS:.cpp=.o}

CPPC	= c++

FLAGS	= -Wall -Wextra -Werror -std=c++98 -ggdb3

.PHONY	= all clean fclean re

all: $(NAME)

%.o: %.cpp
	$(CPPC) $(FLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CPPC) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
