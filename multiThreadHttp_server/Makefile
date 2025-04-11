EXECBIN  = httpserver
SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:.c=.o)
FORMATS  = $(SOURCES:.c=.fmt)
FORMAT_FILE = Filelock.h

CC       = clang
FORMAT   = clang-format
CFLAGS   = -Wall -Wpedantic -Werror -Wextra

# Add the helper functions library and header
HELPERLIB = asgn4_helper_funcs.a


.PHONY: all clean format

all: $(EXECBIN)

$(EXECBIN): $(OBJECTS) $(HELPERLIB)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXECBIN) $(OBJECTS)

format: $(FORMATS) format_filelock

%.fmt: %.c
	$(FORMAT) -i $<
	touch $@

format_filelock:
	$(FORMAT) -i $(FORMAT_FILE)

