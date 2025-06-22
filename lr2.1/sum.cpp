#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <omp.h>

void verify_sort(const std::vector<int>& arr) {
    if (!std::is_sorted(arr.begin(), arr.end())) {
        std::cerr << "Sorting verification FAILED!" << std::endl;
        exit(1);
    }
}

void sequential_sort(std::vector<int>& arr) {
    std::sort(arr.begin(), arr.end());
}

void parallel_sort(std::vector<int>& arr, int threads) {
    if (threads <= 1 || arr.size() <= 10000) {
        std::sort(arr.begin(), arr.end());
        return;
    }

    size_t chunk = arr.size() / threads;

#pragma omp parallel num_threads(threads)
    {
#pragma omp single nowait
        {
            for (int i = 0; i < threads; i++) {
                size_t begin = i * chunk;
                size_t end = (i == threads - 1) ? arr.size() : begin + chunk;

#pragma omp task
                std::sort(arr.begin() + begin, arr.begin() + end);
            }
        }
    }

    // Финальное слияние
    for (int step = chunk; step < arr.size(); step *= 2) {
        for (size_t i = 0; i < arr.size(); i += 2 * step) {
            size_t mid = std::min(i + step, arr.size());
            size_t end = std::min(i + 2 * step, arr.size());
            std::inplace_merge(arr.begin() + i, arr.begin() + mid, arr.begin() + end);
        }
    }
}

int main() {
    const int size = 1000000;
    std::vector<int> base_arr(size);
    std::srand(42);
    std::generate(base_arr.begin(), base_arr.end(), []() { return std::rand() % 100000; });

    std::cout << "Array size: " << size << "\n========================" << std::endl;

    for (int threads : {1, 2, 4, 8}) {
        auto arr = base_arr;

        double seq_time, par_time;

        // Последовательная сортировка
        auto start = std::chrono::high_resolution_clock::now();
        sequential_sort(arr);
        auto end = std::chrono::high_resolution_clock::now();
        seq_time = std::chrono::duration<double>(end - start).count();
        verify_sort(arr);

        // Параллельная сортировка
        arr = base_arr;
        start = std::chrono::high_resolution_clock::now();
        parallel_sort(arr, threads);
        end = std::chrono::high_resolution_clock::now();
        par_time = std::chrono::duration<double>(end - start).count();
        verify_sort(arr);

        std::cout << "Threads: " << threads
            << " | Seq: " << seq_time << "s"
            << " | Par: " << par_time << "s"
            << " | Speedup: " << seq_time / par_time << "x\n"
            << "------------------------" << std::endl;
    }

    // Ожидание нажатия клавиши
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}
