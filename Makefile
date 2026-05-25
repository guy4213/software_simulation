CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

all: simulation

simulation: $(SRCS)
	$(CC) $(CFLAGS) -o simulation $(SRCS)

clean:
	rm -f src/*.o simulation
