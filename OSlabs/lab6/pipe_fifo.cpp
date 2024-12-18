#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctime>
#include <cstdlib>

#define FIFO_PATH "/tmp/my_fifo"

void pipe_example();
void fifo_example();

int main() {
    std::cout << "Starting pipe and fifo examples.\n";

    // Пример работы с pipe
    std::cout << "=== PIPE EXAMPLE ===\n";
    pipe_example();

    // Пример работы с fifo
    std::cout << "\n=== FIFO EXAMPLE ===\n";
    fifo_example();

    return 0;
}

// Функция для передачи через pipe
void pipe_example() {
    int pipe_fd[2]; // file descriptors для pipe
    pid_t pid;
    char buffer[256] = {0};

    // Создаем pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Создаем дочерний процесс
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) { // Родительский процесс
        close(pipe_fd[0]); // Закрываем чтение

        // Получаем текущее время
        time_t now = time(nullptr);
        std::string current_time = ctime(&now);
        current_time.pop_back(); // Удаляем символ новой строки

        // Формируем сообщение
        std::string message = "PID: " + std::to_string(getpid()) +
                              ", Time: " + current_time;

        // Пишем сообщение в pipe
        write(pipe_fd[1], message.c_str(), message.size() + 1);
        close(pipe_fd[1]);

        // Ждем 5 секунд
        sleep(5);
    } else { // Дочерний процесс
        close(pipe_fd[1]); // Закрываем запись

        // Читаем сообщение из pipe
        read(pipe_fd[0], buffer, sizeof(buffer));
        close(pipe_fd[0]);

        // Получаем текущее время
        time_t now = time(nullptr);
        std::string current_time = ctime(&now);
        current_time.pop_back();

        // Выводим полученное сообщение и текущее время
        std::cout << "Child Process Current Time: " << current_time << "\n";
        std::cout << "Received: " << buffer << "\n";
    }
}

// Функция для передачи через fifo
void fifo_example() {
    pid_t pid;
    char buffer[256] = {0};

    // Создаем FIFO
    mkfifo(FIFO_PATH, 0666);

    // Создаем дочерний процесс
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) { // Родительский процесс
        int fifo_fd;

        // Открываем FIFO для записи
        fifo_fd = open(FIFO_PATH, O_WRONLY);

        // Получаем текущее время
        time_t now = time(nullptr);
        std::string current_time = ctime(&now);
        current_time.pop_back(); // Удаляем символ новой строки

        // Формируем сообщение
        std::string message = "PID: " + std::to_string(getpid()) +
                              ", Time: " + current_time;

        // Пишем сообщение в FIFO
        write(fifo_fd, message.c_str(), message.size() + 1);
        close(fifo_fd);

        // Ждем 5 секунд
        sleep(5);

        // Удаляем FIFO
        unlink(FIFO_PATH);
    } else { // Дочерний процесс
        int fifo_fd;

        // Открываем FIFO для чтения
        fifo_fd = open(FIFO_PATH, O_RDONLY);

        // Читаем сообщение из FIFO
        read(fifo_fd, buffer, sizeof(buffer));
        close(fifo_fd);

        // Получаем текущее время
        time_t now = time(nullptr);
        std::string current_time = ctime(&now);
        current_time.pop_back();

        // Выводим полученное сообщение и текущее время
        std::cout << "Child Process Current Time: " << current_time << "\n";
        std::cout << "Received: " << buffer << "\n";
    }
}
