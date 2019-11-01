#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "queue.h"
#include "list.h"

struct AdjListNode { 
    int dest; 
    struct AdjListNode* next; 
}; 

int getStartNode(char* line);
int getEndNode(char* line);
void bfs(struct Graph* graph, int graph_size, int start);

int main() {
    FILE *texasRoads = fopen("./email-Enron.txt", "r");
    char line[256];
    char number[20];
    int i;
    int nodes;
    for (i = 0; i < 2; i++) {
        fgets(line, sizeof(line), texasRoads);
//        printf("%s", line);
    }
    fgets(line, sizeof(line), texasRoads);
    i = 9;
    int j = 0;
    while (*(line + i) != ' ') {
        *(number + j++) = *(line + i++);
    }
    nodes = atoi(number);
    struct Graph* graph = createGraph(nodes); 
    //printf("Nodes: %d\n", nodes);
    while(fgets(line, sizeof(line), texasRoads)) {
        int start = getStartNode(line);
        int end = getEndNode(line);
        //printf("Start: %d, End: %d\n", start, end);
        addEdge(graph, start, end); 
    }
    fclose(texasRoads);
    bfs(graph, nodes, 0);
    //printGraph(graph); 
    return 0;
}

void bfs(struct Graph* graph, int graph_size, int start) {
    struct queue* frontier = createQueue();
    int *visited = calloc(graph_size, sizeof(int));
    visited[start] = 1;
    enqueue(frontier, start);
    int count = 0;

    while(!isEmpty(frontier)) {
        // Print
        int vert = dequeue(frontier);
        //printf("Cur: %d\n", vert);
        struct AdjListNode* temp = getHeadNode(graph, vert);
        while (temp) {
            int adjVert = temp->dest;
            if (visited[adjVert] == 0) {
                count++;
                visited[adjVert] = 1;
                enqueue(frontier, adjVert);
            }
            temp = temp->next;
        }
    }
    printf("Visited %d nodes!\n");
    printf("BFS complete!\n");
}



int getStartNode(char* line) {
    int i = 0;
    char index = *(line+i);
    while(index != '\t') {
       index = *(line + ++i);
    }
    char start[i];
    memcpy(start, line, i-1);
    start[i-1] = '\0';
    int node = atoi(start);
    return node;
}

int getEndNode(char* line) {
    int i = 0;
    char index = *(line+i);
    while(index != '\t') {
       index = *(line + ++i);
    }
    while (index == '\t') {
        index = *(line + ++i);
    }
    int endNodeIndex = i;
    while(index != '\n') {
       index = *(line + ++i);
    }
    char end[i-endNodeIndex];
    memcpy(end, line+endNodeIndex, i-endNodeIndex-1);
    end[i-endNodeIndex-1] = "\0";
    int node = atoi(end);
    //printf("%d\n", i-endNodeIndex-1);
    //printf("i=%d, nodeindex=%d\n", i,endNodeIndex);
  //  printf("%d\n", ids[1]);
    return node;
}
