#include <stdio.h>
#include <stdlib.h>

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[(low + high) / 2];
    int i = low;
    int j = high;
    while (1) {
        while (arr[i] < pivot) {
            i++;
        }
        while (arr[j] > pivot) {
            j--;
        }
        if (i >= j){
        return j;
        }
        swap(&arr[i], &arr[j]);
        i++;
        j--;
    }
}

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int index = partition(arr, low, high);
        quicksort(arr, low, index);
        quicksort(arr, index + 1, high);
    }
}

void printArray(int arr[], int lenArray) {
    for (int i = 0; i < lenArray; i++) {
        printf("%d ", arr[i]);
    }
}

int main() {
    int lenArray;
    if (!scanf("%d", &lenArray)) {
        return 1;
    }
    int* arr = (int*)malloc(lenArray * sizeof(int));
    if (arr == NULL) {
        return 1;
    }
    for (int i = 0; i < lenArray; i++) {
        if (!scanf("%d", &arr[i])) {
            free(arr);
            return 1;
        }
    }
    quicksort(arr, 0, lenArray - 1);
    printArray(arr, lenArray);
    free(arr);
    return 0;
}

