#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define MAX_DATA_LEN 256

typedef struct {
    double key;
    char data[MAX_DATA_LEN];
} Element;

int64_t integer_log2(uint64_t n) {
    if (n == 0) return -1;
    int64_t log = 0;
    while (n >>= 1) log++;
    return log;
}

int64_t integer_pow(int64_t base, int64_t exp) {
    int64_t res = 1;
    for (int64_t i = 0; i < exp; i++) res *= base;
    return res;
}

void print_table(Element arr[], int64_t n, char* title) {
    printf("\n%s\n", title);
    printf("------------------------------------------------\n");
    for (int64_t i = 0; i < n; i++) {
        printf("%lld: key = %g, data = \"%s\"\n", i, arr[i].key, arr[i].data);
    }
    printf("------------------------------------------------\n");
}

void print_keys(Element arr[], int64_t n) {
    for (int64_t i = 0; i < n; i++) {
        printf("%g", arr[i].key);
        if (i < n - 1) printf(" ");
    }
    printf("\n");
}

void shell_sort(Element arr[], int64_t n) {
    if (n <= 1) return;

    int64_t len_steps = integer_log2(n);
    int64_t steps[len_steps];
    int64_t k = integer_pow(2, len_steps);
    for (int64_t i = 0; i < len_steps; i++) {
        steps[i] = k - 1;
        k /= 2;
    }

    for (int64_t s = 0; s < len_steps; s++) {
        int64_t step = steps[s];
        if (step >= n) continue;

        for (int64_t i = step; i < n; i++) {
            Element temp = arr[i];
            int64_t j = i;
            while (j >= step && arr[j - step].key > temp.key) {
                arr[j] = arr[j - step];
                j -= step;
            }
            arr[j] = temp;
        }

        printf("После шага %lld:\n", step);
        print_keys(arr, n);
    }
}

int64_t binary_search(const Element arr[], int64_t n, double key) {
    int64_t left = 0, right = n - 1;
    while (left <= right) {
        int64_t mid = (left + right) / 2;
        if (arr[mid].key == key) return mid;
        else if (arr[mid].key < key) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

Element* read_table_from_stdin(int64_t* n) {
    Element* arr = NULL;
    int64_t capacity = 16;
    int64_t size = 0;
    arr = (Element*)malloc(capacity * sizeof(Element));
    if (!arr) return NULL;

    char buffer[MAX_DATA_LEN];
    while (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) == 0) continue;

        char* space = buffer;
        while (*space && !isspace(*space)) space++;
        if (*space == '\0') {
            printf("Обязателен пробел между ключом и значением\n");
            free(arr);
            return NULL;
        }
        *space = '\0';
        double key = atof(buffer);
        *space = ' ';
        char* data_start = space + 1;
        while (*data_start && isspace(*data_start)) data_start++;

        if (size >= capacity) {
            capacity *= 2;
            Element* new_arr = (Element*)realloc(arr, capacity * sizeof(Element));
            if (!new_arr) {
                free(arr);
                return NULL;
            }
            arr = new_arr;
        }
        arr[size].key = key;
        strncpy(arr[size].data, data_start, MAX_DATA_LEN - 1);
        arr[size].data[MAX_DATA_LEN - 1] = '\0';
        size++;
    }
    *n = size;
    return arr;
}

int main() {
    int64_t n = 0;
    Element* table = read_table_from_stdin(&n);
    if (!table || n == 0) {
        printf("Не удалось прочитать таблицу НОРМАЛЬНЫЕ ДАННЫЕ ВЕСТИ НЕ МОЖЕШЬ?????.\n");
        return 1;
    }

    print_table(table, n, "Исходная таблица:");

    printf("\n--- НАЧАЛО СОРТИРОВКИ ШЕЛЛА ---\n");
    shell_sort(table, n);
    printf("--- СОРТИРОВКА ЗАВЕРШЕНА ---\n");

    print_table(table, n, "Окончательная отсортированная таблица:");

    // Тут происходит магия, мы меняем поток ввода с файла на поток из терминала
    if (freopen("/dev/tty", "r", stdin) == NULL) {
        printf("Не получается переключиться на стандартный поток входа (ну который из терминала пишет пользователь).\n");
    }

    printf("\n=== ДВОИЧНЫЙ ПОИСК ===\n");
    printf("Введите ключи для поиска (вещественные числа). 0 = выход.\n");
    double key;
    while (1) {
        printf("Ключ: ");
        if (scanf("%lf", &key) != 1) break;
        if (key == 0.0) break;
        int64_t idx = binary_search(table, n, key);
        if (idx != -1) {
            printf("НАЙДЕН: ключ = %lf, значение = \"%s\" (индекс %lld)\n",
                   table[idx].key, table[idx].data, (int64_t)idx);
        } else {
            printf("Ключ %lf НЕ НАЙДЕН\n", key);
        }
    }

    free(table);
    return 0;
}