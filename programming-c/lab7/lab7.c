#include <stdio.h>
#include <stdlib.h>
#define MAXN 2000

typedef enum {
    NOT_VISITED,
    IN_PROGRESS,
    VISITED
} VisitStatus;

typedef struct {
    short *edges;
    int count;
    int capacity;
} List;

List *Create_Graph(int N) {
    List *graph = malloc(N * sizeof(List));
    if (!graph) {return NULL;}
    for (int i = 0; i < N; i++) {
        graph[i].edges = NULL;
        graph[i].count = 0;
        graph[i].capacity = 0;
    }
    return graph;
}

void Free_Graph(List *graph, int N) {
    if (!graph) return;
    for (int i = 0; i < N; i++) {
        free(graph[i].edges);
    }
    free(graph);
}

int Add_Edge(List *graph, int u, int v) {
    if (graph[u].count == graph[u].capacity) {
        int new_capacity = graph[u].capacity == 0 ? 2 : graph[u].capacity * 2;
        short *new_edges = realloc(graph[u].edges, new_capacity * sizeof(short));
        if (!new_edges) {
            return 0;
        }
        graph[u].edges = new_edges;
        graph[u].capacity = new_capacity;
    }
    graph[u].edges[graph[u].count++] = (short)v;
    return 1;
}

int dfs(int v, List *graph, VisitStatus *visited, int *result, int *index) {
    visited[v] = IN_PROGRESS;
    for (int i = 0; i < graph[v].count; i++) {
        int next = graph[v].edges[i];
        if (visited[next] == IN_PROGRESS) return 1;
        if (visited[next] == NOT_VISITED) {
            if (dfs(next, graph, visited, result, index)) return 1;
        }
    }
    visited[v] = VISITED;
    result[(*index)--] = v;
    return 0;
}

int topological_sort(List *graph, int N, int *result) {
    VisitStatus *visited = calloc(N, sizeof(VisitStatus));
    if (!visited) {
        return 0;
    }
    int index = N - 1;
    for (int i = 0; i < N; i++) {
        if (visited[i] == NOT_VISITED) {
            if (dfs(i, graph, visited, result, &index)) {
                free(visited);
                return 0;
            }
        }
    }
    free(visited);
    return 1;
}

int main() {
    int N, M;
    if (scanf("%d", &N) != 1 || N < 0 || N > MAXN) {
        printf("bad number of vertices\n");
        return 0;
    }
    if (scanf("%d", &M) != 1) {
        printf("bad number of lines\n");
        return 0;
    }
    if (M < 0 || M > N * (N - 1) / 2) {
        printf("bad number of edges\n");
        return 0;
    }

    List *graph = Create_Graph(N);
    if (!graph) {return 0;}

    int u, v;
    for (int i = 0; i < M; i++) {
        if (scanf("%d %d", &u, &v) != 2) {
            printf("bad number of lines\n");
            Free_Graph(graph, N);
            return 0;
        }
        if (u < 1 || u > N || v < 1 || v > N) {
            printf("bad vertex\n");
            Free_Graph(graph, N);
            return 0;
        }
        if (!Add_Edge(graph, u - 1, v - 1)) {
            Free_Graph(graph, N);
            return 0;
        }
    }

    int *result = malloc(N * sizeof(int));
    if (!result) {
        Free_Graph(graph, N);
        return 0;
    }

    if (!topological_sort(graph, N, result)) {
        printf("impossible to sort\n");
    } else {
        for (int i = 0; i < N; i++) {
            printf("%d ", result[i] + 1);
        }
    }

    free(result);
    Free_Graph(graph, N);
    return 0;
}

