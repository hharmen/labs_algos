
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_VERTICES 100
#define INF          INT_MAX

typedef struct {
    int  n;
    int  adj[MAX_VERTICES][MAX_VERTICES];
} Graph;


void graph_init(Graph *g, int n) {
    g->n = n;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            g->adj[i][j] = 0;
}

void graph_add_edge(Graph *g, int u, int v, int w) {
    if (u < 0 || u >= g->n || v < 0 || v >= g->n || w <= 0) {
        fprintf(stderr, "Ошибка: недопустимое ребро (%d->%d, вес=%d)\n", u, v, w);
        return;
    }
    g->adj[u][v] = w;
}

void graph_print(const Graph *g) {
    printf("\nМатрица смежности (%d вершин):\n", g->n);
    printf("     ");
    for (int j = 0; j < g->n; j++) printf("%4d", j);
    printf("\n     ");
    for (int j = 0; j < g->n; j++) printf("----");
    printf("\n");
    for (int i = 0; i < g->n; i++) {
        printf("%3d |", i);
        for (int j = 0; j < g->n; j++) {
            if (g->adj[i][j] == 0)
                printf("   .");
            else
                printf("%4d", g->adj[i][j]);
        }
        printf("\n");
    }
}

//алгоритм Дейкстр
static int min_distance(const int dist[], const int visited[], int n) {
    int min_val = INF, min_idx = -1;
    for (int v = 0; v < n; v++) {
        if (!visited[v] && dist[v] < min_val) {
            min_val = dist[v];
            min_idx = v;
        }
    }
    return min_idx;
}

void dijkstra(const Graph *g, int src, int dist[], int prev[]) {
    int visited[MAX_VERTICES] = {0};

    for (int i = 0; i < g->n; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[src] = 0;

    for (int iter = 0; iter < g->n - 1; iter++) {
        int u = min_distance(dist, visited, g->n);
        if (u == -1) break;
        visited[u] = 1;

        for (int v = 0; v < g->n; v++) {
            int w = g->adj[u][v];
            if (w > 0 && !visited[v] && dist[u] != INF) {
                long long new_dist = (long long)dist[u] + w;
                if (new_dist < dist[v]) {
                    dist[v] = (int)new_dist;
                    prev[v] = u;
                }
            }
        }
    }
}

static void print_path_recursive(const int prev[], int v) {
    if (prev[v] == -1) {
        printf("%d", v);
        return;
    }
    print_path_recursive(prev, prev[v]);
    printf(" → %d", v);
}

void print_results(const Graph *g, int src,
                   const int dist[], const int prev[]) {
    printf("\nАлгоритм Дейкстры. Источник: вершина %d\n", src);
    printf("%-10s %-15s %s\n", "Вершина", "Расстояние", "Путь");
    printf("%-10s %-15s %s\n", "-------", "----------", "----");
    for (int v = 0; v < g->n; v++) {
        printf("%-10d ", v);
        if (dist[v] == INF) {
            printf("%-15s %s\n", "∞", "недостижима");
        } else {
            printf("%-15d ", dist[v]);
            print_path_recursive(prev, v);
            printf("\n");
        }
    }
}


Graph *graph_read(void) {
    int n, m;
    printf("Введите число вершин и рёбер: ");
    if (scanf("%d %d", &n, &m) != 2 || n < 1 || n > MAX_VERTICES) {
        fprintf(stderr, "Некорректный ввод. (ВВОДИ НОРМАЛЬНО, БАГОВ ТУТ НЕ ДОЛЖНО БЫТЬ)\n");
        exit(EXIT_FAILURE);
    }
    Graph *g = (Graph *)malloc(sizeof(Graph));
    if (!g) { perror("malloc"); exit(EXIT_FAILURE); }
    graph_init(g, n);

    printf("Введите рёбра в формате «u v w» (вершины 0..%d, вес/длина > 0):\n", n - 1);
    for (int i = 0; i < m; i++) {
        int u, v, w;
        if (scanf("%d %d %d", &u, &v, &w) != 3) {
            fprintf(stderr, "Ошибка чтения ребра %d.\n", i + 1);
            exit(EXIT_FAILURE);
        }
        graph_add_edge(g, u, v, w);
    }
    return g;
}

int main(void) {
    printf("----------------!!!!НАШ ГРАФ ОРИЕНТИРОВАННЫЙ!!!!----------------\n\n");


   

    printf("\n\n--- Ввод (учитывай, что при вводе одинаковых вершин принимается только путь с весом, который ввел в конце) ---\n");
    Graph *g = graph_read();
    graph_print(g);
    int dist[MAX_VERTICES], prev[MAX_VERTICES];
    int src;
    printf("\nВведите начальную вершину для Дейкстры (0..%d): ", g->n - 1);
    if (scanf("%d", &src) != 1 || src < 0 || src >= g->n) {
        fprintf(stderr, "Некорректная вершина. (ты слепой чтоли?)\n");
        free(g);
        return EXIT_FAILURE;
    }

    dijkstra(g, src, dist, prev);
    print_results(g, src, dist, prev);

    free(g);
    return EXIT_SUCCESS;
}
