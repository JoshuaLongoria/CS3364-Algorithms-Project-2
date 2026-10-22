#include "courses.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

static void test_assignment(void)
{
    Graph *g = NULL;
    size_t order[29], positions[29], i, j, k;
    CHECK(build_course_graph(&g) == GRAPH_OK);
    CHECK(course_count == 29);
    CHECK(graph_vertex_count(g) == 29 && graph_edge_count(g) == 30);
    CHECK(graph_topological_sort(g, order, 29) == GRAPH_OK);
    CHECK(graph_is_valid_order(g, order, 29));
    for (i = 0; i < 29; ++i) positions[order[i]] = i;
    /* Check raw course prerequisites as well as the graph's edge validator. */
    for (i = 0; i < course_count; ++i)
        for (j = 0; j < courses[i].prerequisite_count; ++j) {
            for (k = 0; k < course_count; ++k)
                if (!strcmp(courses[k].code, courses[i].prerequisites[j])) break;
            CHECK(k < course_count);
            CHECK(positions[k] < positions[i]);
        }
    graph_destroy(g);
    puts("PASS: all 29 courses and 30 raw prerequisite relationships");
}

static void test_empty_and_disconnected(void)
{
    Graph *g = graph_create(0);
    size_t order[5];
    CHECK(g);
    CHECK(graph_topological_sort(g, NULL, 0) == GRAPH_OK);
    CHECK(graph_is_valid_order(g, NULL, 0));
    graph_destroy(g);
    g = graph_create(5);
    CHECK(g);
    CHECK(graph_add_edge(g, 0, 1) == GRAPH_OK);
    CHECK(graph_add_edge(g, 2, 3) == GRAPH_OK);
    CHECK(graph_topological_sort(g, order, 5) == GRAPH_OK);
    CHECK(graph_is_valid_order(g, order, 5));
    graph_destroy(g);
    puts("PASS: empty graph, disconnected components and isolated vertex");
}

static void test_diamond_and_repeat(void)
{
    Graph *g = graph_create(4);
    size_t first[4], second[4], i;
    CHECK(g);
    CHECK(graph_add_edge(g, 0, 1) == GRAPH_OK);
    CHECK(graph_add_edge(g, 0, 2) == GRAPH_OK);
    CHECK(graph_add_edge(g, 1, 3) == GRAPH_OK);
    CHECK(graph_add_edge(g, 2, 3) == GRAPH_OK);
    CHECK(graph_add_edge(g, 0, 1) == GRAPH_OK);
    CHECK(graph_edge_count(g) == 4);
    CHECK(graph_topological_sort(g, first, 4) == GRAPH_OK);
    CHECK(graph_topological_sort(g, second, 4) == GRAPH_OK);
    CHECK(graph_is_valid_order(g, first, 4));
    for (i = 0; i < 4; ++i) CHECK(first[i] == second[i]);
    graph_destroy(g);
    puts("PASS: converging dependencies, duplicate edge and repeat sorting");
}

static void test_cycles(void)
{
    Graph *g = graph_create(4);
    size_t order[4];
    CHECK(g);
    CHECK(graph_add_edge(g, 1, 2) == GRAPH_OK);
    CHECK(graph_add_edge(g, 2, 3) == GRAPH_OK);
    CHECK(graph_add_edge(g, 3, 1) == GRAPH_OK);
    CHECK(graph_topological_sort(g, order, 4) == GRAPH_CYCLE);
    graph_destroy(g);
    g = graph_create(1);
    CHECK(g);
    CHECK(graph_add_edge(g, 0, 0) == GRAPH_OK);
    CHECK(graph_topological_sort(g, order, 4) == GRAPH_CYCLE);
    graph_destroy(g);
    puts("PASS: directed cycle in disconnected component and self-loop");
}

static void test_invalid_arguments(void)
{
    Graph *g = graph_create(2);
    size_t good[] = {0, 1}, reverse[] = {1, 0};
    size_t duplicate[] = {0, 0}, outside[] = {0, 2};
    CHECK(g);
    CHECK(graph_add_edge(g, 0, 1) == GRAPH_OK);
    CHECK(graph_add_edge(g, 2, 0) == GRAPH_INVALID);
    CHECK(graph_add_edge(NULL, 0, 1) == GRAPH_INVALID);
    CHECK(graph_topological_sort(g, NULL, 2) == GRAPH_INVALID);
    CHECK(graph_topological_sort(g, good, 1) == GRAPH_INVALID);
    CHECK(graph_topological_sort(NULL, good, 2) == GRAPH_INVALID);
    CHECK(graph_is_valid_order(g, good, 2));
    CHECK(!graph_is_valid_order(g, reverse, 2));
    CHECK(!graph_is_valid_order(g, duplicate, 2));
    CHECK(!graph_is_valid_order(g, outside, 2));
    CHECK(!graph_is_valid_order(g, good, 1));
    CHECK(!graph_is_valid_order(g, NULL, 2));
    CHECK(build_course_graph(NULL) == GRAPH_INVALID);
    graph_destroy(NULL);
    graph_destroy(g);
    puts("PASS: invalid vertex IDs, buffers, orders and NULL arguments");
}

static void test_exhaustive(void)
{
    const size_t sources[] = {0,0,1,1,2,2};
    const size_t destinations[] = {1,2,0,2,0,1};
    const size_t permutations[6][3] = {
        {0,1,2}, {0,2,1}, {1,0,2}, {1,2,0}, {2,0,1}, {2,1,0}
    };
    unsigned int mask;
    for (mask = 0; mask < 64; ++mask) {
        Graph *g = graph_create(3);
        size_t order[3], i, p;
        int possible = 0;
        GraphStatus status;
        CHECK(g);
        for (i = 0; i < 6; ++i)
            if (mask & (1u << i))
                CHECK(graph_add_edge(g, sources[i], destinations[i]) == GRAPH_OK);
        /* Independent brute-force oracle, without the graph validator. */
        for (p = 0; p < 6; ++p) {
            size_t position[3];
            int valid = 1;
            for (i = 0; i < 3; ++i) position[permutations[p][i]] = i;
            for (i = 0; i < 6; ++i)
                if ((mask & (1u << i)) &&
                    position[sources[i]] >= position[destinations[i]]) valid = 0;
            if (valid) possible = 1;
        }
        status = graph_topological_sort(g, order, 3);
        if (possible) {
            size_t position[3];
            CHECK(status == GRAPH_OK);
            CHECK(graph_is_valid_order(g, order, 3));
            for (i = 0; i < 3; ++i) position[order[i]] = i;
            for (i = 0; i < 6; ++i)
                if (mask & (1u << i))
                    CHECK(position[sources[i]] < position[destinations[i]]);
        } else CHECK(status == GRAPH_CYCLE);
        graph_destroy(g);
    }
    puts("PASS: all 64 directed three-vertex graphs against brute-force oracle");
}

int main(void)
{
    test_assignment();
    test_empty_and_disconnected();
    test_diamond_and_repeat();
    test_cycles();
    test_invalid_arguments();
    test_exhaustive();
    puts("All 6 test groups passed.");
    return EXIT_SUCCESS;
}
