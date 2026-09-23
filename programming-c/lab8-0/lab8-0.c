#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_VERTICES 5000
#define MAX_EDGES (MAX_VERTICES * (MAX_VERTICES - 1) / 2)

typedef struct Edge {
    int start; 
    int end; 
    int length;
} Edge;

int CompareEdgesLength(const void *a, const void *b) {
    const Edge *edge_a = (const Edge*)a;
    const Edge *edge_b = (const Edge*)b;
    return (edge_a->length > edge_b->length) - (edge_a->length < edge_b->length);
}

int FindParent(int vertex, int *parent) {
    if (parent[vertex] != vertex) {
        parent[vertex] = FindParent(parent[vertex], parent);
    }
    return parent[vertex];
}

void UnionSets(int a, int b, int *parent, int *rank) {
    int root_a = FindParent(a, parent);
    int root_b = FindParent(b, parent);
    if (root_a != root_b) {
        if (rank[root_a] < rank[root_b]) {
            parent[root_a] = root_b;
        } else if (rank[root_a] > rank[root_b]) {
            parent[root_b] = root_a;
        } else {
            parent[root_b] = root_a;
            rank[root_a]++;
        }
    }
}

void KraskalAlgorithm(Edge *edges, int N, int M) {
    if (N <= 0 || N > MAX_VERTICES) {
        printf("no spanning tree");
        return;
    }
    qsort(edges, M, sizeof(Edge), CompareEdgesLength);
    int *parent = (int*)malloc((N + 1) * sizeof(int));
    int *rank = (int*)malloc((N + 1) * sizeof(int));
    Edge *result = NULL;
    if (!parent || !rank) {
        free(parent);
        free(rank);
        printf("no spanning tree");
        return;
    }
    result = (Edge*)malloc((N - 1) * sizeof(Edge));
    if (!result) {
        free(parent);
        free(rank);
        printf("no spanning tree");
        return;
    }
    for (int i = 1; i <= N; i++) {
        parent[i] = i;
        rank[i] = 0;
    }
    int edge_count = 0;
    for (int i = 0; i < M && edge_count < N - 1; i++) {
        Edge current = edges[i];
        int root_start = FindParent(current.start, parent);
        int root_end = FindParent(current.end, parent);
        if (root_start != root_end) {
            result[edge_count] = current;
            edge_count++;
            UnionSets(current.start, current.end, parent, rank);
        }
    }
    if (edge_count != N - 1) {
        printf("no spanning tree");
    } else {
        for (int i = 0; i < edge_count; i++) {
            printf("%d %d\n", result[i].start, result[i].end);
        }
    }
    free(parent);
    free(rank);
    free(result);
}

int main() {
    int N, M;
    if (scanf("%d", &N) != 1) {return 0;}
    if (N < 0 || N > MAX_VERTICES) {printf("bad number of vertices");return 0;}
    if (scanf("%d", &M) != 1) {return 0;}
    if (M < 0 || M > (N * (N - 1)) / 2) {printf("bad number of edges");return 0;}
    Edge *edges = (Edge*)malloc(M * sizeof(Edge));
    if (!edges) {return 0;}
    for (int i = 0; i < M; i++) {
        int start, end, length;
        if (scanf("%d %d %d", &start, &end, &length) != 3) {
            printf("bad number of lines");
            free(edges);
            return 0;
        }
        if (start < 1 || start > N || end < 1 || end > N) {
            printf("bad vertex");
            free(edges);
            return 0;
        }
        if (length < 0 || length > INT_MAX) {
            printf("bad length");
            free(edges);
            return 0;
        }
        edges[i].start = start;
        edges[i].end = end;
        edges[i].length = length;
    }
    if (N == 0) {
        printf("no spanning tree");
        free(edges);
        return 0;
    }
    KraskalAlgorithm(edges, N, M);
    free(edges);
    return 0;
}

