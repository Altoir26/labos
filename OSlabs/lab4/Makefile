# Имя исполняемого файла
TARGET = mychmod

# Компилятор и флаги
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17

# Правило сборки
all: $(TARGET)

$(TARGET): mychmod.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) mychmod.cpp

# Очистка
clean:
	rm -f $(TARGET)

# Правило установки (опционально)
install:
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean install
