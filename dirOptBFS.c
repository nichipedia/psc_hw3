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

/**
 * README COMMENTS!!!!
 * This is a comments block that is a identical copy of whats found in the README.
 * This is the implementation for the Hybrid BFS algorithm.
 * It introduces a bottom up search into the classic BFS algorithm.
 * This switch will happen when the following heuristic is encountered.
 * For each process, if the edges to check on the frontier is ever larger than the edges to check on the graph then switch to bottom up search.
 * This heuristic optimizes BFS only when the frontier is esspicially large.
 * It limit the ammount of false children found on the graph.
 * It will also optimize the most inefficent portion of BFS (that being when the frontier is large).
 * I have the switch highlighted in the code below.
 */
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
    if (rank == 0) {
        printf("Total Nodes of graph: %d\n", nodes);
        char* running = malloc(world_size*sizeof(char));    
        int i;
        for (i=0; i < world_size; i++) {
            running[i] = 1;
        }
        char run = 1;
        char status;
        while (run) {
            char sum = 0;
            for (i = 0; i<world_size; i++) {
                if (i != 0 && running[i] == 1) {
                    MPI_Recv(&status, 1, MPI_CHAR, i, 0, com, MPI_STATUS_IGNORE);
                    running[i] = status;
                }
                sum += running[i];
            }
            if (sum == 1) {
                run = 0;
            } else {
                for (i = 1; i<world_size; i++) {
                    if (running[i] == 1) {
                        MPI_Send(running, world_size, MPI_CHAR, i, 0, com);
                    }
                }
            }
        }
    } else {
        bfs(com, rank, world_size, graph, nodes);
    }
    printf("Process %d done\n", rank);
    MPI_Barrier(com);
    MPI_Finalize();
    //printGraph(graph); 
    return 0;
}

int checkDegrees(struct queue* frontier, struct Graph* graph) {
    int size = sizeOfQ(frontier);
    int count = 0;
    int i;
    for (i = 0; i < size; i++) {
        int vert = dequeue(frontier);
        struct AdjListNode* temp = getHeadNode(graph, vert);
        while (temp) {
            count++;
            temp=temp->next;
        }
        enqueue(frontier, vert);
    }
    return count;
}

int checkUnexplored(struct Graph* graph, char* visited, int graph_size) {
    int count = 0;
    int i;
    for (i=0; i < graph_size; i++) {
        if (!visited[i]) {
            struct AdjListNode* temp = getHeadNode(graph, i);
            while (temp) {
                count++;
                temp=temp->next;
            }
        }
    }
    return count;
}

void bfs(MPI_Comm com, int rank, int world_size, struct Graph* graph, int graph_size) {
    struct queue* frontier = createQueue();
    char* visited = calloc(graph_size, sizeof(char));
    char* changedBuffer = calloc(graph_size, sizeof(char));
    char* running = malloc(world_size*sizeof(char));
    int start = (graph_size/world_size) * rank;
    int count = 0;
    visited[start] = 1;
    char run = 1;
    enqueue(frontier, start);
    

    while(!isEmpty(frontier)) {
        MPI_Send(&run, 1, MPI_CHAR, 0, 0, com);
        MPI_Recv(running, world_size, MPI_CHAR, 0, 0, com, MPI_STATUS_IGNORE);
        char* changed = calloc(graph_size, sizeof(char));
        // Below is the heuristic for switching to bottom up...
        if (checkUnexplored(graph, visited, graph_size) < checkDegrees(frontier, graph)) {
           // Bottom up search here... 
           int i;
           for (i=0; i < graph_size; i++) {
                if (visited[i] == 0) {
                    struct AdjListNode* temp = getHeadNode(graph, i);
                    while (temp) {
                        if (contains(frontier, i)) {
                            changed[i] = 1;
                            visited[i] = 1;
                            count++;
                            enqueue(frontier, i);
                            break;
                        }
                        temp=temp->next;
                    }
                    
                }
           }
        } else {
            // Classic BFS search here.
            int vert = dequeue(frontier);
            //printf("Cur: %d\n", vert);
            struct AdjListNode* temp = getHeadNode(graph, vert);
            while (temp) {
                int adjVert = temp->dest;
                if (visited[adjVert] == 0) {
                    changed[adjVert] = 1;
                    visited[adjVert] = 1;
                    count++;
                    enqueue(frontier, adjVert);
                }
                temp = temp->next;
            }
        }
        int i;
        int j;
        for (i = 1; i < world_size; i++) {
            if (running[i] && i != rank) {
                MPI_Send(changed, graph_size, MPI_CHAR, i, 0, com);
            } else if (running[i]) {
                for (j = 1; j < world_size; j++) {
                    if (running[j] && j != rank) {
                        MPI_Recv(changedBuffer, graph_size, MPI_CHAR, j, 0, com, MPI_STATUS_IGNORE);
                        int k;
                        for (k = 1; k < graph_size; k++) {
                            if (changedBuffer[k] == 1) {
                                visited[k] = 1;
                            }
                        }
                    }
                }
            }
        }
        free(changed);
    }
    printf("Nodes searched by process %d: %d\n", rank, count);
    run = 0;
    MPI_Send(&run, 1, MPI_CHAR, 0, 0, com);
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
