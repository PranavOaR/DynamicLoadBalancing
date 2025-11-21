#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

/* ============================================================================
 * DATA STRUCTURES (UNCHANGED FROM ORIGINAL)
 * ============================================================================ */

typedef struct {
    int id;
    float capacity;
    float currentLoad;
} Server;

typedef struct Node {
    int serverId;
    struct Node* next;
} Node;

typedef struct {
    int numServers;
    Node** adjList;
} Graph;

typedef struct {
    int serverId;
    float load;
} HeapNode;

typedef struct {
    HeapNode* arr;
    int size;
    int capacity;
} MinHeap;

/* ============================================================================
 * GLOBAL CONFIGURATION (USER-INPUTTED AT RUNTIME)
 * ============================================================================ */

typedef struct {
    int numServers;
    int numTasks;
    int taskAssignmentMode;      // 0 = manual, 1 = random
    float rebalanceThreshold;
    int rebalanceInterval;
} SimulationConfig;

typedef struct {
    float* taskLoads;
    int numTasks;
} TaskData;

/* ============================================================================
 * UTILITY & INPUT VALIDATION FUNCTIONS
 * ============================================================================ */

void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getIntInput(const char* prompt, int minVal, int maxVal) {
    int value;
    int valid = 0;
    
    while (!valid) {
        printf("%s", prompt);
        if (scanf("%d", &value) != 1) {
            clearBuffer();
            printf("❌ Invalid input! Please enter an integer.\n");
            continue;
        }
        clearBuffer();
        
        if (value < minVal || value > maxVal) {
            printf("❌ Input out of range! Must be between %d and %d.\n", minVal, maxVal);
            continue;
        }
        valid = 1;
    }
    return value;
}

float getFloatInput(const char* prompt, float minVal, float maxVal) {
    float value;
    int valid = 0;
    
    while (!valid) {
        printf("%s", prompt);
        if (scanf("%f", &value) != 1) {
            clearBuffer();
            printf("❌ Invalid input! Please enter a number.\n");
            continue;
        }
        clearBuffer();
        
        if (value < minVal || value > maxVal) {
            printf("❌ Input out of range! Must be between %.2f and %.2f.\n", minVal, maxVal);
            continue;
        }
        valid = 1;
    }
    return value;
}

char getYesNoInput(const char* prompt) {
    char response;
    int valid = 0;
    
    while (!valid) {
        printf("%s (Y/N): ", prompt);
        if (scanf("%c", &response) != 1) {
            clearBuffer();
            printf("❌ Invalid input! Please enter Y or N.\n");
            continue;
        }
        clearBuffer();
        
        response = (response == 'Y' || response == 'y') ? 'Y' : 
                  (response == 'N' || response == 'n') ? 'N' : '?';
        
        if (response == '?') {
            printf("❌ Please enter Y or N.\n");
            continue;
        }
        valid = 1;
    }
    return response;
}

/* ============================================================================
 * GRAPH FUNCTIONS (UNCHANGED FROM ORIGINAL)
 * ============================================================================ */

Graph* createGraph(int numServers) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numServers = numServers;
    graph->adjList = (Node**)malloc(numServers * sizeof(Node*));
    
    for (int i = 0; i < numServers; i++) {
        graph->adjList[i] = NULL;
    }
    
    return graph;
}

