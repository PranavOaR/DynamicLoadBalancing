# README — Dynamic Load Balancer in C

## PROJECT OVERVIEW

This project implements a **Dynamic Load Balancing Simulation** for a distributed system. The simulator manages multiple interconnected servers that process incoming tasks. The system dynamically assigns tasks to the least-loaded server and automatically rebalances loads when imbalances exceed a configurable threshold.

The key innovation is using a **Min-Heap data structure** to efficiently identify the least-loaded server in O(log n) time, combined with a graph representation of server connections and periodic rebalancing logic to maintain system equilibrium.

---

## DATA STRUCTURES

### 1. Server Structure
Represents a single server node in the distributed system
- **id**: Unique server identifier
- **capacity**: Maximum load the server can handle
- **currentLoad**: Current computational load on the server

### 2. Graph (Adjacency List)
Represents network topology connecting servers
- **Node**: Stores neighbor server ID and pointer to next neighbor
- **Graph**: Array of adjacency lists for each server
- Allows representation of complex network topologies

### 3. Min Heap
Priority queue for efficient server selection
- **HeapNode**: Stores server ID and its current load
- **MinHeap**: Implements a binary min-heap with dynamic operations
- Enables O(log n) extraction of least-loaded server

---

## ALGORITHM WORKFLOW

### INITIALIZATION PHASE

1. Create `NUM_SERVERS` server nodes with random capacities (80-120)
2. Initialize each server with zero load
3. Build network graph with random interconnections
4. Create min-heap and insert all servers (initially at load 0)

### TASK ASSIGNMENT PHASE

For each incoming task:
- a. Generate random task load (5-15 units)
- b. Extract minimum load server from heap (O(log n))
- c. Assign task to that server (update currentLoad)
- d. Reinsert server back into heap with new load (O(log n))
- e. Print assignment details

After every `REBALANCE_INTERVAL` tasks:
- a. Calculate system average load
- b. Identify most-loaded and least-loaded servers
- c. Check if imbalance exceeds threshold (20%)
- d. If threshold exceeded, migrate 50% of excess load
- e. Update heap with new loads

### TERMINATION

1. After `NUM_TASKS` assignments, simulation ends
2. Print final load distribution and statistics
3. Free all allocated memory

---

## FUNCTION REFERENCE

### GRAPH FUNCTIONS
| Function | Purpose |
|----------|---------|
| `createGraph(numServers)` | Create graph with n nodes |
| `addEdge(graph, src, dest)` | Add directed edge src→dest |
| `printGraph(graph)` | Display network topology |
| `freeGraph(graph)` | Deallocate graph memory |

### HEAP FUNCTIONS
| Function | Purpose |
|----------|---------|
| `createMinHeap(capacity)` | Create min-heap |
| `insertHeap(heap, id, load)` | Insert server into heap (O(log n)) |
| `extractMin(heap)` | Get minimum load server (O(log n)) |
| `heapifyUp(heap, index)` | Restore heap property upward |
| `heapifyDown(heap, index)` | Restore heap property downward |
| `updateHeap(heap, id, load)` | Update server load in heap |
| `freeMinHeap(heap)` | Deallocate heap memory |

### LOAD BALANCING FUNCTIONS
| Function | Purpose |
|----------|---------|
| `calculateAverageLoad()` | Compute mean system load |
| `getLoadPercentage(server)` | Calculate % of server capacity |
| `findMostLoadedServer()` | Identify highest load server |
| `findLeastLoadedServer()` | Identify lowest load server |
| `rebalanceLoads()` | Migrate loads if imbalanced |
| `printServerStates()` | Display all server statistics |

### SIMULATION
| Function | Purpose |
|----------|---------|
| `simulateTaskAssignment()` | Main task assignment loop |
| `main()` | Entry point and orchestrator |

---

## TIME COMPLEXITY ANALYSIS

| Operation | Complexity | Notes |
|-----------|-----------|-------|
| Insert into heap | O(log n) | Worst case for heapifyUp |
| Extract min from heap | O(log n) | Root removal + heapifyDown |
| Update heap entry | O(n) | Linear search + O(log n) reheapify |
| Calculate average load | O(n) | Sum all server loads |
| Find most/least loaded | O(n) | Linear scan required |
| Rebalance loads | O(n log n) | Find + update multiple servers |
| Single task assignment | O(log n) | Extract + Insert heap operations |
| N task assignments | O(n log n) | n × O(log n) per task |
| **Total simulation** | **O(n log n)** | Dominated by task assignment |

---

## CONFIGURATION PARAMETERS

| Parameter | Value | Description |
|-----------|-------|-------------|
| `NUM_SERVERS` | 6 | Number of servers in system |
| `NUM_TASKS` | 30 | Total tasks to simulate |
| `MIN_CAPACITY` / `MAX_CAPACITY` | 80-120 | Server capacity range |
| `MIN_TASK_LOAD` / `MAX_TASK_LOAD` | 5-15 | Task load range |
| `REBALANCE_THRESHOLD` | 20.0 | % imbalance before rebalancing |
| `REBALANCE_INTERVAL` | 5 | Rebalance after N tasks |

