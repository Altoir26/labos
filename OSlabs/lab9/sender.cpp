#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define SHM_KEY 1234    // Ключ для разделяемой памяти
#define SEM_KEY 5678    // Ключ для семафора
#define SHM_SIZE 256    // Размер разделяемой памяти

using namespace std;

struct SharedData {
    pid_t sender_pid;
    char message[100];
};

int check_running_instance();

int main() {
    // Проверка на наличие уже работающего экземпляра
    if (check_running_instance()) {
        cerr << "Передающая программа уже запущена! Завершение работы." << endl;
        return 1;
    }

    // Создаем семафор
    int sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        return 1;
    }
    semctl(sem_id, 0, SETVAL, 1); // Устанавливаем начальное значение семафора

    // Создаем разделяемую память
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        return 1;
    }

    // Присоединяем разделяемую память
    SharedData* shm_data = (SharedData*)shmat(shm_id, NULL, 0);
    if (shm_data == (SharedData*)-1) {
        perror("shmat");
        return 1;
    }

    cout << "Передающая программа запущена. PID: " << getpid() << endl;

    // Бесконечный цикл передачи данных
    while (true) {
        // Захватываем семафор
        struct sembuf lock = {0, -1, 0};
        semop(sem_id, &lock, 1);

        // Заполняем данные
        shm_data->sender_pid = getpid();
        time_t current_time = time(NULL);
        snprintf(shm_data->message, sizeof(shm_data->message), "Текущее время: %s", ctime(&current_time));
        shm_data->message[strlen(shm_data->message) - 1] = '\0'; // Удаляем символ новой строки

        // Освобождаем семафор
        struct sembuf unlock = {0, 1, 0};
        semop(sem_id, &unlock, 1);

        sleep(1); // Задержка в 1 секунду
    }

    // Отключаем разделяемую память
    shmdt(shm_data);
    return 0;
}

// Функция проверки наличия уже запущенного экземпляра
int check_running_instance() {
    int fd = open("/tmp/sender.lock", O_CREAT | O_EXCL, 0666);
    if (fd == -1) {
        if (errno == EEXIST) {
            return 1;
        }
    }
    return 0;
}