int edgeExists(Graph* graph, int src, int dest) {
    Node* current = graph->adjList[src];
    while (current != NULL) {
        if (current->serverId == dest) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int addEdge(Graph* graph, int src, int dest) {
    // Validate edge
    if (src < 0 || src >= graph->numServers || dest < 0 || dest >= graph->numServers) {
        printf("❌ Server ID out of range! Valid range: 0-%d\n", graph->numServers - 1);
        return 0;
    }
    
    if (src == dest) {
        printf("❌ Cannot add self-edge (Server %d → Server %d).\n", src, dest);
        return 0;
    }
    
    if (edgeExists(graph, src, dest)) {
        printf("❌ Edge already exists (Server %d → Server %d).\n", src, dest);
        return 0;
    }
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->serverId = dest;
    newNode->next = graph->adjList[src];
    graph->adjList[src] = newNode;
    
    printf("✓ Added edge: Server %d → Server %d\n", src, dest);
    return 1;
}

void printGraph(Graph* graph) {
    printf("\n┌─── Server Network Topology ───┐\n");
    for (int i = 0; i < graph->numServers; i++) {
        printf("│ Server %d → ", i);
        Node* current = graph->adjList[i];
        
        if (current == NULL) {
            printf("(no connections)\n");
        } else {
            while (current != NULL) {
                printf("%d", current->serverId);
                current = current->next;
                if (current != NULL) printf(", ");
            }
            printf("\n");
        }
    }
    printf("└───────────────────────────────┘\n");
}

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
 * MIN HEAP FUNCTIONS (UNCHANGED FROM ORIGINAL)
 * ============================================================================ */

MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->arr = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void swap(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(MinHeap* heap, int index) {
    if (index == 0) return;
    
    int parentIdx = (index - 1) / 2;
    
    if (heap->arr[index].load < heap->arr[parentIdx].load) {
        swap(&heap->arr[index], &heap->arr[parentIdx]);
        heapifyUp(heap, parentIdx);
    }
}

void heapifyDown(MinHeap* heap, int index) {
    int smallest = index;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    
    if (leftChild < heap->size && 
        heap->arr[leftChild].load < heap->arr[smallest].load) {
        smallest = leftChild;
    }
    
    if (rightChild < heap->size && 
        heap->arr[rightChild].load < heap->arr[smallest].load) {
        smallest = rightChild;
    }
    
    if (smallest != index) {
        swap(&heap->arr[index], &heap->arr[smallest]);
        heapifyDown(heap, smallest);
    }
}

void insertHeap(MinHeap* heap, int serverId, float load) {
    if (heap->size >= heap->capacity) {
        printf("❌ Heap capacity exceeded!\n");
        return;
    }
    
    heap->arr[heap->size].serverId = serverId;
    heap->arr[heap->size].load = load;
    
    heapifyUp(heap, heap->size);
    heap->size++;
}

HeapNode extractMin(MinHeap* heap) {
    HeapNode min = heap->arr[0];
    
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    
    if (heap->size > 0) {
        heapifyDown(heap, 0);
    }
    
    return min;
}

void updateHeap(MinHeap* heap, int serverId, float newLoad) {
    int index = -1;
    for (int i = 0; i < heap->size; i++) {
        if (heap->arr[i].serverId == serverId) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf("❌ Server %d not found in heap!\n", serverId);
        return;
    }
    
    heap->arr[index].load = newLoad;
    
    int parentIdx = (index - 1) / 2;
    if (index > 0 && heap->arr[index].load < heap->arr[parentIdx].load) {
        heapifyUp(heap, index);
    } else {
        heapifyDown(heap, index);
    }
}

void freeMinHeap(MinHeap* heap) {
    free(heap->arr);
    free(heap);
}

/* ============================================================================
 * LOAD BALANCING FUNCTIONS (UNCHANGED FROM ORIGINAL)
 * ============================================================================ */

float calculateAverageLoad(Server servers[], int numServers) {
    float totalLoad = 0.0;
    for (int i = 0; i < numServers; i++) {
        totalLoad += servers[i].currentLoad;
    }
    return totalLoad / numServers;
}

float getLoadPercentage(Server server) {
    return (server.currentLoad / server.capacity) * 100.0;
}

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

void rebalanceLoads(Server servers[], int numServers, float threshold, MinHeap* heap) {
    float avgLoad = calculateAverageLoad(servers, numServers);
    
    int mostLoadedIdx = findMostLoadedServer(servers, numServers);
    int leastLoadedIdx = findLeastLoadedServer(servers, numServers);
    
    float mostLoadedPercent = getLoadPercentage(servers[mostLoadedIdx]);
    float leastLoadedPercent = getLoadPercentage(servers[leastLoadedIdx]);
    
    float imbalance = mostLoadedPercent - leastLoadedPercent;
    
    if (imbalance > threshold) {
        float migrationAmount = (servers[mostLoadedIdx].currentLoad - avgLoad) * 0.5;
        
        printf("\n╔════════════════════════════════════╗\n");
        printf("║ ⚠️  REBALANCING TRIGGERED ⚠️       ║\n");
        printf("╚════════════════════════════════════╝\n");
        printf("   Imbalance: %.2f%% (threshold: %.2f%%)\n", imbalance, threshold);
        printf("   Server %d (%.2f%%) → Server %d (%.2f%%)\n",
               mostLoadedIdx, mostLoadedPercent,
               leastLoadedIdx, leastLoadedPercent);
        printf("   Migrating %.2f load units\n", migrationAmount);
        
        servers[mostLoadedIdx].currentLoad -= migrationAmount;
        servers[leastLoadedIdx].currentLoad += migrationAmount;
        
        updateHeap(heap, mostLoadedIdx, servers[mostLoadedIdx].currentLoad);
        updateHeap(heap, leastLoadedIdx, servers[leastLoadedIdx].currentLoad);
        
        printf("   ✓ Rebalancing complete\n\n");
    }
}

void printServerStates(Server servers[], int numServers) {
    printf("\n┌─── Current Server States ───┐\n");
    for (int i = 0; i < numServers; i++) {
        float percentage = getLoadPercentage(servers[i]);
        printf("│ Server %d: %6.2f/%6.2f (%.1f%%)\n",
               servers[i].id, servers[i].currentLoad, 
               servers[i].capacity, percentage);
    }
    
    float avgLoad = calculateAverageLoad(servers, numServers);
    printf("│ Average Load: %.2f\n", avgLoad);
    printf("└────────────────────────────────┘\n");
}

/* ============================================================================
 * INTERACTIVE INPUT FUNCTIONS
 * ============================================================================ */

void inputServerConfigurations(Server** servers, int* numServers, Graph** graph) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║     SERVER CONFIGURATION PHASE         ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    *numServers = getIntInput("Enter number of servers (1-20): ", 1, 20);
    
    *servers = (Server*)malloc(*numServers * sizeof(Server));
    *graph = createGraph(*numServers);
    
    printf("\n🔹 Server Capacity Configuration\n");
    printf("   Choose mode:\n");
    printf("   1. Auto-generate random capacities (80-120)\n");
    printf("   2. Enter each server capacity manually\n");
    
    int capacityMode = getIntInput("   Select mode (1-2): ", 1, 2);
    
    if (capacityMode == 1) {
        printf("\n✓ Generating random capacities...\n");
        for (int i = 0; i < *numServers; i++) {
            (*servers)[i].id = i;
            (*servers)[i].capacity = 80.0 + (float)rand() / RAND_MAX * 40.0;
            (*servers)[i].currentLoad = 0.0;
            printf("  Server %d: Capacity = %.2f\n", i, (*servers)[i].capacity);
        }
    } else {
        printf("\n✓ Enter capacity for each server:\n");
        for (int i = 0; i < *numServers; i++) {
            char prompt[100];
            sprintf(prompt, "  Server %d capacity (50-200): ", i);
            (*servers)[i].id = i;
            (*servers)[i].capacity = getFloatInput(prompt, 50.0, 200.0);
            (*servers)[i].currentLoad = 0.0;
        }
    }
}

void inputGraphTopology(Graph* graph, int numServers) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║    NETWORK TOPOLOGY CONFIGURATION      ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    char response = getYesNoInput("Auto-generate random network connections");
    
    if (response == 'Y') {
        printf("\n✓ Generating random network topology...\n");
        for (int i = 0; i < numServers; i++) {
            int neighbors = 1 + (rand() % 3);  // 1-3 connections per server
            for (int j = 0; j < neighbors; j++) {
                int dest = rand() % numServers;
                if (dest != i && !edgeExists(graph, i, dest)) {
                    addEdge(graph, i, dest);
                }
            }
        }
    } else {
        printf("\n✓ Manual edge entry mode\n");
        printf("   Enter edges in format: source destination\n");
        printf("   Example: 0 1 (connects Server 0 to Server 1)\n");
        printf("   Enter -1 when done\n\n");
        
        while (1) {
            printf("   Enter edge (or -1 to finish): ");
            int src, dest;
            
            if (scanf("%d %d", &src, &dest) != 2) {
                clearBuffer();
                printf("   ❌ Invalid input! Enter two numbers.\n");
                continue;
            }
            clearBuffer();
            
            if (src == -1) {
                break;
            }
            
            addEdge(graph, src, dest);
        }
    }
    
    printGraph(graph);
}

void inputTaskLoads(TaskData* taskData, int numTasks) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║       TASK LOAD CONFIGURATION          ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    taskData->numTasks = numTasks;
    taskData->taskLoads = (float*)malloc(numTasks * sizeof(float));
    
    printf("🔹 Task Load Generation Mode\n");
    printf("   1. Auto-generate random loads (5-15 units)\n");
    printf("   2. Enter each task load manually\n");
    
    int loadMode = getIntInput("   Select mode (1-2): ", 1, 2);
    
    if (loadMode == 1) {
        printf("\n✓ Generating random task loads...\n");
        for (int i = 0; i < numTasks; i++) {
            taskData->taskLoads[i] = 5.0 + (float)rand() / RAND_MAX * 10.0;
            printf("  Task %d: Load = %.2f\n", i + 1, taskData->taskLoads[i]);
        }
    } else {
        printf("\n✓ Enter load for each task:\n");
        for (int i = 0; i < numTasks; i++) {
            char prompt[100];
            sprintf(prompt, "  Task %d load (0.1-50): ", i + 1);
            taskData->taskLoads[i] = getFloatInput(prompt, 0.1, 50.0);
        }
    }
}

