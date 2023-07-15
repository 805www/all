#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================
// bf 算法
// =========================================

int bf(const char *master, const char *pattern) {
    int n_master = strlen(master);
    int n_pattern = strlen(pattern);

    for (int i = 0; i < n_master - n_pattern + 1; i++) {
        int j = 0;
        for (; j < n_pattern && master[i] == pattern[j]; i++, j++)
            ;
        if (j == n_pattern) return i - j;
    }
    return -1;
}

// =========================================
// kmp 算法
// =========================================

void get_next(const char *pattern, int *next, int n) {
    next[0] = -1;
    int j = 0, k = -1;

    while (j < n - 1) {
        if (k == -1) {
            j++;
            next[j] = 0;
            k = 0;
        } else if (pattern[j] == pattern[k]) {
            j++, k++;
            next[j] = k;
        } else {
            k = next[k];
        }
    }
}

int kmp(const char *master, const char *pattern) {
    int n_master = strlen(master);
    int n_pattern = strlen(pattern);

    int *next = (int *)malloc(sizeof(int) * n_pattern);
    get_next(pattern, next, n_pattern);

    int j = 0, k = 0;
    while (j < n_master && k < n_pattern) {
        if (k == -1) {
            k = 0;
            j++;
        } else if (master[j] == pattern[k])
            j++, k++;
        else
            k = next[k];
    }

    free(next);
    if (k == n_pattern) return j - k;
    return -1;
}

int main() {
    int index = kmp("abcde", "ad");
    printf("%d ", index);
}
