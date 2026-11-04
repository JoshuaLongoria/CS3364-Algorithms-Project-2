#include "courses.h"
#include <string.h>


const Course courses[] = {
    {"CS 1411", "Programming Principles I", {NULL}, 0},
    {"MATH 1451", "Calculus I with Applications", {NULL}, 0},
    {"ENGL 1301", "Essentials of College Rhetoric", {NULL}, 0},
    {"CS 1412", "Programming Principles II", {"CS 1411"}, 1},
    {"MATH 1452", "Calculus II with Applications", {"MATH 1451"}, 1},
    {"PHYS 1408", "Principles of Physics I", {"MATH 1451"}, 1},
    {"ENGL 1302", "Advanced College Rhetoric", {"ENGL 1301"}, 1},
    {"CS 2413", "Data Structures", {"CS 1412"}, 1},
    {"CS 1382", "Discrete Computational Structures", {"CS 1411"}, 1},
    {"ECE 2372", "Modern Digital System Design", {"MATH 1451"}, 1},
    {"MATH 2450", "Calculus III with Applications", {"MATH 1452"}, 1},
    {"PHYS 2401", "Principles of Physics II", {"PHYS 1408"}, 1},
    {"CS 2350", "Computer Organization and Assembly Language Programming", {"CS 1412", "ECE 2372"}, 2},
    {"CS 2365", "Object-Oriented Programming", {"CS 2413"}, 1},
    {"ENGR 2392", "Engineering Ethics and Its Impact on Society", {NULL}, 0},
    {"POLS 1301", "American Government", {NULL}, 0},
    {"MATH 2360", "Linear Algebra", {"MATH 1452"}, 1},
    {"ENGL 2311", "Introduction to Technical Writing", {"ENGL 1301", "ENGL 1302"}, 2},
    {"CS 3361", "Concepts of Programming Languages", {"CS 2413"}, 1},
    {"CS 3364", "Design and Analysis of Algorithms", {"CS 2413", "CS 1382", "MATH 2360"}, 3},
    {"MATH 3342", "Mathematical Statistics for Engineers and Scientists", {"MATH 2450"}, 1},
    {"POLS 2306", "Texas Politics and Topics", {NULL}, 0},
    {"CS 3365", "Software Engineering I", {"CS 2365", "CS 2413", "MATH 3342"}, 3},
    {"CS 3375", "Computer Architecture", {"CS 2350"}, 1},
    {"CS 3383", "Theory of Automata", {"CS 1382"}, 1},
    {"CS 4365", "Software Engineering II", {"CS 3365"}, 1},
    {"CS 4352", "Operating Systems", {"CS 3364", "CS 3375"}, 2},
    {"CS 4354", "Concepts of Database Systems", {"CS 3364"}, 1},
    {"CS 4366", "Senior Capstone Project", {"CS 4365"}, 1},
};
const size_t course_count = sizeof(courses) / sizeof(courses[0]);
static size_t find_course(const char *code)
{
    size_t i;
    for (i = 0; i < course_count; ++i)
        if (strcmp(courses[i].code, code) == 0) return i;
    return course_count;
}

GraphStatus build_course_graph(Graph **result)
{
    Graph *graph;
    size_t i, j;
    if (!result) return GRAPH_INVALID;
    *result = NULL;
    graph = graph_create(course_count);
    if (!graph) return GRAPH_NO_MEMORY;
    for (i = 0; i < course_count; ++i) {
        for (j = 0; j < courses[i].prerequisite_count; ++j) {
            size_t prerequisite = find_course(courses[i].prerequisites[j]);
            GraphStatus status;
            if (prerequisite == course_count) {
                graph_destroy(graph);
                return GRAPH_INVALID;
            }
            status = graph_add_edge(graph, prerequisite, i);
            if (status != GRAPH_OK) {
                graph_destroy(graph);
                return status;
            }
        }
    }
    *result = graph;
    return GRAPH_OK;
}
