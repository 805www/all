#include <stddef.h>
#include <stdio.h>

// =======================================
// 冒泡排序
// =======================================
void bubble_sort(int *array, int n) {
  for (int i = 0; i < n - 1; i++)
    for (int j = 0; j < n - 1 - i; j++)
      if (array[j] > array[j + 1]) {
        int tmp = array[j];
        array[j] = array[j + 1];
        array[j + 1] = tmp;
      }
}

void test_bubble_sort() {
  int array[] = {1, 3, 2, 5, 1};
  int n = sizeof(array) / sizeof(int);
  bubble_sort(array, n);

  printf("bubble sort: \n");
  for (int i = 0; i < n; i++) printf("%d, ", array[i]);
  printf("\n\n");
}

// =======================================
// 选择排序
// =======================================
void select_sort(int *array, int n) {
  for (int i = 0; i < n - 1; i++) {
    int maxj = 0;
    for (int j = 1; j < n - i; j++)
      if (array[j] > array[maxj]) maxj = j;

    int tmp = array[n - i - 1];
    array[n - i - 1] = array[maxj];
    array[maxj] = tmp;
  }
}

void test_select_sort() {
  int array[] = {1, 3, 2, 5, 1};
  int n = sizeof(array) / sizeof(int);
  select_sort(array, n);

  printf("select sort: \n");
  for (int i = 0; i < n; i++) printf("%d, ", array[i]);
  printf("\n\n");
}

// ==============================================
// 快速排序
// ==============================================
void fast_sort_recursive(int *array, int begin, int end) {
  if (begin >= end) return;

  int left = begin, right = end - 1;
  int mid = array[end];

  while (left < right) {
    while (left < right && array[left] < mid) left++;
    while (left < right && array[right] >= mid) right--;

    int tmp = array[left];
    array[left] = array[right];
    array[right] = tmp;
  }

  if (array[left] >= mid) {
    int tmp = array[left];
    array[left] = array[end];
    array[end] = tmp;
  } else {
    left++;
  }

  if (left) fast_sort_recursive(array, 0, left - 1);
  fast_sort_recursive(array, left + 1, end);
}

void fast_sort(int *array, int n) { fast_sort_recursive(array, 0, n - 1); }

void test_fast_sort() {
  // int array[] = {1, 3, 2, 5, 1};
  int array[] = {1, 3, 2, 2, 3, 1, 2, 3, 1, 23, 12, 1, 3, 55, 1, 5, 1};
  int n = sizeof(array) / sizeof(int);
  select_sort(array, n);

  printf("fast sort: \n");
  for (int i = 0; i < n; i++) printf("%d, ", array[i]);
  printf("\n\n");
}

// =======================================
// main
// =======================================
int main() {
  test_bubble_sort();
  test_select_sort();
  test_fast_sort();
}