void inputRebalancingSettings(float* threshold, int* interval) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║    REBALANCING CONFIGURATION           ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    *threshold = getFloatInput("Rebalance threshold (0.1-100%%): ", 0.1, 100.0);
    *interval = getIntInput("Rebalance interval (every N tasks, 1-100): ", 1, 100);
    
    printf("\n✓ Rebalancing settings configured:\n");
    printf("  Threshold: %.2f%%\n", *threshold);
    printf("  Interval: Every %d tasks\n", *interval);
}

/* ============================================================================
 * SIMULATION SUMMARY & CONFIRMATION
 * ============================================================================ */

void printSimulationSummary(Server servers[], int numServers, Graph* graph,
                            int numTasks, float threshold, int interval) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║     SIMULATION CONFIGURATION SUMMARY    ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    printf("📊 SYSTEM CONFIGURATION\n");
    printf("   • Number of servers: %d\n", numServers);
    printf("   • Number of tasks: %d\n", numTasks);
    
    printf("\n📦 SERVER CAPACITIES\n");
    for (int i = 0; i < numServers; i++) {
        printf("   • Server %d: %.2f\n", i, servers[i].capacity);
    }
    
    printf("\n🌐 NETWORK TOPOLOGY\n");
    for (int i = 0; i < numServers; i++) {
        printf("   • Server %d → ", i);
        Node* current = graph->adjList[i];
        if (current == NULL) {
            printf("(isolated)\n");
        } else {
            while (current != NULL) {
                printf("%d", current->serverId);
                current = current->next;
                if (current != NULL) printf(", ");
            }
            printf("\n");
        }
    }
    
    printf("\n⚙️  REBALANCING SETTINGS\n");
    printf("   • Threshold: %.2f%%\n", threshold);
    printf("   • Interval: Every %d tasks\n\n", interval);
}

