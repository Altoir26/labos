# Имя исполняемого файла
TARGET = main

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra

# Правило сборки
all: $(TARGET)

$(TARGET): main.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

# Очистка
clean:
	rm -f $(TARGET) *.o
