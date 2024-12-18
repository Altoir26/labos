#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

// Константы
const int NUM_READERS = 10;      // Количество читающих потоков
const int ARRAY_SIZE = 20;       // Размер общего массива символов
const int WRITE_DELAY = 500;     // Задержка в миллисекундах для пишущего потока
const int READ_DELAY = 200;      // Задержка в миллисекундах для читающих потоков

// Общий массив и синхронизация доступа
char shared_array[ARRAY_SIZE];   // Общий массив символов
int write_counter = 0;           // Счетчик записей
std::mutex mtx;                  // Мьютекс для синхронизации доступа

// Функция пишущего потока
void writer_thread() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(WRITE_DELAY)); // Задержка перед записью

        std::lock_guard<std::mutex> lock(mtx);  // Блокировка мьютекса
        // Заполняем массив символами на основе текущего значения write_counter
        for (int i = 0; i < ARRAY_SIZE; ++i) {
            shared_array[i] = 'A' + (write_counter % 26);  // Циклическая запись символов A-Z
        }
        ++write_counter;  // Увеличиваем счетчик записей

        // Сообщаем о текущей записи
        std::cout << "[Писатель] Записал данные: " << std::string(shared_array, ARRAY_SIZE)
                  << ", Счетчик записи: " << write_counter << std::endl;
    }
}

// Функция читающих потоков
void reader_thread(int thread_id) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(READ_DELAY)); // Задержка перед чтением

        std::lock_guard<std::mutex> lock(mtx);  // Блокировка мьютекса
        // Чтение текущего состояния массива
        std::cout << "[Читатель " << thread_id << "] Прочитал массив: "
                  << std::string(shared_array, ARRAY_SIZE) << std::endl;
    }
}

int main() {
    std::cout << "Запуск программы. Создаем 10 читающих потоков и 1 пишущий поток.\n";

    // Создание потоков
    std::thread writer(writer_thread);  // Пишущий поток
    std::vector<std::thread> readers;   // Вектор для читающих потоков

    for (int i = 0; i < NUM_READERS; ++i) {
        readers.emplace_back(reader_thread, i + 1);  // Создаем читающий поток с идентификатором
    }

    // Ожидание завершения потоков (бесконечный цикл, поэтому ждем их "вечно")
    writer.join();
    for (auto& reader : readers) {
        reader.join();
    }

    return 0;
}
