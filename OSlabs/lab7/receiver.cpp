#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <ctime>

#define SHM_KEY 1234  // Ключ для разделяемой памяти

// Структура для получения данных через разделяемую память
struct SharedData {
    bool data_ready;     // Флаг готовности данных
    pid_t sender_pid;    // PID отправителя
    char time_str[64];   // Строка с текущим временем
};

int main() {
    // Подключение к существующей разделяемой памяти
    int shm_id = shmget(SHM_KEY, sizeof(SharedData), 0666);
    if (shm_id == -1) {
        perror("Ошибка подключения к разделяемой памяти");
        return 1;
    }

    SharedData *shared_data = static_cast<SharedData*>(shmat(shm_id, nullptr, 0));
    if (shared_data == reinterpret_cast<SharedData*>(-1)) {
        perror("Ошибка подключения разделяемой памяти");
        return 1;
    }

    std::cout << "Принимающая программа запущена. PID: " << getpid() << std::endl;

    while (true) {
        // Проверка готовности данных
        if (shared_data->data_ready) {
            // Получаем текущее время
            time_t now = time(nullptr);
            struct tm *current_time = localtime(&now);
            char current_time_str[64];
            strftime(current_time_str, sizeof(current_time_str), "%Y-%m-%d %H:%M:%S", current_time);

            std::cout << "Принято: " << shared_data->time_str
                      << ", Отправитель PID: " << shared_data->sender_pid
                      << ", Текущее время: " << current_time_str
                      << ", Мой PID: " << getpid() << std::endl;

            shared_data->data_ready = false;  // Сбрасываем флаг
        }
        usleep(500000);  // Пауза 0.5 секунды
    }

    // Очистка ресурсов
    shmdt(shared_data);
    return 0;
}