**Customization Tips:**
- Increase `NUM_SERVERS` for larger systems
- Decrease `REBALANCE_THRESHOLD` for aggressive balancing
- Modify task load ranges to simulate heavy/light workloads

---

## COMPILATION AND EXECUTION

### Compile:
```bash
gcc -o load_balancer load_balancer.c -lm
```

### Run:
```bash
./load_balancer
```

### Expected output includes:
- Server initialization with capacities
- Network topology diagram
- Task-by-task assignment with load percentages
- Rebalancing alerts when threshold is exceeded
- Final load distribution statistics
- System balance assessment

---

## EXAMPLE OUTPUT

```
╔════════════════════════════════════════════════════════════╗
║   DYNAMIC LOAD BALANCING SIMULATION - Distributed System   ║
╚════════════════════════════════════════════════════════════╝

✓ Initializing 6 servers...
  Server 0: Capacity = 95.42
  Server 1: Capacity = 102.15
  ...

--- Server Network Topology ---
Server 0 → 3 1 5
Server 1 → 4 2
...

--- Assigning 30 Tasks Dynamically ---
Task  1 → Server 2 | Load:      7.34/95.62 (7.7%)
Task  2 → Server 0 | Load:      8.92/88.15 (10.1%)
...

⚠️  REBALANCING TRIGGERED ⚠️
   Imbalance: 23.45% (threshold: 20.00%)
   Server 3 (28.5%) → Server 1 (5.2%)
   Migrating 8.75 load units
   ✓ Rebalancing complete

--- Current Server States ---
Server 0: Load =  45.32/ 88.15 (51.4%)
Server 1: Load =  49.87/102.15 (48.8%)
...

╔════════════════════════════════════════════════════════════╗
║                    FINAL LOAD DISTRIBUTION                 ║
╚════════════════════════════════════════════════════════════╝

--- Current Server States ---
Server 0: Load =  52.15/ 88.15 (59.2%)
Server 1: Load =  51.44/102.15 (50.4%)
...
Average Load:    50.87

--- Final Statistics ---
Average Load:    50.87
Max Load:        58.92
Min Load:        45.23
Load Difference: 13.69

✓✓✓ System is WELL-BALANCED ✓✓✓
```

---

## KEY FEATURES

### 1. EFFICIENT SERVER SELECTION
- Min-heap enables O(log n) lookup of least-loaded server
- Much better than O(n) linear search for large systems

### 2. DYNAMIC LOAD REBALANCING
- Automatic migration of load from overloaded to underloaded servers
- Configurable threshold prevents oscillation
- Maintains system equilibrium during operation

### 3. NETWORK TOPOLOGY AWARENESS
- Graph structure represents interconnections between servers
- Foundation for future enhancements (cost-aware routing, locality)

### 4. COMPREHENSIVE MONITORING
- Real-time display of task assignments and load percentages
- Detailed rebalancing alerts with migration amounts
- Final statistics for performance analysis

### 5. SINGLE FILE SIMPLICITY
- All functionality in one .c file
- Standard C libraries only (stdio, stdlib, time, math)
- No external dependencies or build complexity

---

## POSSIBLE EXTENSIONS

1. **Cost-Aware Load Migration**
   - Add network latency costs to edges
   - Prefer migrations between nearby servers

2. **Predictive Rebalancing**
   - Track load patterns over time
   - Predict future hotspots and preemptively rebalance

3. **Task Priority Queues**
   - Assign high-priority tasks to less-loaded servers
   - Support SLA-based scheduling

4. **Multi-Criteria Scheduling**
   - Consider CPU, memory, and network bandwidth
   - Weighted load metrics for heterogeneous systems

5. **Geographic Distribution**
   - Extend graph to include geographic locations
   - Implement geo-distributed rebalancing

6. **Fault Tolerance**
   - Simulate server failures and recovery
   - Automatic load migration on failure

7. **Interactive Shell**
   - Command-line interface for dynamic configuration
   - Query current system state
   - Manually trigger rebalancing or task injection

---

## NOTES

- All random values use time-seeded `rand()` for variability
- Simulation is deterministic given a fixed random seed
- Heap operations maintain O(log n) complexity throughout
- Memory is properly freed to prevent leaks
- Output includes visual indicators (✓, ⚠️, ✓✓✓) for clarity
- Percentages calculated as (load/capacity) × 100
- Rebalancing migrates 50% of excess load above average

---

## Author Notes

This project demonstrates fundamental concepts in distributed systems:
- **Priority Queues** for efficient resource selection
- **Dynamic Programming** through rebalancing algorithms
- **Graph Theory** for network topology representation
- **Simulation Techniques** for system behavior modeling

Perfect for educational purposes and as a foundation for more complex load balancing systems!
