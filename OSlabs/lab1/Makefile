cc = gcc
CFLAGS = -Wall -Wextra -pedantic -g
TARGET = MyLS

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(FLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	rm -f $(TARGET)

.PHONY: all clean
