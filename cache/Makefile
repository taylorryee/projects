CC = clang
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99

SRCS = List.c cacher.c   # Add your source files here
OBJS = $(SRCS:.c=.o)

TARGET = cacher

.PHONY: all clean format

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

format:
	clang-format -i List.c List.h cacher.c


