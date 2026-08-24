# Persistent Priority Queue

A persistent priority queue implemented in **C++17** using a **binary min-heap**, an **ID-to-index hash map**, and **file-based persistence**.

This project was developed as an SDE assessment to demonstrate understanding of data structures, algorithms, object-oriented design, persistence, error handling, and software organization.

---

# 1. How to Run

## Requirements

- C++17-compatible compiler
- Standard C++ library
- Git (only required if cloning the repository)

No external database or third-party library is required.

## Clone the Repository

```bash
git clone <YOUR_GITHUB_REPOSITORY_URL>
cd persistent-priority-queue
```

> Replace `<YOUR_GITHUB_REPOSITORY_URL>` with the actual repository URL after publishing the project.

## Compile

```bash
g++ -std=c++17 main.cpp -o app.exe
```

## Run

### MSYS2 / Git Bash / Linux

```bash
./app.exe
```

### Windows PowerShell

```powershell
.\app.exe
```

The persistent queue data is stored in:

```text
data/queue.dat
```

---

# 2. Assessment Overview

The objective of this project is to implement a **Persistent Priority Queue**.

A normal priority queue keeps its state only in memory. In this implementation, the queue state is also persisted to a local file so that the data can be restored when the application starts again.

Each queue item contains three properties:

```text
ID        → Unique integer identifier
Value     → String containing the task or data
Priority  → Integer used to determine priority
```

Example:

```text
ID: 101
Value: Fix login bug
Priority: 1
```

Lower priority values are treated as higher priority for `extract_min()`.

---

# 3. Required Operations

The implementation supports all operations specified in the assessment:

| Operation | Description |
|---|---|
| `insert` | Adds a new item to the queue |
| `peek` | Returns the minimum-priority item without removing it |
| `extract_min` | Removes and returns the minimum-priority item |
| `extract_max` | Removes and returns the maximum-priority item |
| `update` | Changes the priority of an existing item |
| `delete` | Removes an item using its ID |
| `is_empty` | Checks whether the queue contains any items |

---

# 4. Project Structure

```text
persistent-priority-queue/
│
├── README.md
├── module.cpp
├── main.cpp
└── data/
    └── queue.dat
```

## File Responsibilities

### `module.cpp`

Contains the main implementation of the persistent priority queue.

It includes:

- Queue item definition
- Binary min-heap
- ID-to-index mapping
- Heap operations
- Insert, update, and delete operations
- Minimum and maximum extraction
- File loading and saving
- Error handling

### `main.cpp`

A demonstration program showing all required priority queue operations.

### `README.md`

Project documentation, design decisions, setup instructions, workflow, and complexity analysis.

### `data/queue.dat`

Stores the persistent state of the queue.

---

# 5. System Design

The implementation uses two main in-memory data structures:

```text
                    PriorityQueue
                         │
              ┌──────────┴──────────┐
              │                     │
              ▼                     ▼
        Binary Min-Heap       ID → Index Map
              │                     │
              └──────────┬──────────┘
                         │
                         ▼
                  File Persistence
                         │
                         ▼
                   queue.dat
```

## Binary Min-Heap

The binary min-heap maintains the priority ordering.

The item with the smallest priority is maintained at the root.

This provides efficient:

- Minimum lookup
- Insertion
- Minimum extraction

## ID-to-Index Map

An `unordered_map` maintains the relationship:

```text
ID → Current Heap Index
```

For example:

```text
ID 101 → Heap index 4
ID 205 → Heap index 1
ID 309 → Heap index 6
```

This allows an item to be located efficiently when performing:

- `update`
- `delete`

The mapping is updated whenever heap elements are swapped.

---

# 6. Application Workflow

The overall workflow is:

```text
Application Starts
       │
       ▼
PriorityQueue Created
       │
       ▼
Load queue.dat
       │
       ▼
Rebuild Heap + ID/Index Map
       │
       ▼
Perform Queue Operations
       │
       ▼
Modify Queue?
       │
       ▼
Update In-Memory Structures
       │
       ▼
saveToFile()
       │
       ▼
Update queue.dat
```

The queue therefore maintains both:

```text
In-Memory State
       +
Persistent File State
```

---

# 7. How Persistence Works

Persistence means that the queue state is not lost when the application terminates.

When a `PriorityQueue` object is created, the implementation attempts to load existing data from:

```text
data/queue.dat
```

Each record is stored using the format:

```text
id|value|priority
```

Example:

```text
4|Send email|3
```

represents:

```text
ID       = 4
Value    = Send email
Priority = 3
```

Whenever a modifying operation is performed, the updated queue state is written back to the file.

For example:

```text
insert
   │
   ▼
Update Heap
   │
   ▼
Update ID/Index Map
   │
   ▼
saveToFile()
   │
   ▼
queue.dat Updated
```

The same persistence process is used for:

- `insert`
- `extract_min`
- `extract_max`
- `update`
- `delete`

