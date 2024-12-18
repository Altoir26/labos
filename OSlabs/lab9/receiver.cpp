#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <cstring>
#include <ctime>

#define SHM_KEY 1234    // Ключ для разделяемой памяти
#define SEM_KEY 5678    // Ключ для семафора
#define SHM_SIZE 256    // Размер разделяемой памяти

using namespace std;

struct SharedData {
    pid_t sender_pid;
    char message[100];
};

int main() {
    // Получаем доступ к семафору
    int sem_id = semget(SEM_KEY, 1, 0666);
    if (sem_id == -1) {
        cerr << "Ошибка: передающая программа не запущена!" << endl;
        return 1;
    }

    // Получаем доступ к разделяемой памяти
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shm_id == -1) {
        cerr << "Ошибка: не удалось подключиться к разделяемой памяти!" << endl;
        return 1;
    }

    // Присоединяем разделяемую память
    SharedData* shm_data = (SharedData*)shmat(shm_id, NULL, 0);
    if (shm_data == (SharedData*)-1) {
        perror("shmat");
        return 1;
    }

    cout << "Принимающая программа запущена. PID: " << getpid() << endl;

    // Бесконечный цикл чтения данных
    while (true) {
        // Захватываем семафор
        struct sembuf lock = {0, -1, 0};
        semop(sem_id, &lock, 1);

        // Выводим полученные данные
        time_t current_time = time(NULL);
        cout << "PID: " << getpid()
             << " | Время: " << ctime(&current_time)
             << " | Сообщение от PID " << shm_data->sender_pid << ": " << shm_data->message << endl;

        // Освобождаем семафор
        struct sembuf unlock = {0, 1, 0};
        semop(sem_id, &unlock, 1);

        sleep(1); // Задержка для наглядности
    }

    // Отключаем разделяемую память
    shmdt(shm_data);
    return 0;
}