/* ============================================================================
 * INTERACTIVE SIMULATION
 * ============================================================================ */

void runInteractiveSimulation(Server servers[], Graph* graph, int numServers,
                              TaskData* taskData, float threshold, int interval) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║    INTERACTIVE SIMULATION MODE         ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    MinHeap* heap = createMinHeap(numServers);
    for (int i = 0; i < numServers; i++) {
        insertHeap(heap, i, 0.0);
    }
    
    int tasksAssigned = 0;
    int menuChoice;
    
    while (tasksAssigned < taskData->numTasks) {
        printf("\n┌─── Simulation Menu ───┐\n");
        printf("│ 1. Assign next task\n");
        printf("│ 2. Assign all remaining tasks\n");
        printf("│ 3. Trigger manual rebalance\n");
        printf("│ 4. Show server status\n");
        printf("│ 5. Exit simulation\n");
        printf("└───────────────────────┘\n");
        
        menuChoice = getIntInput("Choose action (1-5): ", 1, 5);
        
        switch (menuChoice) {
            case 1: {
                // Assign one task
                float taskLoad = taskData->taskLoads[tasksAssigned];
                HeapNode minServer = extractMin(heap);
                
                servers[minServer.serverId].currentLoad += taskLoad;
                float newLoad = servers[minServer.serverId].currentLoad;
                
                insertHeap(heap, minServer.serverId, newLoad);
                
                float percentage = getLoadPercentage(servers[minServer.serverId]);
                
                tasksAssigned++;
                printf("\n✓ Task %d assigned to Server %d\n", tasksAssigned, minServer.serverId);
                printf("  Load: %.2f/%.2f (%.1f%%)\n", newLoad,
                       servers[minServer.serverId].capacity, percentage);
                
                if (tasksAssigned % interval == 0) {
                    rebalanceLoads(servers, numServers, threshold, heap);
                }
                break;
            }
            
            case 2: {
                // Assign all remaining tasks
                printf("\n✓ Assigning all remaining %d tasks...\n", taskData->numTasks - tasksAssigned);
                
                while (tasksAssigned < taskData->numTasks) {
                    float taskLoad = taskData->taskLoads[tasksAssigned];
                    HeapNode minServer = extractMin(heap);
                    
                    servers[minServer.serverId].currentLoad += taskLoad;
                    float newLoad = servers[minServer.serverId].currentLoad;
                    
                    insertHeap(heap, minServer.serverId, newLoad);
                    
                    float percentage = getLoadPercentage(servers[minServer.serverId]);
                    
                    tasksAssigned++;
                    printf("  Task %2d → Server %d | Load: %6.2f/%6.2f (%.1f%%)\n",
                           tasksAssigned, minServer.serverId, newLoad,
                           servers[minServer.serverId].capacity, percentage);
                    
                    if (tasksAssigned % interval == 0) {
                        rebalanceLoads(servers, numServers, threshold, heap);
                    }
                }
                break;
            }
            
            case 3: {
                // Manual rebalance
                printf("\n✓ Triggering manual rebalance...\n");
                rebalanceLoads(servers, numServers, threshold, heap);
                break;
            }
            
            case 4: {
                // Show status
                printServerStates(servers, numServers);
                break;
            }
            
            case 5: {
                // Exit
                printf("\n⚠️  Exiting simulation. All remaining tasks will NOT be assigned.\n");
                tasksAssigned = taskData->numTasks;
                break;
            }
        }
    }
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║    FINAL LOAD DISTRIBUTION              ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    printServerStates(servers, numServers);
    
    float avgLoad = calculateAverageLoad(servers, numServers);
    float maxLoad = servers[findMostLoadedServer(servers, numServers)].currentLoad;
    float minLoad = servers[findLeastLoadedServer(servers, numServers)].currentLoad;
    float imbalance = maxLoad - minLoad;
    
    printf("\n📈 Final Statistics:\n");
    printf("   Average Load: %.2f\n", avgLoad);
    printf("   Max Load: %.2f | Min Load: %.2f\n", maxLoad, minLoad);
    printf("   Load Difference: %.2f\n", imbalance);
    
    if (imbalance < threshold) {
        printf("\n✓✓✓ System is WELL-BALANCED ✓✓✓\n");
    } else {
        printf("\n⚠️  System could benefit from further rebalancing\n");
    }
    
    freeMinHeap(heap);
}

