CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src/MyShell.c src/history.c src/input.c src/builtins.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = cshell

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) ./.cshell_history