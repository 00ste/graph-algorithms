#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLACK 0
#define GRAY 1
#define WHITE 2

float *graph_create_random(unsigned int vertices, float density) {
	// Check density value
	if (density < 0 || density > 1) {
		fprintf(stderr, "Density should be a number between 0 and 1\n");
		return NULL;
	}

	// Allocate a new graph
	float *graph = (float *)malloc(sizeof(float) * vertices * vertices);
	if (graph == NULL) {
		fprintf(stderr, "Not enough memory\n");
		return NULL;
	}

	// Init graph with random edges
	size_t i, j;
	for (i = 0; i < vertices; i++) {
		for (j = 0; j < vertices; j++) {
			if ((float)(rand() % 100) / 100 < density) {
				graph[i*vertices + j] = (float)(rand() % 100) / 100;
			}
		}
	}

	return graph;
}

void graph_print(float *graph, unsigned int vertices) {
	if (graph == NULL) {
		fprintf(stderr, "graph_print: graph is NULL\n");
		return;
	}

	size_t i, j;
	for (i = 0; i < vertices; i++) {
		for (j = 0; j < vertices; j++) {
			if (graph[i*vertices + j] == 0)
				printf("       ");
			else
				printf("%.2f   ", graph[i*vertices + j]);

		}
		printf("\n");
	}
}

void graph_get_neighbors(float *graph, unsigned int vertices, unsigned int vertex, unsigned int *neighbors, unsigned int *num_neighbors) {
	if (neighbors == NULL) {
		fprintf(stderr, "graph_get_neighbors: neighbors is NULL\n");
		return;
	}
	if (graph == NULL) {
		fprintf(stderr, "graph_get_neighbors: graph is NULL\n");
		return;
	}
	if (vertex >= vertices) {
		fprintf(stderr, "graph_get_neighbors: vertex %u does not exist\n", vertex);
		return;
	}
	
	(*num_neighbors) = 0;
	unsigned int i;
	for (i = 0; i < vertices; i++) {
		if (graph[vertex * vertices + i] > 0) {
			neighbors[(*num_neighbors)] = i;
			(*num_neighbors) += 1;
		}
	}

#ifdef DEBUG
	printf("Vertex %u has neighbors: ", vertex);
	for (i = 0; i < (*num_neighbors); i++) {
		printf("%u ", neighbors[i]);
	}
	printf("\n");
#endif /* DEBUG */
}

void graph_explore_depth_first_recursive(float *graph, unsigned int *mark, unsigned int vertices, unsigned int vertex) {
	if (mark == NULL) {
                fprintf(stderr, "graph_explore_depth_first_recursive: mark is NULL\n");
                return;
        }
        if (graph == NULL) {
                fprintf(stderr, "graph_explore_depth_first_recursive: graph is NULL\n");
                return;
        }
	if (vertex >= vertices) {
                fprintf(stderr, "graph_explore_depth_first_recursive: vertex %u does not exist\n", vertex);
                return;
        }
	
	// Do not visit if vertex is not WHITE
	if (mark[vertex] != WHITE) return;

	// Mark the vertex GRAY
	mark[vertex] = GRAY;
	printf("New vertex visited: %u\n", vertex);
	
	// Explore all neighbors
	unsigned int num_neighbors;
	unsigned int *neighbors = (unsigned int *)malloc(sizeof(unsigned int) * vertices);

	graph_get_neighbors(graph, vertices, vertex, neighbors, &num_neighbors);

	unsigned int i;
	for (i = 0; i < num_neighbors; i++) {
		graph_explore_depth_first_recursive(graph, mark, vertices, neighbors[i]);
	}
	
	// After all neighbors have been visited, mark node BLACK
	mark[vertex] = BLACK;
}

void graph_explore_depth_first(float *graph, unsigned int vertices) {
	if (graph == NULL) {
		fprintf(stderr, "graph_explore_depth_first: graph is NULL\n");
		return;
	}
	
	// Create a data structure to mark vertices as:
	// - WHITE: not visited
	// - GRAY:  visited, neighbors not visited
	// - BLACK: visited, all neighbors visited
	// All vertices are marked WHITE at first
	unsigned int *mark = (unsigned int *)malloc(sizeof(unsigned int) * vertices);
	unsigned int i;
	for (i = 0; i < vertices; i++) {
		mark[i] = WHITE;
	}
	
	// Explore each node depth-first
	unsigned int vertex;
	for (vertex = 0; vertex < vertices; vertex++) {
		printf("Starting exploration from %u\n", vertex);
               	graph_explore_depth_first_recursive(graph, mark, vertices, vertex);
	}	
}

struct queue_node {
	unsigned int value;
	struct queue_node *next;
};

struct queue {
	struct queue_node *front;
	struct queue_node *back;
};

void queue_enqueue(struct queue *q, unsigned int vertex) {
	// Create new node
	struct queue_node *node = (struct queue_node *)malloc(sizeof(struct queue_node));
	node->value = vertex;
	node->next = NULL;
	
	// Place at the back of the queue
	if (q->back != NULL)
		q->back->next = node;
	if (q->front == NULL)
		q->front = node;
	q->back = node;
}

