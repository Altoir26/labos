#include <iostream>
#include <string>
#include <sys/stat.h> // для chmod
#include <unistd.h>   // для доступа к файлам
#include <cerrno>     // для обработки ошибок
#include <cstring>    // для strerror

// Функция для вывода ошибки и завершения программы
void showErrorAndExit(const std::string &message) {
    std::cerr << "Ошибка: " << message << " (" << strerror(errno) << ")" << std::endl;
    exit(EXIT_FAILURE);
}

// Основная функция
int main(int argc, char *argv[]) {
    // Проверяем количество аргументов
    if (argc != 3) {
        std::cerr << "Использование: ./mychmod <режим> <файл>" << std::endl;
        std::cerr << "Пример: ./mychmod +x file.txt" << std::endl;
        return EXIT_FAILURE;
    }

    std::string mode = argv[1]; // Режим изменения прав
    std::string filePath = argv[2]; // Путь к файлу

    // Проверяем, существует ли файл
    if (access(filePath.c_str(), F_OK) != 0) {
        showErrorAndExit("Файл не найден");
    }

    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0) {
        showErrorAndExit("Не удалось получить информацию о файле");
    }

    mode_t permissions = fileStat.st_mode; // Текущие права доступа

    // Обработка режима
    if (mode[0] == '+' || mode[0] == '-' || mode[0] == 'u' || mode[0] == 'g' || mode[0] == 'o') {
        if (mode == "+x") {
            permissions |= S_IXUSR; // Добавить право на выполнение для владельца
        } else if (mode == "u-r") {
            permissions &= ~S_IRUSR; // Убрать право на чтение для владельца
        } else if (mode == "g+rw") {
            permissions |= (S_IRGRP | S_IWGRP); // Добавить права на чтение и запись для группы
        } else {
            showErrorAndExit("Неверный формат команды");
        }
    } else if (mode.size() == 3 && std::isdigit(mode[0]) && std::isdigit(mode[1]) && std::isdigit(mode[2])) {
        // Устанавливаем права через числовое значение (например, 766)
        permissions = std::stoi(mode, 0, 8); // Преобразуем строку в число в восьмеричной системе
    } else {
        showErrorAndExit("Неверный формат команды");
    }

    // Применяем права
    if (chmod(filePath.c_str(), permissions) != 0) {
        showErrorAndExit("Не удалось изменить права файла");
    }

    std::cout << "Права успешно изменены" << std::endl;
    return EXIT_SUCCESS;
}
