#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "graph.h"
#include "queue.h"
#include "list.h"

struct AdjListNode { 
    int dest; 
    struct AdjListNode* next; 
}; 

int getStartNode(char* line);
int getEndNode(char* line);
void bfs(MPI_Comm com, int rank, int world_size, struct Graph* graph, int graph_size);

int main() {
    MPI_Init(NULL, NULL);
    MPI_Comm com = MPI_COMM_WORLD;
    int world_size;
    int rank;
    MPI_Comm_rank(com, &rank);
    MPI_Comm_size(com, &world_size);
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
    bfs(com, rank, world_size, graph, nodes);
    printf("Process %d done\n", rank);
    MPI_Finalize();
    //printGraph(graph); 
    return 0;
}

void bfs(MPI_Comm com, int rank, int world_size, struct Graph* graph, int graph_size) {
    struct queue* frontier = createQueue();
    char* visited = calloc(graph_size, sizeof(char));
    char* changedBuffer = calloc(graph_size, sizeof(char));
    int start = (graph_size/world_size) * rank;
    visited[start] = 1;
    enqueue(frontier, start);

    while(!isEmpty(frontier)) {
        char* changed = calloc(graph_size, sizeof(char));
        // Print
        int vert = dequeue(frontier);
        printf("Cur: %d\n", vert);
        struct AdjListNode* temp = getHeadNode(graph, vert);
        while (temp) {
            int adjVert = temp->dest;
            if (visited[adjVert] == 0) {
                changed[adjVert] = 1;
                visited[adjVert] = 1;
                enqueue(frontier, adjVert);
            }
            temp = temp->next;
        }
        int i;
        int j;
        for (i = 0; i < world_size; i++) {
            if (i != rank) {
                MPI_Send(changed, graph_size, MPI_CHAR, i, 0, com);
            } else {
                for (j = 0; j < world_size; j++) {
                    if (j != rank) {
                        MPI_Recv(changedBuffer, graph_size, MPI_INT, j, 0, com, MPI_STATUS_IGNORE);
                        int k;
                        for (k = 0; k < graph_size; k++) {
                            if (changedBuffer[k] == 1) {
                                visited[k] = 1;
                            }
                        }
                    }
                }
            }
        }
        free(changed);
        /*
        MPI_Request req;
        for (i = 0; i < world_size; i++) {
            if (i != rank) {
                MPI_Isend(visited, graph_size, MPI_INT, i, 0, com, &req);
            }
        }
        for (i = 0; i < world_size; i++) {
            if (i != rank) {
                MPI_Irecv(recvBuffer, graph_size, MPI_INT, i, 0, com, &req);
                MPI_Wait(&req, 1);
                int subCount = (graph_size/world_size);
                int j;
                for (j = subCount*i; j < subCount*(i+1); j++) {
                    visited[j] = recvBuffer[j];
                }
            }
        }
        */
    }

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
