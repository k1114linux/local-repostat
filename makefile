CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c11
TARGET = repostat

all: $(TARGET)

$(TARGET): repostat.c
	$(CC) $(CFLAGS) -o $(TARGET) repostat.c

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -m755 $(TARGET) /usr/local/bin/$(TARGET)

.PHONY: all clean install
