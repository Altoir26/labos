#include <iostream>
#include <fstream>
#include <string>

// Функция для вывода информации о правильном использовании программы
void print_usage() {
    std::cout << "Usage: ./mycat [-n|-b|-E] <file>" << std::endl;
}

int main(int argc, char* argv[]) {
    // Переменные для флагов опций
    bool number_lines = false;       // Нумерация всех строк
    bool number_non_blank = false;  // Нумерация только непустых строк
    bool show_ends = false;          // Отображение символа $ в конце каждой строки
    std::string file_name;           // Имя файла для чтения

    // Проверяем, переданы ли аргументы командной строки
    if (argc < 2) {
        print_usage(); // Выводим инструкцию, если аргументов недостаточно
        return 1;
    }

    // Обрабатываем аргументы командной строки
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-n") {
            number_lines = true; // Устанавливаем флаг для нумерации строк
        } else if (arg == "-b") {
            number_non_blank = true; // Устанавливаем флаг для нумерации непустых строк
        } else if (arg == "-E") {
            show_ends = true; // Устанавливаем флаг для отображения символа $ в конце строк
        } else {
            file_name = arg; // Сохраняем имя файла
        }
    }

    // Проверяем, указано ли имя файла
    if (file_name.empty()) {
        print_usage(); // Выводим инструкцию, если файл не указан
        return 1;
    }

    // Открываем файл для чтения
    std::ifstream file(file_name);
    if (!file.is_open()) {
        // Если файл не удалось открыть, выводим сообщение об ошибке
        std::cerr << "Error: Could not open file " << file_name << std::endl;
        return 1;
    }

    std::string line;       // Переменная для хранения строки из файла
    int line_number = 0;    // Счётчик строк

    // Читаем файл построчно
    while (std::getline(file, line)) {
        line_number++; // Увеличиваем номер строки

        // Если включена нумерация только непустых строк и строка пустая
        if (number_non_blank && line.empty()) {
            if (show_ends) {
                std::cout << "$" << std::endl; // Отображаем только символ $ при пустой строке
            } else {
                std::cout << std::endl; // Печатаем пустую строку
            }
            continue; // Переходим к следующей строке
        }

        // Печатаем номер строки, если включена любая нумерация
        if (number_lines || (number_non_blank && !line.empty())) {
            std::cout << line_number << "\t"; // Номер строки с табуляцией
        }

        // Печатаем саму строку
        std::cout << line;

        // Если включено отображение символа $, добавляем его в конец строки
        if (show_ends) {
            std::cout << "$";
        }
        std::cout << std::endl; // Завершаем текущую строку
    }

    // Закрываем файл
    file.close();
    return 0;
}
