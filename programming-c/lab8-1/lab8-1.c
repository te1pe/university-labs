#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_VERTICES 5000
#define INF UINT_MAX

typedef struct Edge { 
    short u, v; 
    unsigned len;
}Edge; 

unsigned* Create_Graph(int N, Edge* edges, int M) {
    size_t size = N * (N + 1) / 2;
    unsigned* graph = malloc(size * sizeof(unsigned));
    if (!graph) return NULL;
    for (size_t i = 0; i < size; ++i) graph[i] = INF;
    for (int i = 0; i < M; ++i) {
        int u = edges[i].u - 1, v = edges[i].v - 1; 
        if (u == v) continue;
        if (u > v) { int t = u; u = v; v = t; } 
        size_t idx = (u * (2 * N - u - 1)) / 2 + (v - u - 1);
        if (edges[i].len < graph[idx]) graph[idx] = edges[i].len;
    }
    return graph;
}

int get_weight(unsigned* graph, int N, int u, int v) {
    if (u == v) return 0;
    if (u > v) { int t = u; u = v; v = t; }
    size_t idx = (u * (2 * N - u - 1)) / 2 + (v - u - 1);
    return graph[idx];
}

void Prim_Algorithm(int N, unsigned* graph) {
    unsigned* key = malloc(N * sizeof(unsigned));
    int* parent = malloc(N * sizeof(int));
    char* visited = calloc(N, sizeof(char));
    if (!key || !parent || !visited) {
        free(key); free(parent); free(visited); 
        return;
    }
    for (int i = 0; i < N; ++i) key[i] = INF;
    key[0] = 0;
    parent[0] = -1;
    for (int i = 0; i < N; ++i) {
        int u = -1;
        for (int j = 0; j < N; ++j)
            if (!visited[j] && (u == -1 || key[j] < key[u])) u = j;
        if (key[u] == INF) {
            printf("no spanning tree");
            goto cleanup;
        }
        visited[u] = 1;
        for (int v = 0; v < N; ++v) {
            unsigned w = get_weight(graph, N, u, v);
            if (!visited[v] && w < key[v]) {
                parent[v] = u;
                key[v] = w;
            }
        }
    }
    for (int i = 1; i < N; ++i) {
        if (parent[i] == -1) {
            printf("no spanning tree");
            goto cleanup;
        }
        printf("%d %d\n", parent[i] + 1, i + 1);
    }
cleanup:
    free(key); free(parent); free(visited);
}

int main() {
    int N, M;
    if (scanf("%d", &N) != 1) return !printf("bad number of lines");
    if (N < 0 || N > MAX_VERTICES) return !printf("bad number of vertices");
    if (N <= 1) return N == 0 ? !printf("no spanning tree") : 0;
    if (scanf("%d", &M) != 1) return !printf("bad number of lines");
    if (M < 0 || M > N*(N-1)/2) return !printf("bad number of edges");
    Edge* edges = malloc(M * sizeof(Edge));
    if (!edges) return 0;
    for (int i = 0; i < M; ++i) {
        int u, v, len;
        if (scanf("%d %d %d", &u, &v, &len) != 3) {
            free(edges);
            return !printf("bad number of lines");
        }
        if (u < 1 || u > N || v < 1 || v > N) {
            free(edges);
            return !printf("bad vertex");
        }
        if (len < 0) {
            free(edges);
            return !printf("bad length");
        }
        edges[i] = (Edge){(short)u, (short)v, (unsigned)len};
    }
    unsigned* graph = Create_Graph(N, edges, M);
    if (!graph) {
        free(edges);
        return 0;
    }
    Prim_Algorithm(N, graph);
    free(graph);
    free(edges);
    return 0;
}

