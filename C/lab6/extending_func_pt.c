#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// to avoid an ugly header 
typedef void (*SortFunc_t)(int *, int);

typedef struct {
    char *name;
    SortFunc_t sort_func;
} sort_info;

void bubble_sort(int *, int);
void selection_sort(int *, int);
void insertion_sort(int *, int);
const int NUM_SORTS = 3;
sort_info SORTS[] = {
    {.name = "bubble", .sort_func = bubble_sort},
    {.name = "selection", .sort_func = selection_sort},
    {.name = "insertion", .sort_func = insertion_sort}
};

// if you don't define a struct SortFunc_T
// then the header should be
// void (* parse_command_line(int argc, char * argv[]))(int *,int)

// the function parse_command_line takes an integer and an array of strings
// then it returns a function pointer 
// the function pointer points to a function which takes an int array and an int
// and return void.
SortFunc_t parse_command_line(int argc, char *argv[]) {
    if (argc > 1) {
        for (int sort = 0; sort < NUM_SORTS; sort++) {
            if (strcmp(argv[1], SORTS[sort].name) == 0) {
                return SORTS[sort].sort_func;
            }
        }
    }
    fprintf(stderr, "Unrecognized sort name. Valid names are:\n");
    for (int i = 0; i < NUM_SORTS; i++) {
        fprintf(stderr, "  %s\n", SORTS[i].name);
    }
    exit(1);
}


void check_sort(int *arr, int size) {
    for (int i = 1; i < size; i++) {
        if (arr[i - 1] > arr[i]) {
            printf("Mis-sorted at index %d\n", i);
            return;
        }
    }
}


void random_init(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand();
    }
}


void max_to_min_init(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = size - i;
    }
}


double time_sort(int size, SortFunc_t sort_func, SortFunc_t initializer) {
    int arr[size];
    initializer(arr, size);

    clock_t begin = clock();
    sort_func(arr, size);
    clock_t end = clock();

    check_sort(arr, size);

    return (double)(end - begin) / CLOCKS_PER_SEC;
}


int main(int argc, char *argv[]) {
    srand(time(NULL));

    SortFunc_t sort_func = parse_command_line(argc, argv);

    for (int size = 1; size <= 4096; size *= 2) {
        double time_spent = time_sort(size, sort_func, random_init);
        printf("%d: %f\n", size, time_spent);
    }

    return 0;
}




// ==================== HELPER FUNCTIONS ========================
void swap(int *arr, int index1, int index2) {
    int temp = arr[index1];
    arr[index1] = arr[index2];
    arr[index2] = temp;
}

/* Notice that all of these sorts have the same signature. They 
 * all take an integer pointer and an integer and have void return type.
 */
void bubble_sort(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 1; j < size - i; j++) {
            if (arr[j - 1] > arr[j]) {
                swap(arr, j - 1, j);
            }
        }
    }
}


void selection_sort(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        int current = i;
        for (int j = i; j < size; j++) {
            if (arr[current] > arr[j]) {
                current = j;
            }
        }
        swap(arr, i, current);
    }
}

void insertion_sort(int *arr, int size) {
    for (int i = 1; i < size; i++) {
        for (int j = i; j > 0 && arr[j - 1] > arr[j]; j--) {
            swap(arr, j - 1, j);
        }
    }
}
