#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void init_arrays(float** a, float** b, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            a[i][j] = (float)(rand() % 100 + 1);
            b[i][j] = (float)(rand() % 100 + 1);
        }
}

void operations_serial(float** a, float** b, float** add, float** sub, float** mul, float** div, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            add[i][j] = a[i][j] + b[i][j];
            sub[i][j] = a[i][j] - b[i][j];
            mul[i][j] = a[i][j] * b[i][j];
            div[i][j] = a[i][j] / b[i][j];
        }
}

void operations_parallel(float** a, float** b, float** add, float** sub, float** mul, float** div, int n) {
    int size = n * n;
#pragma omp parallel for
    for (int idx = 0; idx < size; idx++) {
        int i = idx / n;
        int j = idx % n;
        add[i][j] = a[i][j] + b[i][j];
        sub[i][j] = a[i][j] - b[i][j];
        mul[i][j] = a[i][j] * b[i][j];
        div[i][j] = a[i][j] / b[i][j];
    }
}

int main() {
    int n;
    printf("Enter matrix size (one number): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid input\n");
        return 1;
    }

    float** a = (float**)malloc(n * sizeof(float*));
    float** b = (float**)malloc(n * sizeof(float*));
    float** res_add = (float**)malloc(n * sizeof(float*));
    float** res_sub = (float**)malloc(n * sizeof(float*));
    float** res_mul = (float**)malloc(n * sizeof(float*));
    float** res_div = (float**)malloc(n * sizeof(float*));

    for (int i = 0; i < n; i++) {
        a[i] = (float*)malloc(n * sizeof(float));
        b[i] = (float*)malloc(n * sizeof(float));
        res_add[i] = (float*)malloc(n * sizeof(float));
        res_sub[i] = (float*)malloc(n * sizeof(float));
        res_mul[i] = (float*)malloc(n * sizeof(float));
        res_div[i] = (float*)malloc(n * sizeof(float));
    }

    srand((unsigned int)time(NULL));
    init_arrays(a, b, n);

    clock_t start_serial = clock();
    operations_serial(a, b, res_add, res_sub, res_mul, res_div, n);
    clock_t end_serial = clock();
    double time_serial = (double)(end_serial - start_serial) / CLOCKS_PER_SEC;
    printf("Sequential time: %.6f seconds\n", time_serial);

    int threads[] = { 1, 2, 4, 8 };
    for (int i = 0; i < 4; i++) {
        omp_set_num_threads(threads[i]);
        double start_parallel = omp_get_wtime();
        operations_parallel(a, b, res_add, res_sub, res_mul, res_div, n);
        double end_parallel = omp_get_wtime();
        printf("Parallel time with %d thread(s): %.6f seconds\n", threads[i], end_parallel - start_parallel);
    }

    for (int i = 0; i < n; i++) {
        free(a[i]);
        free(b[i]);
        free(res_add[i]);
        free(res_sub[i]);
        free(res_mul[i]);
        free(res_div[i]);
    }
    free(a); free(b); free(res_add); free(res_sub); free(res_mul); free(res_div);

    return 0;
}
