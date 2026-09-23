#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int weight;
    int points;
} Itembp;

int main() {
    int N, W;
    if (scanf("%d %d", &N, &W)!=2) {return 0;}
    Itembp *items = (Itembp*)malloc(N * sizeof(Itembp));
    if (items==NULL) {return 0;}
    for (int i = 0; i < N; i++) {
        if (scanf("%d %d", &items[i].weight, &items[i].points)!=2) {free(items); return 0;}
    }
    int *Max_points = (int*)malloc((W + 1) * sizeof(int));
    if (Max_points==NULL) {free(items); return 0;}
    int **selected = (int**)malloc((W + 1) * sizeof(int*));
    if (selected==NULL) {free(items); free(Max_points); return 0;}
    for (int i = 0; i <= W; i++) {
        Max_points[i] = 0;
        selected[i] = (int*)calloc(N, sizeof(int));
        if (selected[i]==NULL) {
            for (int g=0; g<i; g++){free(selected[g]);}
            free(items); 
            free(Max_points);  
            free(selected);
            return 0;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int w = W; w >= items[i].weight; w--) {
            if (Max_points[w - items[i].weight] + items[i].points > Max_points[w]) {
                Max_points[w] = Max_points[w - items[i].weight] + items[i].points;
                for (int j = 0; j < N; j++) {
                    selected[w][j] = selected[w - items[i].weight][j];
                }
                selected[w][i] = 1;
            }
        }
    }
    printf("%d\n", Max_points[W]);
    for (int i = 0; i < N; i++) {
        if (selected[W][i]) {
            printf("%d %d\n", items[i].weight, items[i].points);
        }
    }
    for (int i = 0; i <= W; i++) {free(selected[i]);}
    free(selected);
    free(Max_points);
    free(items);
    return 0;
}

