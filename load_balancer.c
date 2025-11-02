#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* ============================================================================
 * CONSTANTS AND CONFIGURATION
 * ============================================================================ */
#define NUM_SERVERS 6
#define NUM_TASKS 30
#define MIN_CAPACITY 80.0
#define MAX_CAPACITY 120.0
#define MIN_TASK_LOAD 5.0
#define MAX_TASK_LOAD 15.0
#define REBALANCE_THRESHOLD 20.0  // Percentage imbalance threshold
#define REBALANCE_INTERVAL 5      // Rebalance after every N tasks

/* ============================================================================
 * DATA STRUCTURES
 * ============================================================================ */

/* Server Structure: Represents a single server node */
typedef struct {
    int id;
    float capacity;
    float currentLoad;
} Server;

/* Graph Node: Adjacency list node for server connections */
typedef struct Node {
    int serverId;
    struct Node* next;
} Node;

/* Graph Structure: Adjacency list representation of server network */
typedef struct {
    int numServers;
    Node** adjList;
} Graph;

/* Heap Node: Represents a server in the min-heap */
typedef struct {
    int serverId;
    float load;
} HeapNode;

/* Min Heap Structure: Priority queue for least-loaded server selection */
typedef struct {
    HeapNode* arr;
    int size;
    int capacity;
} MinHeap;

/* ============================================================================
 * GRAPH FUNCTIONS
 * ============================================================================ */

/* Create a graph with numServers nodes
 * Time Complexity: O(n)
 */
Graph* createGraph(int numServers) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numServers = numServers;
    
    // Allocate adjacency list
    graph->adjList = (Node**)malloc(numServers * sizeof(Node*));
    
    // Initialize all lists to NULL
    for (int i = 0; i < numServers; i++) {
        graph->adjList[i] = NULL;
    }
    
    return graph;
}

/* Add a directed edge from src to dest in the graph
 * Time Complexity: O(1)
 */
void addEdge(Graph* graph, int src, int dest) {
    // Create new node for adjacency list
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->serverId = dest;
    newNode->next = graph->adjList[src];
    graph->adjList[src] = newNode;
}

/* Print the graph structure for debugging
 * Time Complexity: O(V + E)
 */
void printGraph(Graph* graph) {
    printf("\n--- Server Network Topology ---\n");
    for (int i = 0; i < graph->numServers; i++) {
        printf("Server %d → ", i);
        Node* current = graph->adjList[i];
        while (current != NULL) {
            printf("%d ", current->serverId);
            current = current->next;
        }
        printf("\n");
    }
}

/* Free graph memory
 * Time Complexity: O(V + E)
 */
