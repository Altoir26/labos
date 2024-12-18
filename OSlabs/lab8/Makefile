# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++11 -pthread -Wall -Wextra

# Исполняемый файл
TARGET = threads_app

# Исходные файлы
SRC = main.cpp

# Правило по умолчанию
all: $(TARGET)

# Компиляция программы
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Очистка
clean:
	rm -f $(TARGET)

# Фазовая цель
.PHONY: all clean
