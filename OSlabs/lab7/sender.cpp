#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <cstring>
#include <ctime>
#include <csignal>

#define SHM_KEY 1234  // Ключ для разделяемой памяти
#define LOCK_FILE "/tmp/sender.lock"  // Файл для блокировки

// Структура для передачи данных через разделяемую память
struct SharedData {
    bool data_ready;     // Флаг готовности данных
    pid_t sender_pid;    // PID отправителя
    char time_str[64];   // Строка с текущим временем
};

int main() {
    // Попытка создать блокировку для проверки единственного запуска
    int lock_fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("Ошибка открытия файла блокировки");
        return 1;
    }
    if (flock(lock_fd, LOCK_EX | LOCK_NB) == -1) {
        std::cerr << "Ошибка: Передающая программа уже запущена." << std::endl;
        return 1;
    }

    // Создание разделяемой памяти
    int shm_id = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Ошибка создания разделяемой памяти");
        return 1;
    }

    // Подключение разделяемой памяти
    SharedData *shared_data = static_cast<SharedData*>(shmat(shm_id, nullptr, 0));
    if (shared_data == reinterpret_cast<SharedData*>(-1)) {
        perror("Ошибка подключения разделяемой памяти");
        return 1;
    }

    std::cout << "Передающая программа запущена. PID: " << getpid() << std::endl;

    // Инициализация данных
    shared_data->data_ready = false;

    while (true) {
        // Получение текущего времени
        time_t now = time(nullptr);
        struct tm *current_time = localtime(&now);
        strftime(shared_data->time_str, sizeof(shared_data->time_str), "%Y-%m-%d %H:%M:%S", current_time);

        shared_data->sender_pid = getpid();
        shared_data->data_ready = true;  // Устанавливаем флаг готовности данных

        std::cout << "Отправлено: " << shared_data->time_str << ", PID: " << shared_data->sender_pid << std::endl;

        sleep(1);  // Задержка в 1 секунду
    }

    // Очистка ресурсов (при завершении программы)
    shmdt(shared_data);
    shmctl(shm_id, IPC_RMID, nullptr);
    close(lock_fd);
    return 0;
}
