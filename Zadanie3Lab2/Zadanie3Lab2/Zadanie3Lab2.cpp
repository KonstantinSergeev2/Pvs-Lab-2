#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <ctime>

// Функция для последовательного сложения элементов двух массивов
// Принимает два входных массива A и B, результат записывает в C
void sequential_add(const std::vector<double>& A, const std::vector<double>& B, std::vector<double>& C) {
    for (size_t i = 0; i < A.size(); i++) {
        C[i] = A[i] + B[i]; 
    }
}

// Функция для последовательного вычитания элементов двух массивов
// Выполняет вычитание элементов A и B, сохраняя результат в C
void sequential_subtract(const std::vector<double>& A, const std::vector<double>& B, std::vector<double>& C) {
    for (size_t i = 0; i < A.size(); i++) {
        C[i] = A[i] - B[i]; 
    }
}

// Функция для последовательного умножения элементов двух массивов
// Умножает элементы A и B, сохраняя результат в C
void sequential_multiply(const std::vector<double>& A, const std::vector<double>& B, std::vector<double>& C) {
    for (size_t i = 0; i < A.size(); i++) {
        C[i] = A[i] * B[i];
    }
}

// Функция для последовательного деления элементов двух массивов
// Делит элементы A на элементы B, сохраняя результат в C
void sequential_divide(const std::vector<double>& A, const std::vector<double>& B, std::vector<double>& C) {
    for (size_t i = 0; i < A.size(); i++) {
        C[i] = A[i] / B[i];
    }
}

// Функция для параллельного сложения элементов двух массивов
// Использует OpenMP для распараллеливания цикла
void parallel_add(const std::vector<double>& A, const std::vector<double>& B, std::vector<double>& C) {
#pragma omp parallel for
    for (long i = 0; i < static_cast<long>(A.size()); i++) { 
        C[i] = A[i] + B[i];
    }
}

// Функция для параллельного вычитания элементов двух массивов
// Аналогична сложению, но выполняет вычитание
void parallel_subtract(const std::vector<double>& A, const std::vector<double>& B, std::vector<double>& C) {
#pragma omp parallel for
    for (long i = 0; i < static_cast<long>(A.size()); i++) {
        C[i] = A[i] - B[i]; 
    }
}

// Функция для параллельного умножения элементов двух массивов
// Распараллеливает умножение с помощью OpenMP
void parallel_multiply(const std::vector<double>& A, const std::vector<double>& B, std::vector<double>& C) {
#pragma omp parallel for
    for (long i = 0; i < static_cast<long>(A.size()); i++) {
        C[i] = A[i] * B[i]; 
    }
}

// Функция для параллельного деления элементов двух массивов
// Распараллеливает деление с помощью OpenMP
void parallel_divide(const std::vector<double>& A, const std::vector<double>& B, std::vector<double>& C) {
#pragma omp parallel for
    for (long i = 0; i < static_cast<long>(A.size()); i++) { 
        C[i] = A[i] / B[i]; 
    }
}

int main(int argc, char* argv[]) {
    int numThreads; // Переменная для количества потоков
    size_t N;       // Переменная для размера массивов

    if (argc >= 3) {
        numThreads = std::atoi(argv[1]); 
        N = std::atoi(argv[2]);
    }
    else {
        std::cout << "Enter number of threads: ";
        std::cin >> numThreads;
        std::cout << "Enter array size: ";
        std::cin >> N;
    }

    // Проверка, что размер массива больше 100000, как указано в задании
    if (N <= 100000) {
        std::cout << "Array size must be greater than 100000." << std::endl;
    }

    // Установка количества потоков для OpenMP
    omp_set_num_threads(numThreads);

    // Создание трех массивов: A, B — входные, C — для результатов
    std::vector<double> A(N); // Первый входной массив
    std::vector<double> B(N); // Второй входной массив
    std::vector<double> C(N); // Массив для результатов операций

    // Заполнение массивов A и B случайными числами
    srand(time(nullptr));
    for (size_t i = 0; i < N; i++) {
        A[i] = static_cast<double>(rand()) / RAND_MAX; 
        B[i] = static_cast<double>(rand()) / RAND_MAX + 1.0;
    }

    // Список названий операций для вывода результатов
    std::vector<std::string> operations = { "Addition", "Subtraction", "Multiplication", "Division" };

    // Массив указателей на последовательные функции
    using OpFunc = void (*)(const std::vector<double>&, const std::vector<double>&, std::vector<double>&);
    OpFunc seq_funcs[] = { sequential_add, sequential_subtract, sequential_multiply, sequential_divide };

    // Массив указателей на параллельные функции
    OpFunc par_funcs[] = { parallel_add, parallel_subtract, parallel_multiply, parallel_divide };

    // Вывод начальных параметров
    std::cout << "Number of threads: " << numThreads << ", Array size: " << N << std::endl;

    // Цикл по всем четырём операциям (сложение, вычитание, умножение, деление)
    for (int op = 0; op < 4; op++) {
        std::cout << "\nOperation: " << operations[op] << std::endl;

        // Замер времени для последовательной версии
        auto start = std::chrono::high_resolution_clock::now(); // Начало замера
        seq_funcs[op](A, B, C);                                 // Выполнение операции
        auto end = std::chrono::high_resolution_clock::now();   // Конец замера
        std::chrono::duration<double> elapsed_seq = end - start; // Время выполнения
        double sum_seq = 0.0;                                   // Сумма для проверки результата
        for (const auto& val : C) sum_seq += val;               // Суммируем элементы
        std::cout << "Sequential time: " << std::fixed << std::setprecision(6) << elapsed_seq.count()
            << " seconds, sum: " << sum_seq << std::endl;

        // Замер времени для параллельной версии
        start = std::chrono::high_resolution_clock::now();      // Начало замера
        par_funcs[op](A, B, C);                                 // Выполнение операции
        end = std::chrono::high_resolution_clock::now();        // Конец замера
        std::chrono::duration<double> elapsed_par = end - start; // Время выполнения
        double sum_par = 0.0;                                   // Сумма для проверки результата
        for (const auto& val : C) sum_par += val;               // Суммируем элементы
        std::cout << "Parallel time:   " << std::fixed << std::setprecision(6) << elapsed_par.count()
            << " seconds, sum: " << sum_par << std::endl;
    }

    return 0; // Завершение программы
}