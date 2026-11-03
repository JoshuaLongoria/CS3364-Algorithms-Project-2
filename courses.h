#ifndef COURSES_H
#define COURSES_H
#include "graph.h"

typedef struct {
    const char *code;
    const char *title;
    const char *prerequisites[3];
    size_t prerequisite_count;
} Course;

extern const Course courses[];
extern const size_t course_count;
/* On success caller owns *result and must graph_destroy it. */
GraphStatus build_course_graph(Graph **result);
#endif
