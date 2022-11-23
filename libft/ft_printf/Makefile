# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vkuikka <vkuikka@student.hive.fi>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/12/08 13:46:11 by vkuikka           #+#    #+#              #
#    Updated: 2022/02/08 22:04:46 by vkuikka          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libftprintf.a
FILES = ft_printf.c floats.c printing.c output_num_char.c output_int.c length.c input.c skip_arg.c
OBJ = $(FILES:.c=.o)
LIBFT = ../libft.a
INCLUDE = ../includes
.PHONY: clean fclean re all

all: $(NAME)

$(NAME):
	@cp $(LIBFT) $(NAME)
	@gcc -c -Wall -Wextra -Werror $(FILES) -I $(INCLUDE)
	@ar rc $(NAME) $(OBJ)
	@ranlib $(NAME)

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean
	@make all
