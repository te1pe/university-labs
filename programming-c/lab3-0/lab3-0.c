#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void printArray(int arr[], int lenArray) {
    for (int i = 0; i < lenArray; i++) {
        printf("%d ", arr[i]);
    }
}

void heapify(int arr[], int lenArray, int i) {
    int root= i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < lenArray && arr[left] > arr[root]) {
        root = left;
    }
    if (right < lenArray && arr[right] > arr[root]) {
        root= right;
    }
    if (root != i) {
        swap(&arr[i], &arr[root]);
        heapify(arr, lenArray, root);
    }
}

void HeapSort(int arr[], int lenArray) {
    for (int i = lenArray / 2 - 1; i >= 0; i--) {
        heapify(arr, lenArray, i);
    }
    for (int i = lenArray - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0);
    }
}

int main() {
    int lenArray;
    if (!scanf("%d", &lenArray)) {
        return 0;
    }
    int *arr = (int*)malloc(lenArray * sizeof(int));
    if (arr==NULL) {
        free(arr);
        return 0; 
    }
    for (int i = 0; i < lenArray; i++) {
        if (!scanf("%d", &arr[i])) {
            free(arr);
            return 0;
        }
    }
    HeapSort(arr, lenArray);
    printArray(arr, lenArray);
    free(arr);
    return 0;
}

