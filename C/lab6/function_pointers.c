#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void bubble_sort(int *, int);
void selection_sort(int *, int);
void insertion_sort(int *, int);


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

// Initialize the values in arr to be in decreasing order.
// This is the reverse of what our sorting function will provide.
void max_to_min_init(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = size - i;
    }
}

// Now our time_sort takes both the sorting function and the initializer
// function as parameters. This function does not need to be recompiled
// to change these values.

// to make the function a parameter to another function :
// eg. --> void (*sort_func)(int *,int)
//         return type (*function_name)(parameters function take)
double time_sort(int size, void (*sort_func)(int *, int), void (*initializer)(int *, int)) {
    int arr[size];
    // We are calling the function received as a parameter.
    // note that you don't need to dereference it 
    initializer(arr, size);

    clock_t begin = clock();
    // We are calling the function received as a parameter.
    // note that you don't need to dereference it 
    sort_func(arr, size);
    clock_t end = clock();

    check_sort(arr, size);

    return (double)(end - begin) / CLOCKS_PER_SEC;
}


int main() {
    srand(time(NULL));

    for (int size = 1; size <= 4096; size *= 2) {
        // insertion_sort and random_init are functions. In the line below
        // they are being passed as parameters to time_sort
        double time_spent = time_sort(size, insertion_sort, random_init);
        printf("%d: %f\n", size, time_spent);
    }

    return 0;
}

// ================== HELPER FUNCTIONS ===========================
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