/* ============================================================================
 * MAIN PROGRAM
 * ============================================================================ */

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║   DYNAMIC LOAD BALANCER - INTERACTIVE VERSION              ║\n");
    printf("║   Distributed System Simulation                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    srand((unsigned int)time(NULL));
    
    // Input Phase
    Server* servers = NULL;
    Graph* graph = NULL;
    int numServers = 0;
    
    inputServerConfigurations(&servers, &numServers, &graph);
    
    int numTasks = getIntInput("\nEnter number of tasks (1-1000): ", 1, 1000);
    
    TaskData taskData;
    inputTaskLoads(&taskData, numTasks);
    
    float rebalanceThreshold;
    int rebalanceInterval;
    inputRebalancingSettings(&rebalanceThreshold, &rebalanceInterval);
    
    inputGraphTopology(graph, numServers);
    
    // Summary and Confirmation
    printSimulationSummary(servers, numServers, graph, numTasks, 
                          rebalanceThreshold, rebalanceInterval);
    
    char confirmStart = getYesNoInput("Start simulation");
    
    if (confirmStart == 'Y') {
        // Run simulation
        runInteractiveSimulation(servers, graph, numServers, &taskData, 
                               rebalanceThreshold, rebalanceInterval);
    } else {
        printf("\n❌ Simulation cancelled by user.\n");
    }
    
    // Cleanup
    free(servers);
    free(taskData.taskLoads);
    freeGraph(graph);
    
    printf("\n✓ Resources freed. Program terminated.\n\n");
    
    return 0;
}