void freeGraph(Graph* graph) {
    for (int i = 0; i < graph->numServers; i++) {
        Node* current = graph->adjList[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->adjList);
    free(graph);
}

/* ============================================================================
 * MIN HEAP FUNCTIONS
 * ============================================================================ */

/* Create a min heap with given capacity
 * Time Complexity: O(n)
 */
MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->arr = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

/* Swap two heap nodes
 * Time Complexity: O(1)
 */
void swap(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

/* Move a node up the heap to maintain min-heap property
 * Time Complexity: O(log n)
 */
void heapifyUp(MinHeap* heap, int index) {
    if (index == 0) return;
    
    int parentIdx = (index - 1) / 2;
    
    // If current node's load is less than parent, swap
    if (heap->arr[index].load < heap->arr[parentIdx].load) {
        swap(&heap->arr[index], &heap->arr[parentIdx]);
        heapifyUp(heap, parentIdx);
    }
}

/* Move a node down the heap to maintain min-heap property
 * Time Complexity: O(log n)
 */
void heapifyDown(MinHeap* heap, int index) {
    int smallest = index;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    
    // Check if left child exists and is smaller
    if (leftChild < heap->size && 
        heap->arr[leftChild].load < heap->arr[smallest].load) {
        smallest = leftChild;
    }
    
    // Check if right child exists and is smaller
    if (rightChild < heap->size && 
        heap->arr[rightChild].load < heap->arr[smallest].load) {
        smallest = rightChild;
    }
    
    // If smallest is not the current node, swap and continue
    if (smallest != index) {
        swap(&heap->arr[index], &heap->arr[smallest]);
        heapifyDown(heap, smallest);
    }
}

/* Insert a server into the min heap
 * Time Complexity: O(log n)
 */
void insertHeap(MinHeap* heap, int serverId, float load) {
    if (heap->size >= heap->capacity) {
        printf("Heap capacity exceeded!\n");
        return;
    }
    
    heap->arr[heap->size].serverId = serverId;
    heap->arr[heap->size].load = load;
    
    heapifyUp(heap, heap->size);
    heap->size++;
}

/* Extract and return the server with minimum load
 * Time Complexity: O(log n)
 */
HeapNode extractMin(MinHeap* heap) {
    HeapNode min = heap->arr[0];
    
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    
    if (heap->size > 0) {
        heapifyDown(heap, 0);
    }
    
    return min;
}

/* Update a server's load in the heap
 * Time Complexity: O(n) in worst case
 */
void updateHeap(MinHeap* heap, int serverId, float newLoad) {
    // Find the server in the heap
    int index = -1;
    for (int i = 0; i < heap->size; i++) {
        if (heap->arr[i].serverId == serverId) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf("Server %d not found in heap!\n", serverId);
        return;
    }
    
    heap->arr[index].load = newLoad;
    
    // Reheapify from this position
    int parentIdx = (index - 1) / 2;
    if (index > 0 && heap->arr[index].load < heap->arr[parentIdx].load) {
        heapifyUp(heap, index);
    } else {
        heapifyDown(heap, index);
    }
}

/* Free heap memory
 * Time Complexity: O(1)
 */
void freeMinHeap(MinHeap* heap) {
    free(heap->arr);
    free(heap);
}

/* ============================================================================
 * LOAD BALANCING FUNCTIONS
 * ============================================================================ */

/* Calculate average load across all servers
 * Time Complexity: O(n)
 */
float calculateAverageLoad(Server servers[], int numServers) {
    float totalLoad = 0.0;
    for (int i = 0; i < numServers; i++) {
        totalLoad += servers[i].currentLoad;
    }
    return totalLoad / numServers;
}

/* Calculate load percentage for a server
 * Time Complexity: O(1)
 */
float getLoadPercentage(Server server) {
    return (server.currentLoad / server.capacity) * 100.0;
}

/* Find the most loaded server
 * Time Complexity: O(n)
 */
int findMostLoadedServer(Server servers[], int numServers) {
    int mostLoaded = 0;
    float maxLoad = servers[0].currentLoad;
    
    for (int i = 1; i < numServers; i++) {
        if (servers[i].currentLoad > maxLoad) {
            maxLoad = servers[i].currentLoad;
            mostLoaded = i;
        }
    }
    
    return mostLoaded;
}

/* Find the least loaded server
 * Time Complexity: O(n)
 */
int findLeastLoadedServer(Server servers[], int numServers) {
    int leastLoaded = 0;
    float minLoad = servers[0].currentLoad;
    
    for (int i = 1; i < numServers; i++) {
        if (servers[i].currentLoad < minLoad) {
            minLoad = servers[i].currentLoad;
            leastLoaded = i;
        }
    }
    
    return leastLoaded;
}

/* Rebalance loads across servers if imbalance exceeds threshold
 * Time Complexity: O(n log n)
 */
void rebalanceLoads(Server servers[], int numServers, float threshold, 
                    MinHeap* heap) {
    float avgLoad = calculateAverageLoad(servers, numServers);
    
    int mostLoadedIdx = findMostLoadedServer(servers, numServers);
    int leastLoadedIdx = findLeastLoadedServer(servers, numServers);
    
    float mostLoadedPercent = getLoadPercentage(servers[mostLoadedIdx]);
    float leastLoadedPercent = getLoadPercentage(servers[leastLoadedIdx]);
    
    float imbalance = mostLoadedPercent - leastLoadedPercent;
    
    // Check if rebalancing is needed
    if (imbalance > threshold) {
        float migrationAmount = (servers[mostLoadedIdx].currentLoad - avgLoad) * 0.5;
        
        printf("\n⚠️  REBALANCING TRIGGERED ⚠️\n");
        printf("   Imbalance: %.2f%% (threshold: %.2f%%)\n", imbalance, threshold);
        printf("   Server %d (%.2f%%) → Server %d (%.2f%%)\n",
               mostLoadedIdx, mostLoadedPercent,
               leastLoadedIdx, leastLoadedPercent);
        printf("   Migrating %.2f load units\n", migrationAmount);
        
        // Perform load migration
        servers[mostLoadedIdx].currentLoad -= migrationAmount;
        servers[leastLoadedIdx].currentLoad += migrationAmount;
        
        // Update heap with new loads
        updateHeap(heap, mostLoadedIdx, servers[mostLoadedIdx].currentLoad);
        updateHeap(heap, leastLoadedIdx, servers[leastLoadedIdx].currentLoad);
        
        printf("   ✓ Rebalancing complete\n");
    }
}

/* Print current state of all servers
 * Time Complexity: O(n)
 */
void printServerStates(Server servers[], int numServers) {
    printf("\n--- Current Server States ---\n");
    for (int i = 0; i < numServers; i++) {
        float percentage = getLoadPercentage(servers[i]);
        printf("Server %d: Load = %6.2f/%6.2f (%.1f%%)\n",
               servers[i].id, servers[i].currentLoad, 
               servers[i].capacity, percentage);
    }
    
    float avgLoad = calculateAverageLoad(servers, numServers);
    printf("\nAverage Load: %.2f\n", avgLoad);
}

/* Simulate task assignment to servers
 * Time Complexity: O(n log n) for n tasks
 */
void simulateTaskAssignment(Server servers[], Graph* graph, MinHeap* heap, 
                           int numTasks) {
    printf("\n--- Assigning %d Tasks Dynamically ---\n", numTasks);
    
    for (int task = 1; task <= numTasks; task++) {
        // Generate random task load
        float taskLoad = MIN_TASK_LOAD + 
                        (float)rand() / RAND_MAX * 
                        (MAX_TASK_LOAD - MIN_TASK_LOAD);
        
        // Find least-loaded server using heap
        HeapNode minServer = extractMin(heap);
        
        // Assign task to this server
        servers[minServer.serverId].currentLoad += taskLoad;
        float newLoad = servers[minServer.serverId].currentLoad;
        
        // Reinsert server into heap with updated load
        insertHeap(heap, minServer.serverId, newLoad);
        
        float percentage = getLoadPercentage(servers[minServer.serverId]);
        
        printf("Task %2d → Server %d | Load: %6.2f/%6.2f (%.1f%%)\n",
               task, minServer.serverId, newLoad,
               servers[minServer.serverId].capacity, percentage);
        
        // Rebalance periodically
        if (task % REBALANCE_INTERVAL == 0) {
            rebalanceLoads(servers, NUM_SERVERS, REBALANCE_THRESHOLD, heap);
        }
    }
}

/* ============================================================================
 * MAIN SIMULATION
 * ============================================================================ */

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║   DYNAMIC LOAD BALANCING SIMULATION - Distributed System   ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // ========== INITIALIZATION ==========
    printf("\n✓ Initializing %d servers...\n", NUM_SERVERS);
    
    // Create and initialize servers
    Server servers[NUM_SERVERS];
    for (int i = 0; i < NUM_SERVERS; i++) {
        servers[i].id = i;
        servers[i].capacity = MIN_CAPACITY + 
                             (float)rand() / RAND_MAX * 
                             (MAX_CAPACITY - MIN_CAPACITY);
        servers[i].currentLoad = 0.0;
        printf("  Server %d: Capacity = %.2f\n", i, servers[i].capacity);
    }
    
    // Create network graph
    Graph* networkGraph = createGraph(NUM_SERVERS);
    
    // Add random edges to create network topology
    // Each server is connected to at least one other
    for (int i = 0; i < NUM_SERVERS; i++) {
        int neighbors = 1 + (rand() % 3);  // 1-3 connections per server
        for (int j = 0; j < neighbors; j++) {
            int dest = rand() % NUM_SERVERS;
            if (dest != i) {
                addEdge(networkGraph, i, dest);
            }
        }
    }
    
    printGraph(networkGraph);
    
    // Create and initialize min heap
    MinHeap* loadHeap = createMinHeap(NUM_SERVERS);
    for (int i = 0; i < NUM_SERVERS; i++) {
        insertHeap(loadHeap, i, 0.0);
    }
    
    printf("✓ Min-heap initialized with all servers\n");
    
    // ========== TASK ASSIGNMENT PHASE ==========
    simulateTaskAssignment(servers, networkGraph, loadHeap, NUM_TASKS);
    
    // ========== FINAL STATE ==========
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    FINAL LOAD DISTRIBUTION                 ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    printServerStates(servers, NUM_SERVERS);
    
    // Calculate final statistics
    float avgLoad = calculateAverageLoad(servers, NUM_SERVERS);
    float maxLoad = servers[findMostLoadedServer(servers, NUM_SERVERS)].currentLoad;
    float minLoad = servers[findLeastLoadedServer(servers, NUM_SERVERS)].currentLoad;
    float imbalance = maxLoad - minLoad;
    
    printf("\n--- Final Statistics ---\n");
    printf("Average Load:    %.2f\n", avgLoad);
    printf("Max Load:        %.2f\n", maxLoad);
    printf("Min Load:        %.2f\n", minLoad);
    printf("Load Difference: %.2f\n", imbalance);
    
    if (imbalance < REBALANCE_THRESHOLD) {
        printf("\n✓✓✓ System is WELL-BALANCED ✓✓✓\n");
    } else {
        printf("\n⚠ System could benefit from further rebalancing\n");
    }
    
    // ========== CLEANUP ==========
    freeMinHeap(loadHeap);
    freeGraph(networkGraph);
    
    printf("\n✓ Simulation complete. Resources freed.\n\n");
    
    return 0;
}
