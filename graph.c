#include "graph.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct Edge {
    size_t destination;
    struct Edge *next;
} Edge;

struct Graph {
    size_t vertex_count;
    size_t edge_count;
    Edge **heads;
    Edge **tails;
};

Graph *graph_create(size_t count)
{
    Graph *graph;
    if (count > SIZE_MAX / sizeof(Edge *)) return NULL;
    graph = calloc(1, sizeof(*graph));
    if (!graph) return NULL;
    graph->vertex_count = count;
    if (count) {
        graph->heads = calloc(count, sizeof(*graph->heads));
        graph->tails = calloc(count, sizeof(*graph->tails));
        if (!graph->heads || !graph->tails) {
            free(graph->heads);
            free(graph->tails);
            free(graph);
            return NULL;
        }
    }
    return graph;
}

void graph_destroy(Graph *graph)
{
    size_t u;
    if (!graph) return;
    for (u = 0; u < graph->vertex_count; ++u) {
        Edge *edge = graph->heads[u];
        while (edge) {
            Edge *next = edge->next;
            free(edge);
            edge = next;
        }
    }
    free(graph->heads);
    free(graph->tails);
    free(graph);
}

size_t graph_vertex_count(const Graph *graph)
{
    return graph ? graph->vertex_count : 0;
}

size_t graph_edge_count(const Graph *graph)
{
    return graph ? graph->edge_count : 0;
}

GraphStatus graph_add_edge(Graph *graph, size_t source, size_t destination)
{
    Edge *edge;
    if (!graph || source >= graph->vertex_count ||
        destination >= graph->vertex_count) return GRAPH_INVALID;
    for (edge = graph->heads[source]; edge; edge = edge->next)
        if (edge->destination == destination) return GRAPH_OK;
    if (graph->edge_count == SIZE_MAX) return GRAPH_INVALID;
    edge = malloc(sizeof(*edge));
    if (!edge) return GRAPH_NO_MEMORY;
    edge->destination = destination;
    edge->next = NULL;
    if (graph->tails[source]) graph->tails[source]->next = edge;
    else graph->heads[source] = edge;
    graph->tails[source] = edge;
    ++graph->edge_count;
    return GRAPH_OK;
}

/* WHITE=0: unseen; GRAY=1: active; BLACK=2: finished. */
static GraphStatus visit(const Graph *graph, size_t u, unsigned char *color,
                         size_t *order, size_t *next_slot)
{
    const Edge *edge;
    color[u] = 1;
    for (edge = graph->heads[u]; edge; edge = edge->next) {
        size_t v = edge->destination;
        if (color[v] == 1) return GRAPH_CYCLE;
        if (color[v] == 0) {
            GraphStatus status = visit(graph, v, color, order, next_slot);
            if (status != GRAPH_OK) return status;
        }
    }
    color[u] = 2;
    /* Place finishes from right to left: reverse postorder, no extra reversal. */
    order[--(*next_slot)] = u;
    return GRAPH_OK;
}

GraphStatus graph_topological_sort(const Graph *graph, size_t *order,
                                 size_t capacity)
{
    unsigned char *color;
    size_t u, next_slot;
    GraphStatus status = GRAPH_OK;
    if (!graph || capacity < graph->vertex_count ||
        (graph->vertex_count && !order)) return GRAPH_INVALID;
    if (!graph->vertex_count) return GRAPH_OK;
    color = calloc(graph->vertex_count, sizeof(*color));
    if (!color) return GRAPH_NO_MEMORY;
    next_slot = graph->vertex_count;
    for (u = 0; u < graph->vertex_count && status == GRAPH_OK; ++u)
        if (color[u] == 0) status = visit(graph, u, color, order, &next_slot);
    free(color);
    return status;
}

int graph_is_valid_order(const Graph *graph, const size_t *order, size_t length)
{
    size_t *position;
    size_t i, u;
    int valid = 1;
    if (!graph || length != graph->vertex_count || (length && !order)) return 0;
    if (!length) return 1;
    if (length > SIZE_MAX / sizeof(*position)) return 0;
    position = malloc(length * sizeof(*position));
    if (!position) return 0;
    for (i = 0; i < length; ++i) position[i] = SIZE_MAX;
    for (i = 0; i < length; ++i) {
        if (order[i] >= length || position[order[i]] != SIZE_MAX) {
            free(position);
            return 0;
        }
        position[order[i]] = i;
    }
    for (u = 0; u < length && valid; ++u) {
        const Edge *edge;
        for (edge = graph->heads[u]; edge; edge = edge->next)
            if (position[u] >= position[edge->destination]) {
                valid = 0;
                break;
            }
    }
    free(position);
    return valid;
}
