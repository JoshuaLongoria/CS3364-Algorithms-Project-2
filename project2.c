/* CS 3364 Project 2: course ordering with DFS.
 * Compile: gcc -std=c11 -Wall -Wextra -Wpedantic project2.c -o project2
 * Run: ./project2
 * Each edge points from a prerequisite to a dependent course.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char CLASSLIST[][5][10] = {
    {"CS1411"},{"MATH1451"},{"ENGL1301"},{"CS1412","CS1411"},
     {"MATH1452","MATH1451"},{"PHYS1408","MATH1451"}, {"ENGL1302","ENGL1301"},{"CS2413","CS1412"},
     {"CS1382","CS1411"},{"ECE2372","MATH1451"},{"MATH2450","MATH1452"},{"PHYS2401","PHYS1408"},
     {"CS2350","CS1412","ECE2372"},{"CS2365","CS2413"},{"ENGR2392"},{"POLS1301"},{"MATH2360","MATH1452"},
     {"ENGL2311","ENGL1301","ENGL1302"},{"CS3361","CS2413"},{"CS3364","CS2413","CS1382","MATH2360"},
     {"MATH3342","MATH2450"},{"POLS2306"},{"CS3365","CS2365","CS2413","MATH3342"},
     {"CS3375","CS2350"},{"CS3383","CS1382"},{"CS4365","CS3365"},{"CS4352","CS3364","CS3375"},
     {"CS4354","CS3364"},{"CS4366","CS4365"}
};

enum { COURSE_COUNT = sizeof(CLASSLIST) / sizeof(CLASSLIST[0]) };
enum { WHITE, GRAY, BLACK };

/* Array-backed adjacency lists: only degree[u] entries are traversed.
 * Capacity grows automatically with the number of listed courses.
 * Storage O(V^2); DFS O(V + E).
 */
typedef struct {
    int count;
    int edges;
    int neighbors[COURSE_COUNT][COURSE_COUNT];
    int degree[COURSE_COUNT];
} Graph;

int graphInit(Graph *g, int count)
{
    if (!g || count < 0 || count > COURSE_COUNT) return 0;
    memset(g, 0, sizeof(*g));
    g->count = count;
    return 1;
}

int addEdge(Graph *g, int source, int destination)
{
    if (!g || source < 0 || source >= g->count ||
        destination < 0 || destination >= g->count) return 0;
    for (int i = 0; i < g->degree[source]; ++i)
        if (g->neighbors[source][i] == destination) return 1;
    if (g->degree[source] >= COURSE_COUNT) return 0;
    g->neighbors[source][g->degree[source]++] = destination;
    ++g->edges;
    return 1;
}

int findCourse(const char *code)
{
    for (int i = 0; i < COURSE_COUNT; ++i)
        if (strcmp(CLASSLIST[i][0], code) == 0) return i;
    return -1;
}

int generateGraph(Graph *g)
{
    if (!graphInit(g, COURSE_COUNT)) return 0;
    for (int course = 0; course < COURSE_COUNT; ++course) {
        for (int p = 1; p < 5; ++p) {
            if (CLASSLIST[course][p][0] == '\0') continue;
            int prerequisite = findCourse(CLASSLIST[course][p]);
            if (prerequisite < 0) {
                fprintf(stderr, "Unknown prerequisite: %s\n", CLASSLIST[course][p]);
                return 0;
            }
            if (!addEdge(g, prerequisite, course)) return 0;
        }
    }
    return 1;
}

/* Gray neighbors are ancestors on the active path, indicating a cycle. */
static int dfs(const Graph *g, int u, int color[], int order[], int *next)
{
    color[u] = GRAY;
    for (int i = 0; i < g->degree[u]; ++i) {
        int v = g->neighbors[u][i];
        if (color[v] == GRAY) return 0;
        if (color[v] == WHITE && !dfs(g, v, color, order, next)) return 0;
    }
    color[u] = BLACK;
    order[--(*next)] = u;  /* Fill right to left: reverse finishing order. */
    return 1;
}

/* Returns 1 on success, 0 for a cycle, -1 for invalid arguments.
 * order must hold at least g->count integers; use it only on success.
 */
int topologicalSort(const Graph *g, int order[], int capacity)
{
    int color[COURSE_COUNT] = {0};
    if (!g || g->count < 0 || g->count > COURSE_COUNT ||
        capacity < g->count || (g->count > 0 && !order)) return -1;
    int next = g->count;
    for (int u = 0; u < g->count; ++u)
        if (color[u] == WHITE && !dfs(g, u, color, order, &next)) return 0;
    return 1;
}

/* Verify that all vertices occur once and every edge goes forward. */
int validateOrder(const Graph *g, const int order[], int length)
{
    int position[COURSE_COUNT];
    if (!g || g->count < 0 || g->count > COURSE_COUNT ||
        length != g->count || (length > 0 && !order)) return 0;
    for (int i = 0; i < g->count; ++i) position[i] = -1;
    for (int i = 0; i < length; ++i) {
        int u = order[i];
        if (u < 0 || u >= g->count || position[u] != -1) return 0;
        position[u] = i;
    }
    for (int u = 0; u < g->count; ++u)
        for (int i = 0; i < g->degree[u]; ++i)
            if (position[u] >= position[g->neighbors[u][i]]) return 0;
    return 1;
}

void printGraph(const Graph *g)
{
    puts("Adjacency lists (prerequisite -> dependent courses):");
    for (int u = 0; u < g->count; ++u) {
        printf("%s -> ", CLASSLIST[u][0]);
        if (g->degree[u] == 0) printf("(none)");
        for (int i = 0; i < g->degree[u]; ++i)
            printf("%s%s", i ? ", " : "", CLASSLIST[g->neighbors[u][i]][0]);
        putchar('\n');
    }
    printf("\nV = {");
    for (int u = 0; u < g->count; ++u)
        printf("%s%s", u ? ", " : "", CLASSLIST[u][0]);
    puts("}");
    puts("\nE (prerequisite, course):");
    for (int u = 0; u < g->count; ++u)
        for (int i = 0; i < g->degree[u]; ++i)
            printf("(%s, %s)\n", CLASSLIST[u][0],
                   CLASSLIST[g->neighbors[u][i]][0]);
}

int main(void)
{
    Graph graph;
    int order[COURSE_COUNT];
    if (!generateGraph(&graph)) {
        fputs("Could not construct the graph.\n", stderr);
        return EXIT_FAILURE;
    }
    printf("Courses: %d; prerequisite edges: %d\n\n", graph.count, graph.edges);
    printGraph(&graph);
    int status = topologicalSort(&graph, order, COURSE_COUNT);
    if (status != 1) {
        fputs(status == 0 ? "Cycle detected: no valid course order exists.\n" :
              "Invalid sorting arguments.\n", stderr);
        return EXIT_FAILURE;
    }
    if (!validateOrder(&graph, order, COURSE_COUNT)) {
        fputs("Course order validation failed.\n", stderr);
        return EXIT_FAILURE;
    }
    puts("\nDFS topological order (one valid solution):");
    for (int i = 0; i < COURSE_COUNT; ++i)
        printf("%2d. %s\n", i + 1, CLASSLIST[order[i]][0]);
    puts("Validation: PASS (every prerequisite precedes its course)");
    return EXIT_SUCCESS;
}
