CC = g++
CFLAGS = -Wall -std=c++11
TARGETS = sender receiver

all: $(TARGETS)

sender: sender.cpp
	$(CC) $(CFLAGS) -o sender sender.cpp

receiver: receiver.cpp
	$(CC) $(CFLAGS) -o receiver receiver.cpp

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
