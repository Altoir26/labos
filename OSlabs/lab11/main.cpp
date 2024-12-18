#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstring>

const int ARRAY_SIZE = 10; // Размер массива
char shared_array[ARRAY_SIZE]; // Общий массив символов
std::atomic<int> write_counter(0); // Счетчик записей
std::condition_variable condvar;
std::mutex mutex; // Мьютекс для синхронизации условной переменной
bool is_written = false; // Флаг записи

// Функция для пишущего потока
void writer() {
    while (write_counter < 50) { // Максимум 50 записей
        std::unique_lock<std::mutex> lock(mutex); // Захват мьютекса
        condvar.wait(lock, [] { return !is_written; }); // Ждать, пока массив доступен для записи

        // Запись в массив
        std::memset(shared_array, ' ', ARRAY_SIZE - 1);
        shared_array[ARRAY_SIZE - 1] = '\0';
        snprintf(shared_array, ARRAY_SIZE, "%d", write_counter++);
        is_written = true;

        // Уведомляем читающие потоки
        condvar.notify_all();
    }
}

// Функция для читающих потоков
void reader(int tid) {
    while (write_counter <= 50) {
        std::unique_lock<std::mutex> lock(mutex); // Захват мьютекса
        condvar.wait(lock, [] { return is_written; }); // Ждать, пока массив доступен для чтения

        // Чтение массива
        std::cout << "Reader " << tid << ": " << shared_array << std::endl;

        is_written = false;

        // Уведомляем пишущий поток
        condvar.notify_one();
    }
}

int main() {
    // Создаем потоки
    std::thread writer_thread(writer);
    std::vector<std::thread> reader_threads;

    for (int i = 0; i < 10; ++i) {
        reader_threads.emplace_back(reader, i);
    }

    // Ожидаем завершения потоков
    writer_thread.join();
    for (auto &thread : reader_threads) {
        thread.join();
    }

    return 0;
}
