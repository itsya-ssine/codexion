NAME     = codexion
CC       = cc
CFLAGS   = -Wall -Wextra -Werror -pthread
INCLUDES = -I include/

SRCS     = src/main.c src/args_parser.c src/validation.c \
           src/simulation_init.c src/mutex_utils.c \
           src/simulation_cleanup.c src/simulation_run.c \
           src/coder.c src/coder_helpers.c src/dongle.c \
           src/monitor.c src/heap.c src/heap_operations.c \
           src/logger.c src/time_utils.c

HEADER   = include/codexion.h

OBJS     = $(patsubst src/%.c, objects/%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

objects/%.o: src/%.c $(HEADER)
	mkdir -p objects
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf objects

fclean: clean
	rm -f $(NAME)

re: fclean all
