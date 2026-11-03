#include "courses.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    Graph *graph = NULL;
    size_t *order;
    size_t i;
    GraphStatus status = build_course_graph(&graph);
    if (status != GRAPH_OK) {
        fprintf(stderr, "Cannot build course graph (status %d).\n", (int)status);
        return EXIT_FAILURE;
    }
    order = malloc(course_count * sizeof(*order));
    if (!order) {
        fprintf(stderr, "Cannot allocate course order.\n");
        graph_destroy(graph);
        return EXIT_FAILURE;
    }
    status = graph_topological_sort(graph, order, course_count);
    if (status != GRAPH_OK) {
        fprintf(stderr, "%s\n", status == GRAPH_CYCLE ?
                "Cycle detected: no valid course order exists." :
                "Cannot compute course order.");
        free(order);
        graph_destroy(graph);
        return EXIT_FAILURE;
    }
    if (!graph_is_valid_order(graph, order, course_count)) {
        fprintf(stderr, "Course order validation failed.\n");
        free(order);
        graph_destroy(graph);
        return EXIT_FAILURE;
    }
    printf("Courses: %zu; prerequisite edges: %zu\n", graph_vertex_count(graph),
           graph_edge_count(graph));
    puts("DFS topological order (one valid solution):");
    for (i = 0; i < course_count; ++i)
        printf("%2zu. %s - %s\n", i + 1, courses[order[i]].code,
               courses[order[i]].title);
    puts("Validation: PASS (every prerequisite precedes its course)");
    free(order);
    graph_destroy(graph);
    return EXIT_SUCCESS;
}
