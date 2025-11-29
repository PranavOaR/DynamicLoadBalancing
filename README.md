# Dynamic Load Balancer in C

**A production-quality C implementation of a dynamic load balancing system for distributed computing environments.**

Two versions available:
- **`load_balancer.c`** - Automated simulation with fixed configuration
- **`load_balancer_interactive.c`** - Fully interactive version with user-controlled parameters

---

## 📋 Table of Contents

1. [Program Flow Flowchart](#program-flow-flowchart)
2. [Project Overview](#project-overview)
3. [Architecture & Flowcharts](#architecture--flowcharts)
4. [Data Structures](#data-structures)
5. [Function Reference](#function-reference)
6. [Compilation & Usage](#compilation--usage)
7. [Algorithm Complexity](#algorithm-complexity)
8. [Examples](#examples)

---

## 🔄 Program Flow Flowchart

### Complete Program Execution Flowchart

```
                            ╔════════════════════╗
                            ║    PROGRAM START   ║
                            ╚════════════════════╝
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │  Print Welcome Banner │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Seed Random Generator │
                          │   srand(time(NULL))   │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │  Initialize Servers   │
                          │  (Random Capacities)  │
                          │   Loop: 0 to n-1      │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │   Create Graph        │
                          │  (Adjacency List)     │
                          │  Allocate memory      │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │  Build Network        │
                          │  (Random Edges)       │
                          │  addEdge() for each   │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Create Min-Heap       │
                          │ (Priority Queue)      │
                          │ Capacity = numServers │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │  Populate Min-Heap    │
                          │  insertHeap() all     │
                          │  servers with load=0  │
                          │  O(n log n)           │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Print Network Topology│
                          │ printGraph()          │
                          └───────────┬───────────┘
                                      │
                                      ▼
         ╔════════════════════════════════════════════════════╗
         ║  MAIN SIMULATION LOOP: Task Assignment            ║
         ║  FOR task = 1 TO numTasks                         ║
         ╚════════════════════════════════════════════════════╝
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Generate Random Task  │
                          │ Load: MIN to MAX      │
                          │ taskLoad = rand()%MAX │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │  Extract Min Server   │
                          │  from Heap            │
                          │  extractMin()         │
                          │  O(log n)             │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Add Load to Server    │
                          │ server.currentLoad += │
                          │ taskLoad              │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Reinsert Server to    │
                          │ Heap with New Load    │
                          │ insertHeap()          │
                          │ O(log n)              │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Print Assignment      │
                          │ Details:              │
                          │ - Server ID           │
                          │ - New Load            │
                          │ - Load Percentage     │
                          └───────────┬───────────┘
                                      │
                                      ▼
                    ┌──────────────────────────────────┐
                    │  Check Rebalancing Trigger       │
                    │  IF (task % REBALANCE_INTERVAL   │
                    │       == 0)                      │
                    └──────────┬───────────────────────┘
                               │
                    ┌──────────┴──────────┐
                    │                     │
                   YES                   NO
                    │                     │
                    ▼                     ▼
    ╔═══════════════════════╗      Continue Loop
    ║ REBALANCING PHASE     ║      │
    ║ rebalanceLoads()      ║      │
    ╚═══════════════════════╝      │
            │                      │
            ▼                      │
    ┌──────────────────────┐       │
    │ Calculate Average    │       │
    │ Load O(n)            │       │
    └──────────┬───────────┘       │
               │                   │
               ▼                   │
    ┌──────────────────────┐       │
    │ Find Most-Loaded     │       │
    │ Server O(n)          │       │
    └──────────┬───────────┘       │
               │                   │
               ▼                   │
    ┌──────────────────────┐       │
    │ Find Least-Loaded    │       │
    │ Server O(n)          │       │
    └──────────┬───────────┘       │
               │                   │
               ▼                   │
    ┌──────────────────────┐       │
    │ Calculate Imbalance  │       │
    │ Percentage           │       │
    └──────────┬───────────┘       │
               │                   │
               ▼                   │
    ┌─────────────────────────────┐│
    │ Imbalance > Threshold?      ││
    └──────────┬────────────────┬─┘│
               │                │  │
             YES              NO   │
               │                │  │
               ▼                ▼  │
    ┌──────────────────┐  Skip  │
    │ Perform Load     │  Rebal │
    │ Migration:       │  ___   │
    │ • Calculate 50%  │        │
    │   of excess load │        │
    │ • Update most-   │        │
    │   loaded server  │        │
    │ • Update least-  │        │
    │   loaded server  │        │
    │ • Update both in │        │
    │   heap O(2log n) │        │
    │ • Log action     │        │
    └──────────┬───────┘        │
               │                │
               ▼                ▼
    ┌──────────────────────────────┐
    │ End Rebalancing              │
    └──────────┬───────────────────┘
               │
               └──────────┬──────────┐
                          │          │
                          ▼          │
                  ┌──────────────┐   │
                  │  task < NUM_ │   │
                  │  TASKS?      │   │
                  └──────┬───┬──┘    │
                        YES NO       │
                         │   │       │
                         │   └───────┤
                         │           │
                         ▼           ▼
                  Continue      End Loop
                  
         ╔════════════════════════════════════════════════════╗
         ║  ALL TASKS ASSIGNED - DISPLAY FINAL STATISTICS    ║
         ╚════════════════════════════════════════════════════╝
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Print Final Server    │
                          │ States                │
                          │ printServerStates()   │
                          │ O(n)                  │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Calculate Final       │
                          │ Statistics:           │
                          │ • Average Load        │
                          │ • Max Load            │
                          │ • Min Load            │
                          │ • Load Difference     │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Determine Balance     │
                          │ Status:               │
                          │ • Difference < 10%    │
                          │   → WELL-BALANCED     │
                          │ • Difference >= 10%   │
                          │   → NEEDS ATTENTION   │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Print Final Stats Box │
                          │ with Status           │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Free Min-Heap         │
                          │ freeMinHeap()         │
                          │ O(1)                  │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Free Graph            │
                          │ freeGraph()           │
                          │ O(V+E)                │
                          └───────────┬───────────┘
                                      │
                                      ▼
                          ┌───────────────────────┐
                          │ Print Completion      │
                          │ Message               │
                          └───────────┬───────────┘
                                      │
                                      ▼
                            ╔════════════════════╗
                            ║   PROGRAM END      ║
                            ║   return 0         ║
                            ╚════════════════════╝
```

### Key Decision Points

| Decision | Condition | True Path | False Path |
|----------|-----------|-----------|------------|
| Rebalancing Trigger | `task % interval == 0` | Execute rebalancing | Continue to next task |
| Imbalance Check | `imbalance > threshold` | Migrate load | Skip rebalancing |
| Balance Status | `diff < 10%` | WELL-BALANCED | NEEDS ATTENTION |

### Complexity Summary for Main Loop

```
Per Task Assignment:
  • extractMin(): O(log n)          [Heapify down]
  • insertHeap(): O(log n)          [Heapify up]
  • Total per task: O(log n)

Periodic Rebalancing (every k tasks):
  • Find min: O(n)
  • Find max: O(n)
  • Update heap: O(2 log n)
  • Total per rebalance: O(n)

Total for n tasks:
  • Assignments: O(n log n)
  • Rebalances: O((n/k) × n) = O(n²/k)
  • Overall: O(n log n + n²/k)
  • With k=5: O(n log n) dominates for large n
```

---

## 🎯 Project Overview

This project simulates a **distributed system with multiple interconnected servers** that dynamically balance computational tasks. The system:

✅ Uses a **min-heap** for O(log n) least-loaded server selection  
✅ Maintains **network topology** with graph adjacency lists  
✅ Automatically **rebalances loads** when imbalance exceeds threshold  
✅ Provides **real-time monitoring** of server states  
✅ Validates all **user inputs** in interactive mode  

### Real-World Applications
- Cloud platforms (AWS, Azure, GCP)
- Kubernetes pod scheduling
- Message queue task routing
- Database query distribution
- Microservices load balancing

---

## 🏗️ Architecture & Flowcharts

### System Architecture Overview

```
┌─────────────────────────────────────────────────────┐
│         Load Balancing System                       │
├─────────────────────────────────────────────────────┤
│                                                      │
│  ┌──────────────────────────────────────────────┐  │
│  │    Min-Heap Priority Queue                   │  │
│  │  (O(log n) least-loaded selection)          │  │
│  │  ┌────────┬────────┬──────────────────────┐ │  │
│  │  │ Server │ Server │     ...              │ │  │
│  │  │ Load   │ Load   │  (sorted by load)    │ │  │
│  │  └────────┴────────┴──────────────────────┘ │  │
│  └──────────────────────────────────────────────┘  │
│                    ↕                               │
│  ┌──────────────────────────────────────────────┐  │
│  │    Server Network Graph                      │  │
│  │  (Adjacency List Topology)                  │  │
│  │  Server 0 → [1, 3, 5]                      │  │
│  │  Server 1 → [0, 2]                         │  │
│  │  Server 2 → [1, 4]                         │  │
│  └──────────────────────────────────────────────┘  │
│                    ↑                               │
│  ┌──────────────────────────────────────────────┐  │
│  │  Dynamic Rebalancing Engine                  │  │
│  │  • Calculate imbalance                      │  │
│  │  • Migrate load if threshold exceeded       │  │
│  │  • Update heap with new loads               │  │
│  └──────────────────────────────────────────────┘  │
│                                                      │
└─────────────────────────────────────────────────────┘
```

### Main Execution Flow

```
START
  │
  ├─→ Initialize System
  │    ├─ Create servers with capacities
  │    ├─ Build network graph
  │    └─ Create min-heap
  │
  ├─→ Task Assignment Loop (for each task)
  │    ├─ Extract min-load server from heap      [O(log n)]
  │    ├─ Assign task to server
  │    ├─ Update server load
  │    └─ Reinsert server to heap                 [O(log n)]
  │
  ├─→ Periodic Rebalancing (every N tasks)
  │    ├─ Calculate average load
  │    ├─ Check imbalance > threshold?
  │    ├─ If YES:
  │    │  ├─ Find most-loaded server
  │    │  ├─ Find least-loaded server
  │    │  ├─ Migrate 50% excess load
  │    │  └─ Update heap
  │    └─ If NO: continue
  │
  ├─→ Display Results
  │    ├─ Final server states
  │    ├─ Load statistics
  │    └─ Balance assessment
  │
  ├─→ Cleanup Resources
  │    └─ Free heap, graph, servers
  │
  └─→ END
```

### Task Assignment Decision Tree

```
                    NEW TASK ARRIVES
                           │
                           ▼
                    ┌───────────────┐
                    │  Extract Min  │
                    │    Server     │
                    │  from Heap    │
                    │  O(log n)     │
                    └───────┬───────┘
                            │
                            ▼
                    ┌───────────────────┐
                    │ Add Task Load to  │
                    │ Server's Current  │
                    │      Load         │
                    └───────┬───────────┘
                            │
                            ▼
                    ┌───────────────────┐
                    │ Reinsert Server   │
                    │   Back to Heap    │
                    │  O(log n)         │
                    └───────┬───────────┘
                            │
                            ▼
                    TASK ASSIGNED ✓
                            │
                            ▼
                    Task Count++
                    │
        ┌───────────┴───────────┐
        │                       │
        ▼                       ▼
   Count % Interval == 0?   NO
        │
        YES
        │
        ▼
   TRIGGER REBALANCING
```

### Rebalancing Logic Flowchart

```
                 CHECK REBALANCING
                        │
                        ▼
            ┌──────────────────────────┐
            │  Calculate Average Load  │
            │  O(n)                    │
            └──────────┬───────────────┘
                       │
                       ▼
            ┌──────────────────────────┐
            │  Find Most-Loaded Server │
            │  Find Least-Loaded       │
            │  O(n) each               │
            └──────────┬───────────────┘
                       │
                       ▼
            ┌──────────────────────────┐
            │  Calculate Imbalance %   │
            │  mostLoad% - leastLoad%  │
            └──────────┬───────────────┘
                       │
                       ▼
            ┌──────────────────────────┐
            │  Imbalance > Threshold?  │
            └──────────┬───────────────┘
                    ┌──┴──┐
                    │     │
                  YES    NO
                    │     │
                    ▼     ▼
              REBALANCE  SKIP
                    │     │
                    ▼     ▼
         ┌─────────────────────┐
         │ Migrate Load:       │
         │ • Calculate amount  │
         │ • Update both loads │
         │ • Update heap       │
         │ • Log action        │
         └─────────────────────┘
```

### Heap Operations - Insert Process

```
           INSERT SERVER INTO HEAP
                     │
                     ▼
         ┌────────────────────────┐
         │ Add to end of array    │
         │ heap[size] = new node  │
         │ size++                 │
         └────────────┬───────────┘
                      │
                      ▼
         ┌────────────────────────┐
         │  HEAPIFY UP            │
         │  While index > 0:      │
         │  ├─ parent = (i-1)/2   │
         │  ├─ if load < parent   │
         │  │  ├─ SWAP            │
         │  │  └─ i = parent      │
         │  └─ else BREAK         │
         └────────────┬───────────┘
                      │
                      ▼
         MIN-HEAP PROPERTY RESTORED ✓
```

### Heap Operations - Extract Min Process

```
           EXTRACT MIN FROM HEAP
                     │
                     ▼
         ┌────────────────────────┐
         │ Save min = heap[0]     │
         │ (root node)            │
         └────────────┬───────────┘
                      │
                      ▼
         ┌────────────────────────┐
         │ Move last to root:     │
         │ heap[0] = heap[size-1] │
         │ size--                 │
         └────────────┬───────────┘
                      │
                      ▼
         ┌────────────────────────┐
         │  HEAPIFY DOWN          │
         │  From index 0:         │
         │  ├─ left = 2*i + 1     │
         │  ├─ right = 2*i + 2    │
         │  ├─ smallest = min()   │
         │  ├─ SWAP if needed      │
         │  └─ i = smallest       │
         └────────────┬───────────┘
                      │
                      ▼
         MIN-HEAP PROPERTY RESTORED ✓
         RETURN min node
```

---

## 📊 Data Structures

### Server Structure
```c
typedef struct {
    int id;              // Unique server identifier (0 to n-1)
    float capacity;      // Maximum load capacity
    float currentLoad;   // Current load on server
} Server;
```

### Graph Node (Adjacency List)
```c
typedef struct Node {
    int serverId;        // Connected server ID
    struct Node* next;   // Pointer to next neighbor
} Node;

typedef struct {
    int numServers;      // Total servers in system
    Node** adjList;      // Array of adjacency lists
} Graph;
```

### Min-Heap (Priority Queue)
```c
typedef struct {
    int serverId;        // Server ID in heap
    float load;          // Current load value
} HeapNode;

typedef struct {
    HeapNode* arr;       // Array of heap nodes
    int size;            // Current number of nodes
    int capacity;        // Maximum capacity
} MinHeap;
```

---

## 🔧 Function Reference

### GRAPH FUNCTIONS

#### `Graph* createGraph(int numServers)`
**Purpose**: Creates a new graph with numServers nodes

**Parameters**:
- `numServers` (int): Number of server nodes (1-20)

**Returns**: Graph* - Pointer to allocated Graph structure

**Algorithm**:
1. Allocate Graph struct on heap
2. Set numServers field
3. Allocate array of Node* pointers
4. Initialize all pointers to NULL
5. Return Graph pointer

**Time Complexity**: O(n)  
**Space Complexity**: O(n)

**Example**:
```c
Graph* network = createGraph(6);
// Creates graph with 6 isolated servers
```

---

#### `int addEdge(Graph* graph, int src, int dest)`
**Purpose**: Adds directed edge from src to dest with validation

**Parameters**:
- `graph` (Graph*): Network graph
- `src` (int): Source server ID
- `dest` (int): Destination server ID

**Returns**: int - 1 if success, 0 if validation failed

**Validation Checks**:
- src and dest in valid range [0, numServers-1]
- src ≠ dest (no self-edges)
- Edge doesn't already exist

**Algorithm**:
1. Validate edge parameters
2. Check if edge already exists
3. Create new Node with dest
4. Insert at front of adjacency list
5. Return success/failure status

**Time Complexity**: O(degree)  
**Space Complexity**: O(1)

**Example**:
```c
addEdge(graph, 0, 2);  // Server 0 → Server 2
```

---

#### `void printGraph(Graph* graph)`
**Purpose**: Displays network topology in readable format

**Parameters**:
- `graph` (Graph*): Graph to display

**Algorithm**:
1. Print header
2. For each server: print neighbors
3. If no neighbors: print "(no connections)"
4. Print footer

**Time Complexity**: O(V + E)

---

#### `void freeGraph(Graph* graph)`
**Purpose**: Deallocates all graph memory

**Parameters**:
- `graph` (Graph*): Graph to free

**Algorithm**:
1. For each adjacency list: free all Node structs
2. Free adjacency list array
3. Free Graph struct

**Time Complexity**: O(V + E)

---

### MIN-HEAP FUNCTIONS

#### `MinHeap* createMinHeap(int capacity)`
**Purpose**: Creates a min-heap with given capacity

**Parameters**:
- `capacity` (int): Maximum number of servers

**Returns**: MinHeap* - Pointer to allocated heap

**Algorithm**:
1. Allocate MinHeap struct
2. Allocate HeapNode array
3. Initialize size to 0
4. Set capacity

**Time Complexity**: O(n)

---

#### `void insertHeap(MinHeap* heap, int serverId, float load)`
**Purpose**: Inserts server into min-heap

**Parameters**:
- `heap` (MinHeap*): Heap to insert into
- `serverId` (int): Server ID to insert
- `load` (float): Server's current load

**Algorithm**:
1. Add new node at end of array
2. Call heapifyUp from that position
3. Increment size

**Time Complexity**: O(log n)

```
     1
    / \
   2   3  ← New node added here
  /
 4
```

---

#### `HeapNode extractMin(MinHeap* heap)`
**Purpose**: Extracts and returns minimum load server

**Returns**: HeapNode - Server with minimum load

**Algorithm**:
1. Save root node (minimum)
2. Move last node to root
3. Decrement size
4. Call heapifyDown from root
5. Return saved minimum

**Time Complexity**: O(log n)

```
Before:          After:
    1                3
   / \              /
  2   3     ──→    2
 /
4
```

---

#### `void heapifyUp(MinHeap* heap, int index)`
**Purpose**: Restores heap property by moving node up

**Parameters**:
- `heap` (MinHeap*): Heap to reheapify
- `index` (int): Current index

**Algorithm**:
1. Calculate parent index: (index - 1) / 2
2. If child < parent: swap them
3. Recursively heapifyUp from parent
4. Else: stop (heap property restored)

**Time Complexity**: O(log n)

---

#### `void heapifyDown(MinHeap* heap, int index)`
**Purpose**: Restores heap property by moving node down

**Parameters**:
- `heap` (MinHeap*): Heap to reheapify
- `index` (int): Current index

**Algorithm**:
1. Calculate children indices: 2*i+1, 2*i+2
2. Find smallest among node and children
3. If smallest ≠ current: swap and heapifyDown
4. Else: stop (heap property restored)

**Time Complexity**: O(log n)

---

#### `void updateHeap(MinHeap* heap, int serverId, float newLoad)`
**Purpose**: Updates server's load in heap and reheapifies

**Parameters**:
- `heap` (MinHeap*): Heap containing server
- `serverId` (int): Server to update
- `newLoad` (float): New load value

**Algorithm**:
1. Search for serverId in heap (O(n))
2. Update load value
3. If load decreased: heapifyUp
4. If load increased: heapifyDown

**Time Complexity**: O(n)

---

#### `void freeMinHeap(MinHeap* heap)`
**Purpose**: Deallocates heap memory

**Algorithm**:
1. Free HeapNode array
2. Free MinHeap struct

**Time Complexity**: O(1)

---

### LOAD BALANCING FUNCTIONS

#### `float calculateAverageLoad(Server servers[], int numServers)`
**Purpose**: Calculates mean load across all servers

**Parameters**:
- `servers[]` (Server[]): Array of servers
- `numServers` (int): Number of servers

**Returns**: float - Average load

**Algorithm**:
1. Sum all currentLoad values
2. Divide by numServers
3. Return average

**Time Complexity**: O(n)

**Example**:
```
Loads: [50, 45, 55, 48, 52]
Average = (50+45+55+48+52) / 5 = 50.0
```

---

#### `float getLoadPercentage(Server server)`
**Purpose**: Calculates server's load as percentage of capacity

**Parameters**:
- `server` (Server): Server to check

**Returns**: float - Percentage (0-100)

**Formula**: `(currentLoad / capacity) * 100.0`

**Time Complexity**: O(1)

---

#### `int findMostLoadedServer(Server servers[], int numServers)`
**Purpose**: Finds server with highest load

**Returns**: int - Index of most-loaded server

**Algorithm**:
1. Initialize max = servers[0].currentLoad
2. Iterate through all servers
3. Track index with highest load
4. Return that index

**Time Complexity**: O(n)

---

#### `int findLeastLoadedServer(Server servers[], int numServers)`
**Purpose**: Finds server with lowest load

**Returns**: int - Index of least-loaded server

**Algorithm**:
1. Initialize min = servers[0].currentLoad
2. Iterate through all servers
3. Track index with lowest load
4. Return that index

**Time Complexity**: O(n)

---

#### `void rebalanceLoads(Server servers[], int numServers, float threshold, MinHeap* heap)`
**Purpose**: Rebalances loads if imbalance exceeds threshold

**Parameters**:
- `servers[]` (Server[]): Server array
- `numServers` (int): Number of servers
- `threshold` (float): Imbalance threshold %
- `heap` (MinHeap*): Min-heap to update

**Algorithm**:
1. Calculate average load
2. Find most-loaded and least-loaded servers
3. Calculate imbalance percentage
4. If imbalance > threshold:
   - Calculate migration amount (50% of excess)
   - Decrease most-loaded server's load
   - Increase least-loaded server's load
   - Update both in heap
   - Log rebalancing event
5. Else: skip rebalancing

**Time Complexity**: O(n log n)

**Example**:
```
Before: [50, 45, 65, 48, 52]  (Imbalance: 20%)
Avg = 52
Migrate 50% of excess from Server 2 to Server 1
After: [50, 55, 55, 48, 52]   (Imbalance: 7%)
```

---

#### `void printServerStates(Server servers[], int numServers)`
**Purpose**: Displays current load state of all servers

**Algorithm**:
1. Print header box
2. For each server: print load and percentage
3. Calculate and print average load
4. Print footer

**Time Complexity**: O(n)

---

### SIMULATION FUNCTIONS

#### `void simulateTaskAssignment(Server servers[], Graph* graph, MinHeap* heap, int numTasks)`
**Purpose**: Main task assignment loop with periodic rebalancing

**Parameters**:
- `servers[]` (Server[]): Array of servers
- `graph` (Graph*): Network topology (informational)
- `heap` (MinHeap*): Min-heap for server selection
- `numTasks` (int): Number of tasks to assign

**Algorithm**:
```
FOR each task (1 to numTasks):
   1. Generate random task load
   2. Extract minimum-load server from heap  [O(log n)]
   3. Add task load to server's currentLoad
   4. Reinsert server into heap              [O(log n)]
   5. Print assignment details
   
   IF (task % REBALANCE_INTERVAL == 0):
      6. Call rebalanceLoads()
```

**Time Complexity**: O(n log n) for n tasks

---

#### `int main()`
**Purpose**: Program entry point and orchestrator

**Algorithm**:
1. Print welcome banner
2. Seed random number generator
3. Initialize servers with random capacities
4. Create network graph with random connections
5. Create and populate min-heap
6. Call simulateTaskAssignment()
7. Display final statistics
8. Free all resources
9. Print completion message

**Time Complexity**: O(n log n)

---

## 💻 Compilation & Usage

### Compile Automated Version
```bash
gcc -o load_balancer load_balancer.c -lm
```

### Run Automated Version
```bash
./load_balancer
```

### Compile Interactive Version
```bash
gcc -o load_balancer_interactive load_balancer_interactive.c -lm
```

### Run Interactive Version
```bash
./load_balancer_interactive
```

### Flags Explained
- `-o load_balancer` - Output executable name
- `-lm` - Link math library (for floating-point operations)

---

## ⏱️ Algorithm Complexity

| Operation | Time | Space | Notes |
|-----------|------|-------|-------|
| Create Heap | O(n) | O(n) | n = numServers |
| Insert into Heap | O(log n) | O(1) | Heapify up |
| Extract Min | O(log n) | O(1) | Heapify down |
| Update Heap | O(n) | O(1) | Linear search + reheapify |
| Avg Load Calc | O(n) | O(1) | Sum all loads |
| Find Max/Min | O(n) | O(1) | Linear scan |
| Single Rebalance | O(n) | O(1) | Find servers + 2 heap updates |
| Single Task | O(log n) | O(1) | Extract + Insert |
| n Tasks Total | O(n log n) | O(n) | Without rebalancing |
| Graph Traversal | O(V+E) | O(1) | V=servers, E=edges |

### Performance Gains

**Why Min-Heap?**
- Without Heap: O(n) per task → O(n²) total for n tasks
- With Heap: O(log n) per task → O(n log n) total for n tasks
- **For 1000 servers, 1000 tasks**: 100× faster! ⚡

---

## 📈 Example Output

```
╔════════════════════════════════════════════════════════════╗
║   DYNAMIC LOAD BALANCING SIMULATION - Distributed System   ║
╚════════════════════════════════════════════════════════════╝

✓ Initializing 6 servers...
  Server 0: Capacity = 98.53
  Server 1: Capacity = 112.49
  Server 2: Capacity = 117.72
  Server 3: Capacity = 118.88
  Server 4: Capacity = 86.14
  Server 5: Capacity = 82.11

--- Server Network Topology ---
Server 0 → 5
Server 1 → 2 4
...

--- Assigning 30 Tasks Dynamically ---
Task  1 → Server 0 | Load:  12.19/ 98.53 (12.4%)
Task  2 → Server 5 | Load:   5.14/ 82.11 (6.3%)
...
Task 30 → Server 4 | Load:  58.10/ 86.14 (67.4%)

⚠️  REBALANCING TRIGGERED ⚠️
   Imbalance: 26.06% (threshold: 20.00%)
   Server 4 (67.45%) → Server 1 (41.38%)
   Migrating 3.62 load units
   ✓ Rebalancing complete

╔════════════════════════════════════════════════════════════╗
║                    FINAL LOAD DISTRIBUTION                 ║
╚════════════════════════════════════════════════════════════╝

--- Current Server States ---
Server 0: Load =  50.16/ 98.53 (50.9%)
Server 1: Load =  50.17/112.49 (44.6%)
...
Average Load: 50.86

--- Final Statistics ---
Average Load:    50.86
Max Load:        54.48
Min Load:        48.00
Load Difference: 6.48

✓✓✓ System is WELL-BALANCED ✓✓✓
```

---

## 🎓 Learning Outcomes

This project teaches:

✅ **Data Structures**: Heaps, graphs, linked lists  
✅ **Algorithms**: Min-heap operations, graph traversal, rebalancing  
✅ **Complexity Analysis**: Time and space complexity  
✅ **System Design**: Load balancing principles  
✅ **C Programming**: Memory management, modular design  
✅ **Distributed Systems**: Task scheduling, resource allocation  

---

## 📁 Project Files

| File | Purpose | Lines |
|------|---------|-------|
| `load_balancer.c` | Automated simulation | ~700 |
| `load_balancer_interactive.c` | Interactive version | ~1100 |
| `FUNCTION_DOCUMENTATION.txt` | Detailed function specs | ~500 |
| `INTERACTIVE_FUNCTION_DOCUMENTATION.txt` | Interactive functions | ~800 |
| `README.md` | This file | ~600 |

---

## 🚀 Repository

**GitHub**: [https://github.com/PranavOaR/DynamicLoadBalancing](https://github.com/PranavOaR/DynamicLoadBalancing)

---

**Happy Load Balancing! 🎉**