/*
 * ============================================================================
 * INTERACTIVE VERSION - KEY IMPROVEMENTS
 * ============================================================================
 * 
 * 1. USER INPUT FOR ALL CONFIGURATION
 *    - Number of servers
 *    - Server capacities (auto or manual)
 *    - Number of tasks
 *    - Task loads (auto or manual)
 *    - Rebalancing threshold & interval
 *    - Network topology (auto or manual)
 * 
 * 2. NETWORK TOPOLOGY CONFIGURATION
 *    - Auto-generate random connections
 *    - Manual edge entry with validation
 *    - Prevents self-edges and duplicates
 *    - Validates server ID ranges
 * 
 * 3. INPUT VALIDATION
 *    - Integer ranges (1-20 servers, 1-1000 tasks)
 *    - Float ranges (0.1-200 capacities, 0.1-100% threshold)
 *    - Yes/No prompts with error handling
 *    - Invalid input retry loops
 * 
 * 4. INTERACTIVE SIMULATION MENU
 *    - Assign one task at a time
 *    - Assign all remaining tasks
 *    - Manual rebalance trigger
 *    - View server status anytime
 *    - Exit gracefully
 * 
 * 5. IMPROVED FORMATTING
 *    - Box-drawing characters for sections
 *    - Clear status messages (✓, ❌, ⚠️)
 *    - Organized menu system
 *    - Real-time server load display
 * 
 * 6. MODULAR FUNCTIONS
 *    - inputServerConfigurations()
 *    - inputGraphTopology()
 *    - inputTaskLoads()
 *    - inputRebalancingSettings()
 *    - runInteractiveSimulation()
 *    - printSimulationSummary()
 * 
 * 7. CORE ALGORITHMS UNCHANGED
 *    - Heap operations: insertHeap, extractMin, heapifyUp/Down
 *    - Graph operations: createGraph, addEdge, printGraph
 *    - Load balancing: rebalanceLoads, calculateAverageLoad
 *    - Task assignment: Still uses min-heap for O(log n) selection
 * 
 * 8. USAGE FLOW
 *    1. Program starts with welcome banner
 *    2. Collect all configuration interactively
 *    3. Display summary and ask for confirmation
 *    4. Run simulation with step-by-step control
 *    5. Display final statistics
 *    6. Clean up resources
 * 
 * ============================================================================
 */
