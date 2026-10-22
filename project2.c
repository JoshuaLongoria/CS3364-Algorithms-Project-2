#include <stdio.h>
#include <string.h>
char CLASSLIST[][5][10] = {
    {"CS1411"},{"MATH1451"},{"ENGL1301"},{"CS1412","CS1411"},
     {"MATH1452","MATH1451"},{"PHYS1408","MATH1451"}, {"ENGL1302","ENGL1301"},{"CS2413","CS1412"},
     {"CS1382","CS1411"},{"ECE2372","MATH1451"},{"MATH2450","MATH1452"},{"PHYS2401","PHYS1408"},
     {"CS2350","CS1412","ECE2372"},{"CS2365","CS2413"},{"ENGR2392"},{"POLS1301"},{"MATH2360","MATH1452"},
     {"ENGL2311","ENGL1301","ENGL1302"},{"CS3361","CS2413"},{"CS3364","CS2413","CS1382","MATH2360"},
     {"MATH3342","MATH2450"},{"POLS2306"},{"CS3365","CS2365","CS2413","MATH3342"},
     {"CS3375","CS2350"},{"CS3383","CS1382"},{"CS4365","CS3365"},{"CS4352","CS3364","CS3375"},
     {"CS4354","CS3364"},{"CS4366","CS4365"}
};

int LISTSIZE = sizeof(CLASSLIST)/sizeof(CLASSLIST[0]);


void generateDAG(char dag[][5][10])
{
    for (int i=0; i<LISTSIZE; i++)
    {
        strcpy(dag[i][0],CLASSLIST[i][0]); // copys from CLASSLIST to dag
        int connect = 1; 
        for (int j=0; j<LISTSIZE; j++)
        {
            for (int k=1; k<5; k++)
            {
                if (strcmp(CLASSLIST[j][k],CLASSLIST[i][0])==0 && connect <5)
                {
                    strcpy(dag[i][connect],CLASSLIST[j][0]);
                    connect++;
                }
            }
        }
    }
}

int generateGraph(char dag[][5][10],char V[][10],char E[][2][10])
{
    int edgenum = 0;
    for (int i=0; i<LISTSIZE; i++) //Loop throuhg dag list and copy to V
    {
        strcpy(V[i],dag[i][0]);
        for (int j=1; j<5; j++) //Loop through V connections
        {
            if (dag[i][j][0] != '\0') // add Edges to list E
            {
                strcpy(E[edgenum][0],dag[i][0]);
                strcpy(E[edgenum][1],dag[i][j]);
                edgenum++;
            }
        }
        
    }  
    return edgenum;
}

int main()
{
    char dag[LISTSIZE][5][10],V[LISTSIZE][10],E[100][2][10];
    memset(dag, 0, sizeof(dag));
    memset(V, 0, sizeof(V));
    memset(E, 0, sizeof(E));
    
    generateDAG(dag);
    int edgenum = generateGraph(dag, V, E);
    
    printf("dag:");
    for (int i=0; i<LISTSIZE; i++)
    {
        printf("(");
        for (int j=0; j<5;j++)
        {
            if (dag[i][j][0] != '\0') printf("%s, ",dag[i][j]);
        }    
        printf("\b\b)\n");
    }
    printf("V: ");
    for (int i=0; i<LISTSIZE; i++) {printf("%s, ",V[i]);}
    printf("\b\b\n");
    printf("E:");
    for (int i = 0; i < edgenum; i++)
    {
        printf("(%s, %s) ", E[i][0], E[i][1]);
    }
    printf("\b\n");
    
}