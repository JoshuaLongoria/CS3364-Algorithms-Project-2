#ifndef GRAPH_H
#define GRAPH_H
#include <stddef.h>

/* Opaque directed adjacency-list graph. Vertices are 0 .. count-1. */
typedef struct Graph Graph;
typedef enum {
    GRAPH_OK = 0,
    GRAPH_CYCLE,
    GRAPH_NO_MEMORY,
    GRAPH_INVALID
} GraphStatus;

Graph *graph_create(size_t count);
void graph_destroy(Graph *graph);
size_t graph_vertex_count(const Graph *graph);
size_t graph_edge_count(const Graph *graph);
/* Duplicate edges succeed without adding a second copy. */
GraphStatus graph_add_edge(Graph *graph, size_t source, size_t destination);
/* order must have capacity >= vertex count; cycle produces no valid result. */
GraphStatus graph_topological_sort(const Graph *graph, size_t *order,
                                 size_t capacity);
int graph_is_valid_order(const Graph *graph, const size_t *order, size_t length);
#endif
