#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 13

bool check(int *arr, int row) {
  int val = arr[row];
  int j = 1;
  for (int i = row - 1; i >= 0; i--) {
    if (arr[i] == val) return false;
    if (arr[i] + j == val) return false;
    if (arr[i] - j == val) return false;
    j++;
  }
  return true;
}

int n_queue() {
  int total = 0;
  int arr[N] = {0};
  int row = 0;
  int done = 0;

  while (!done) {
    if (check(arr, row)) {
      if (row == N - 1) {
        total++;
      } else {
        row++;
        arr[row] = 0;
        continue;
      }
    }

    arr[row]++;

    while (arr[row] >= N) {
      --row;
      if (row < 0) {
        done = 1;
        break;
      }
      arr[row]++;
    }
  }

  return total;
}

int main() {
  int total = n_queue();
  printf("%d\n", total);
}
