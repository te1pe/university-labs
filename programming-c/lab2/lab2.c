#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define Max_len 10
#define Max_proverka_posl 12
int proverka(const char *str) {
    int len = strlen(str);
    if (len > Max_len || len == 0) {
        return 0;
    }
    int counts[Max_len] = {0};

    for (int i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') return 0;
        if (counts[str[i] - '0']++) return 0;
    }
    return 1;
}

void swap(char *a, char *b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

void reverse(char *str, int start, int end) {
    while (start < end) {
        swap(&str[start], &str[end]);
        start++;
        end--;
    }
}

int combi(char *str, int len) {
    int i = len - 2;
    while (i >= 0 && str[i] >= str[i + 1]) {
        i--;
    }
    if (i == -1) return 0;
    int j = len - 1;
    while (str[j] <= str[i]) {
        j--;
    }
    swap(&str[i], &str[j]);
    reverse(str, i + 1, len - 1);
    return 1;
}

int main() {
    char posl [Max_proverka_posl];
    int n;
        if (!scanf("%11[^\n]",posl)) {
        printf("bad input\n");
        return 0;
    }
    if (!scanf("%d", &n)) {
        printf("bad input\n");
        return 0;
    }
    if (!proverka(posl)) {
        printf("bad input\n");
        return 0;
    }
    if (n == 0) {
        return 0;
    }
    int dlina=strlen(posl);
    for (int i = 0; i < n; i++) {
        if (!combi(posl, dlina)) break;
        printf("%s\n", posl);
    }
    return 0;
}

