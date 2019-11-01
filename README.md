Read Me for Homework 3!

# Foreword
I had a ton of fun with this program. I thought it was intersting, head, and a great learning experience. A++ on the assignment!

# Part 1
## How I did it
My implementation for BFS is very simple. 
I have a queue implementation in the queue.c file, and implemented code to read in the snap graph and load it into a graph structure. 
My graph structure is a very simple array of adjacency lists.
I used the Enron email directed graph from the snap data set. 
This graph is included in my submission. 
A different graph can be used for the program.
However, you will need to change the path string in the program to use it.

## Instructions for compilation and execution 
The instructions for compiling part 1 are as follows...
`gcc serialBFS.c queue.c graph.c`
This will compile the program to a.out.
You can run this by executing `./a.out`.

# Part 2
## How I did it
I used the same structures for the parallel BFS that I used for the serial.
For partitioning I chose random nodes in the graph to start at each process.
I have a code block in the source that goes into detail about how this program improves over the serial implementation.
However, I will also talk about this here.
The parallel BFS improves over the serial in that each process searchs a sub graph. 
I akin the subgraphs to the fire tower problem when explaining voronoi diagrams.
As each process searchs, its subgraph will be defined centered at its starting node.
I accomplished the parallel bfs by sharing the visited nodes with every other process after each iteration.
This is done so that work is not being repeated by other processes!

## Instructions for compilation and execution
The instructions for compiling part 2 are as follows...
`mpicc parBFS.c queue.c graph.c`
This will compile the program to a.out.
You can run this by executing `mpirun -n {{Insert desired process number}} ./a.out`.

# Part 3
## How I did it
I extended my second assignment by accomodating it for direction optimized BFS.
I implemented Steve Beamers instructions in the paper.
My heuristic is calculated by comparing the number of edges to check in the graph to the number of edges to check in the frontier.
If the graph has less to check, then do a bottom up search!
Also, I included a frontier conversion to a bitmap per advised by the paper.
These actions are commented in the source.
Also, I include print statements to show when the program switches to bottom up and when its in top down.

## Instructions for compilation and execution
The instructions for compiling part 3 are as follows...
`mpicc dirOptBFS.c queue.c graph.c`
This will compile the program to a.out.
You can run this by executing `mpirun -n {{Insert desired process number}} ./a.out`.
