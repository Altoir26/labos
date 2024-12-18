CC = g++
CFLAGS = -Wall -std=c++17
TARGET = pipe_fifo

all: $(TARGET)

$(TARGET): pipe_fifo.cpp
	$(CC) $(CFLAGS) -o $(TARGET) pipe_fifo.cpp

clean:
	rm -f $(TARGET)
