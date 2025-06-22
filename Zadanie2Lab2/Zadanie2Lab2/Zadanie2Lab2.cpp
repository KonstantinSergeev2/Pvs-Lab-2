#include <iostream>
#include <algorithm>
#include <omp.h>
#include <cstdlib>
#include <chrono>

// Функция разбиения массива по опорному элементу для быстрой сортировки
int partition(int* arr, int low, int high) {
    int pivot = arr[high];  // Опорный элемент — последний в диапазоне
    int i = low - 1;        // Индекс элемента меньшего или равного опорному

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);  // Помещаем опорный на своё место
    return i + 1;                       // Возвращаем индекс опорного
}

// Последовательная реализация быстрой сортировки
void quickSort_sequential(int* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort_sequential(arr, low, pi - 1);
        quickSort_sequential(arr, pi + 1, high);
    }
}

// Параллельная быстрая сортировка с использованием OpenMP задач
void quickSort_parallel(int* arr, int low, int high, int depth = 0) {
    if (low < high) {
        int pi = partition(arr, low, high);

        // Создаём параллельные задачи, если глубина меньше максимального количества потоков
        if (depth < omp_get_max_threads()) {
#pragma omp task
            quickSort_parallel(arr, low, pi - 1, depth + 1);
#pragma omp task
            quickSort_parallel(arr, pi + 1, high, depth + 1);
#pragma omp taskwait
        }
        else {
            // При большой глубине рекурсии выполняем последовательно, чтобы не создавать лишних задач
            quickSort_parallel(arr, low, pi - 1, depth);
            quickSort_parallel(arr, pi + 1, high, depth);
        }
    }
}

int main(int argc, char* argv[]) {
    int numThreads;

    // Получение количества потоков из аргументов или через ввод
    if (argc >= 2) {
        numThreads = std::atoi(argv[1]);
    }
    else {
        std::cout << "Enter number of threads: ";
        std::cin >> numThreads;
    }

    omp_set_num_threads(numThreads);

    // Размер массива: более 100000, можно менять
    const size_t SIZE = 500000;

    std::cout << "Array size: " << SIZE << std::endl;
    std::cout << "Threads: " << numThreads << std::endl;

    // Создаём два массива для теста: один для последовательной сортировки, второй — для параллельной
    int* arr_seq = new int[SIZE];
    int* arr_par = new int[SIZE];

    // Инициализируем одинаковыми случайными числами
    for (size_t i = 0; i < SIZE; ++i) {
        int val = rand() % 10000;
        arr_seq[i] = val;
        arr_par[i] = val;
    }

    // Измеряем время последовательной сортировки
    auto start_seq = std::chrono::high_resolution_clock::now();
    quickSort_sequential(arr_seq, 0, SIZE - 1);
    auto end_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seq = end_seq - start_seq;

    // Измеряем время параллельной сортировки с использованием OpenMP
    auto start_par = std::chrono::high_resolution_clock::now();
#pragma omp parallel
    {
#pragma omp single
        quickSort_parallel(arr_par, 0, SIZE - 1);
    }
    auto end_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_par = end_par - start_par;

    // Выводим время выполнения обеих сортировок
    std::cout << "Sequential sort time: " << elapsed_seq.count() << " seconds" << std::endl;
    std::cout << "Parallel sort time:   " << elapsed_par.count() << " seconds" << std::endl;

    // Освобождаем память
    delete[] arr_seq;
    delete[] arr_par;

    return 0;
}