When the application starts again, the file is read and the in-memory priority queue is reconstructed.

---

# 8. Extract Max Design

A binary min-heap is optimized for finding the minimum element, not the maximum.

Instead of implementing a more complex min-max heap, this project uses the property that all maximum candidates in a min-heap are located among the leaf nodes.

Therefore, `extract_max()`:

1. Identifies the leaf nodes.
2. Scans the leaf nodes to find the maximum-priority item.
3. Removes that item from the heap.
4. Restores the heap property.
5. Saves the updated state to disk.

This results in:

```text
extract_max → O(n)
```

This is a deliberate trade-off to keep the implementation simpler while maintaining efficient primary min-heap operations.

---

# 9. Time Complexity

| Operation | Time Complexity |
|---|---:|
| `insert` | O(log n) |
| `peek` | O(1) |
| `extract_min` | O(log n) |
| `extract_max` | O(n) |
| `update` | O(log n) |
| `delete` | O(log n) |
| `is_empty` | O(1) |

The ID-to-index hash map provides **average O(1)** lookup for `update` and `delete`.

File persistence adds disk I/O to modifying operations.

---

# 10. Why C++ and File-Based Persistence?

C++ with file-based persistence was selected because this assessment is primarily focused on implementing a data structure with persistent state.

For a lightweight standalone application, local file storage avoids introducing additional infrastructure such as:

- Database server setup
- Database connection management
- SQL queries
- Database schema configuration
- External service dependencies

This keeps the implementation focused on the priority queue itself while still satisfying the persistence requirement.

C++ was selected because the assignment strongly focuses on **Data Structures, Algorithms, and Object-Oriented System Design**. C++ provides a direct and efficient way to implement the binary heap and supporting data structures.

For a larger production system involving multiple users, concurrent access, transactions, complex querying, centralized storage, or distributed services, a relational database such as PostgreSQL would be more appropriate.

Therefore, the choice of C++ with file-based persistence is a deliberate engineering trade-off based on the scope and requirements of this assignment.

---

# 11. Why Binary Min-Heap Instead of Min-Max Heap?

A binary min-heap provides efficient insertion, minimum lookup, and minimum extraction while keeping the implementation relatively simple.

A min-max heap could provide more efficient access to both minimum and maximum elements, but it would add additional implementation complexity.

For this project, a binary min-heap combined with scanning the leaf nodes for `extract_max()` provides a simpler and maintainable solution while keeping the core priority queue operations efficient.

---

# 12. Error Handling

The implementation handles invalid operations using C++ exceptions.

Examples include:

- Attempting to insert a duplicate ID
- Updating a non-existent ID
- Deleting a non-existent ID
- Extracting from an empty queue
- Peeking when the queue is empty
- Persistence file errors

This prevents invalid operations from silently producing incorrect results.

---

# 13. Testing

The implementation was tested for normal operations as well as edge cases.

Test scenarios include:

- Inserting multiple items
- Peeking at the minimum-priority item
- Extracting the minimum-priority item
- Extracting the maximum-priority item
- Updating priority to a lower value
- Updating priority to a higher value
- Deleting an item
- Checking empty queue state
- Duplicate ID handling
- Invalid ID handling
- Extracting from an empty queue
- Persistence across application runs

The `main.cpp` demonstration exercises the primary required operations and displays their results.

---

# 14. Real-World Use Cases

Priority queues are useful in systems where tasks need to be processed according to priority.

### CPU and Process Scheduling

Operating systems can prioritize processes based on scheduling requirements.

### Job and Task Scheduling

Background jobs can be processed according to urgency or importance.

### Network Packet Scheduling

Network systems can prioritize packets that require faster processing.

### Dijkstra's Shortest Path Algorithm

Priority queues are used to efficiently select the vertex with the smallest tentative distance.

### A* Pathfinding

Priority queues maintain candidate nodes according to their estimated cost.

### Customer Support Systems

Support requests can be processed based on severity or priority.

---

# 15. Design Decisions

## Binary Min-Heap

Selected for efficient minimum-priority operations and straightforward implementation.

## ID-to-Index Mapping

Added to efficiently locate items by ID for `update` and `delete`.

## File-Based Persistence

Selected to keep the project lightweight and avoid unnecessary database infrastructure for a standalone assessment.

## Min-Heap + Leaf Scan for Maximum

Selected instead of a min-max heap to reduce implementation complexity while still supporting the required `extract_max()` operation.

## File Format

A simple delimiter-based format was selected because the stored data model is small and straightforward:

```text
id|value|priority
```

---

# 16. Conclusion

This project implements a persistent priority queue using:

- C++17
- Binary min-heap
- `unordered_map` for ID-to-index mapping
- File-based persistence
- Exception-based error handling

The implementation provides all required priority queue operations while maintaining persistent state between application runs.

The design focuses on efficient core operations, simple persistence, maintainability, and clear separation between the queue implementation and demonstration code.