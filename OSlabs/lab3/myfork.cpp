#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>

// Обработчик для atexit
void atexitHandler() {
    std::cout << "Process " << getpid() << " exiting via atexit handler." << std::endl;
}

// Обработчик сигнала SIGINT
void handleSigint(int signal) {
    std::cout << "Caught signal SIGINT (" << signal << ") in process " << getpid() << "." << std::endl;
    exit(EXIT_SUCCESS);
}

// Обработчик сигнала SIGTERM
void handleSigterm(int signal, siginfo_t* info, void* context) {
    std::cout << "Caught signal SIGTERM (" << signal << ") in process " << getpid() << "."
              << " Signal sent by PID: " << info->si_pid << "." << std::endl;
    exit(EXIT_SUCCESS);
}

int main() {
    // Регистрация обработчика atexit
    if (atexit(atexitHandler) != 0) {
        std::cerr << "Failed to register atexit handler." << std::endl;
        return EXIT_FAILURE;
    }

    // Установка обработчика SIGINT через signal()
    if (signal(SIGINT, handleSigint) == SIG_ERR) {
        std::cerr << "Failed to register SIGINT handler." << std::endl;
        return EXIT_FAILURE;
    }

    // Установка обработчика SIGTERM через sigaction
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handleSigterm;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        std::cerr << "Failed to register SIGTERM handler." << std::endl;
        return EXIT_FAILURE;
    }

    // Вызов fork()
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Fork failed." << std::endl;
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // Дочерний процесс
        std::cout << "Child process started (PID: " << getpid() << ")." << std::endl;
        while (true) {
            // Бесконечный цикл для ожидания сигналов
            pause();
        }
    } else {
        // Родительский процесс
        std::cout << "Parent process (PID: " << getpid() << "), spawned child PID: " << pid << "." << std::endl;
        while (true) {
            // Бесконечный цикл для ожидания сигналов
            pause();
        }
    }

    return 0;
}