unsigned int queue_dequeue(struct queue *q) {
	// Fetch node at the front of the queue
	struct queue_node *node = q->front;

	// Set the new queue front to the next node
	q->front = node->next;
	
	// If there is only one node left, reset the back of the queue
	if (q->back == node) q->back = NULL;
	
	// Extract the value and free the node
	unsigned int result = node->value;
	free(node);
	return result;	
}

void graph_explore_breadth_first(float *graph, unsigned int vertices) {
 	if (graph == NULL) {
		fprintf(stderr, "graph_explore_breadth_first: graph is NULL\n");
		return;
	}

	// Create a data structure to mark vertices as:
	// - WHITE: not visited
	// - GRAY:  visited, neighbors not visited
	// - BLACK: visited, all neighbors visited
	// All vertices are marked WHITE at first
	unsigned int *mark = (unsigned int *)malloc(sizeof(unsigned int) * vertices);

	// Create a queue to store the vertices to be visited
	struct queue queue;
	queue.front = NULL;
	queue.back = NULL;
	
	// Enqueue every vertex and explore breadth first
	unsigned int i;
	for (i = 0; i < vertices; i++) {
		// Enqueue only if the vertex is WHITE
		if (mark[i] == WHITE) {
			printf("Starting exploration from vertex %u\n", i);
			queue_enqueue(&queue, i);
		}
		// Explore all vertices in the queue, as the exploration goes on,
		// more nodes will be added to the queue. The exploration ends when
		// all nodes have been visited, which means the queue will be empty.
		while(queue.back != NULL) {
			// Get the first vertex in the queue
			unsigned int vertex = queue_dequeue(&queue);
			
			// Do not visit if vertex is not WHITE
			if (mark[vertex] != WHITE) continue;

			// Mark the vertex GRAY
			mark[vertex] = GRAY;
			printf("New vertex visited: %u\n", vertex);

			// Enqueue all neighbors
			unsigned int num_neighbors;
			unsigned int *neighbors = (unsigned int *)malloc(sizeof(unsigned int) * vertices);
	
			graph_get_neighbors(graph, vertices, vertex, neighbors, &num_neighbors);
	
			unsigned int i;
			for (i = 0; i < num_neighbors; i++) {
				queue_enqueue(&queue, neighbors[i]);
			}

			// After all neighbors have been visited, mark vertex BLACK
			mark[vertex] = BLACK;
		}
	}
}

void graph_explore_breadth_first_from_vertex(float *graph, unsigned int vertices, unsigned int first_vertex) {
	if (graph == NULL) {
                fprintf(stderr, "graph_explore_breadth_first_from_vertex: graph is NULL\n");
                return;
        }
	if (first_vertex >= vertices) {
		fprintf(stderr, "graph_explore_breadth_first_from_vertex: first_vertex %u does not exist\n", first_vertex);
		return;
	}

	// Create a data structure to mark vertices as:
        // - WHITE: not visited
        // - GRAY:  visited, neighbors not visited
        // - BLACK: visited, all neighbors visited
        // All vertices are marked WHITE at first
        unsigned int *mark = (unsigned int *)malloc(sizeof(unsigned int) * vertices);
        unsigned int i;
        for (i = 0; i < vertices; i++) {
                mark[i] = WHITE;
        }

	// Create a queue to store the vertices to be visited
	struct queue queue;
	queue.front = NULL;
	queue.back = NULL;

	// Enqueue the first vertex to visit
	queue_enqueue(&queue, first_vertex);
	
	// Explore all vertices in the queue, as the exploration goes on,
	// more nodes will be added to the queue. The exploration ends when
	// all nodes have been visited, which means the queue will be empty.
	while(queue.back != NULL) {
		// Get the first vertex in the queue
		unsigned int vertex = queue_dequeue(&queue);
		
		// Do not visit if vertex is not WHITE
		if (mark[vertex] != WHITE) continue;

		// Mark the vertex GRAY
		mark[vertex] = GRAY;
		printf("New vertex visited: %u\n", vertex);

		// Enqueue all neighbors
        	unsigned int num_neighbors;
        	unsigned int *neighbors = (unsigned int *)malloc(sizeof(unsigned int) * vertices);
	
	        graph_get_neighbors(graph, vertices, vertex, neighbors, &num_neighbors);
	
       	 	unsigned int i;
        	for (i = 0; i < num_neighbors; i++) {
                	queue_enqueue(&queue, neighbors[i]);
        	}

		// After all neighbors have been visited, mark vertex BLACK
		mark[vertex] = BLACK;
	}
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s, <num_vertices> <density>\n", argv[0]);
		return EXIT_FAILURE;
	}

	srand((unsigned)time(NULL));

	unsigned int vertices = atoi(argv[1]);
	float *graph = graph_create_random(vertices, atof(argv[2]));

	graph_print(graph, vertices);
	
	printf("DEPTH-FIRST EXPLORATION\n");
	graph_explore_depth_first(graph, vertices);
	
	printf("BREADTH-FIRST EXPLORATION\n");
	graph_explore_breadth_first(graph, vertices);
	return EXIT_SUCCESS;
}
