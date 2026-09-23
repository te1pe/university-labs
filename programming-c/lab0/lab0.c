#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#define Max_ss 16
#define Min__ss 2
#define Max_mesto 58
#define vvod_chislo 14
#define znaki_drob 12

int transfet_simvolov(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
}

char transfet_simvolov_obr(int z) {
    if (z >= 0 && z <= 9)
        return '0' + z;
    if (z >= 10 && z <= 15)
        return 'A' + z - 10;
    return 0;
}

int nedrob(const char *num, int b1, int b2, char *result) {
    int len = strlen(num);
    unsigned long long promej = 0;
    char *t = (char *)malloc(Max_mesto * sizeof(char));
    if (t==NULL) {
    return 0;
    }
    
    int index = 0;

    for (int i = 0; i < len; i++) {
        int value = transfet_simvolov(num[i]);
        if (value == -1 || value >= b1) {
            printf("bad input\n");
            free(t);
            return 1;
        }
        promej += value * pow(b1, len - i - 1);
    }

    if (promej == 0) {
        strcpy(result, "0");
        free(t);
        return 0;
    }
    while (promej > 0) {
        t[index++] = transfet_simvolov_obr(promej % b2);
        promej/= b2;
    }

    for (int i = 0; i < index; i++) {
        result[i] = t[index - i - 1];
    }
    result[index] = '\0';
    free(t);
    return 0;
}

int drob(const char *int_part, const char *frac_part, int b1, int b2, char *result) {
    char *neeedddrob = (char *)malloc(Max_mesto * sizeof(char));
    if (neeedddrob==NULL) {
    return 0;
    }
    if (nedrob(int_part, b1, b2, neeedddrob)) {
        free(neeedddrob);
        return 1;
    }
    int len1 = strlen(frac_part);
    char d[Max_mesto] = ".";
    int ind = 1;
    long double promej2 = 0.0;
    for (int i = 0; i < len1; i++) {
        int value = transfet_simvolov(frac_part[i]);
        if (value == -1 || value >= b1) {
            printf("bad input\n");
            free(neeedddrob);
            return 1;
        }
        promej2 += value * pow(b1, -i-1);
    }
    for (int j = 0; j < znaki_drob; j++) {
        promej2 *= b2;
        int ch = (int) promej2;
        d[ind++] = transfet_simvolov_obr(ch);
        promej2 -= ch;
    }
    d[ind] = '\0';
    strcat(neeedddrob, d);
    strcpy(result, neeedddrob);
    free(neeedddrob);
    return 0;
}

int main() {
    int b1, b2;
    char num[vvod_chislo];
    char *result = (char *)malloc(Max_mesto * sizeof(char));
    if (result==NULL) {
    return 0;
    }
    int k = scanf("%d %d %13s", &b1, &b2, num);
    if (k != 3) {
        printf("bad input\n");
        free(result);
        return 0;
    }
    if (b1 < Min__ss || b1 > Max_ss|| b2 < Min__ss || b2 > Max_ss) {
        printf("bad input\n");
        free(result);
        return 0;
    }
    if (strchr(num, '.') != strrchr(num, '.') || num[0] == '.' || num[strlen(num)-1] == '.' || strstr(num, "..")) {
        printf("bad input\n");
        free(result);
        return 0;
    }

    char *int_part = strtok(num, ".");
    char *frac_part = strtok(NULL, ".");
    int osh = 0;
    if (frac_part == NULL) {
        osh = nedrob(int_part, b1, b2, result);
    } else {
        osh = drob(int_part, frac_part, b1, b2, result);
    }
    if (!osh) {
        printf("%s\n", result);
    }
    free(result);
    return 0;
}

