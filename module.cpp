#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct QueueItem {
    int id;
    string value;
    int priority;
};

class PriorityQueue {
private:
    vector<QueueItem> heap;
    unordered_map<int, int> idToIndex;

    const string filePath = "data/queue.dat";

    // Defines the ordering used by the min-heap.
    // Lower priority values are extracted first.
    // ID is used as a deterministic tie-breaker.
    bool comesBefore(const QueueItem& a, const QueueItem& b) const {
        if (a.priority != b.priority) {
            return a.priority < b.priority;
        }

        return a.id < b.id;
    }

    // Swap two heap elements and keep the index map synchronized.
    void swapItems(int i, int j) {
        swap(heap[i], heap[j]);

        idToIndex[heap[i].id] = i;
        idToIndex[heap[j].id] = j;
    }

    // Restore heap property by moving an element toward the root.
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (!comesBefore(heap[index], heap[parent])) {
                break;
            }

            swapItems(index, parent);
            index = parent;
        }
    }

    // Restore heap property by moving an element toward the leaves.
    void heapifyDown(int index) {
        int n = static_cast<int>(heap.size());

        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;

            if (left < n && comesBefore(heap[left], heap[smallest])) {
                smallest = left;
            }

            if (right < n && comesBefore(heap[right], heap[smallest])) {
                smallest = right;
            }

            if (smallest == index) {
                break;
            }

            swapItems(index, smallest);
            index = smallest;
        }
    }

    // Restore heap property after an arbitrary element changes.
    void restoreHeap(int index) {
        if (index > 0) {
            int parent = (index - 1) / 2;

            if (comesBefore(heap[index], heap[parent])) {
                heapifyUp(index);
                return;
            }
        }

        heapifyDown(index);
    }

    // Remove an element at a specific heap index.
    QueueItem removeAt(int index) {
        QueueItem removed = heap[index];

        idToIndex.erase(removed.id);

        int lastIndex = static_cast<int>(heap.size()) - 1;

        if (index != lastIndex) {
            heap[index] = heap[lastIndex];
            idToIndex[heap[index].id] = index;
        }

        heap.pop_back();

        if (index < static_cast<int>(heap.size())) {
            restoreHeap(index);
        }

        return removed;
    }

    // Persist the current queue state to disk.
    void saveToFile() const {
        ofstream file(filePath);

        if (!file.is_open()) {
            throw runtime_error("Unable to open persistence file for writing.");
        }

        for (const QueueItem& item : heap) {
            file << item.id << '|'
                 << item.value << '|'
                 << item.priority << '\n';
        }
    }

    // Load queue state from disk and rebuild the heap/index map.
    void loadFromFile() {
        ifstream file(filePath);

        // The file may not exist on the first run.
        if (!file.is_open()) {
            return;
        }

        heap.clear();
        idToIndex.clear();

        string line;

        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            stringstream ss(line);

            string idString;
            string value;
            string priorityString;

            getline(ss, idString, '|');
            getline(ss, value, '|');
            getline(ss, priorityString, '|');

            if (idString.empty() || priorityString.empty()) {
                continue;
            }

            try {
                QueueItem item{
                    stoi(idString),
                    value,
                    stoi(priorityString)
                };

                heap.push_back(item);
            }
            catch (const exception&) {
                // Ignore malformed records rather than stopping the
                // entire queue from loading.
                continue;
            }
        }

        file.close();

        // Rebuild the ID-to-index mapping.
        for (int i = 0; i < static_cast<int>(heap.size()); ++i) {
            idToIndex[heap[i].id] = i;
        }

        // Build a valid min-heap from the loaded elements.
        for (int i = static_cast<int>(heap.size()) / 2 - 1; i >= 0; --i) {
            heapifyDown(i);
        }
    }

public:
    PriorityQueue() {
        loadFromFile();
    }

    // Insert a new item into the priority queue.
    void insert(int id, const string& value, int priority) {
        if (idToIndex.count(id)) {
            throw runtime_error("ID already exists.");
        }

        QueueItem item{id, value, priority};

        heap.push_back(item);

        int index = static_cast<int>(heap.size()) - 1;
        idToIndex[id] = index;

        heapifyUp(index);

        saveToFile();
    }

    // Return the minimum-priority item without removing it.
    QueueItem peek() const {
        if (heap.empty()) {
            throw runtime_error("Priority queue is empty.");
        }

        return heap[0];
    }

    // Remove and return the minimum-priority item.
    QueueItem extract_min() {
        if (heap.empty()) {
            throw runtime_error("Priority queue is empty.");
        }

        QueueItem result = removeAt(0);

        saveToFile();

        return result;
    }

    // Remove and return the maximum-priority item.
    //
    // In a min-heap, maximum elements are guaranteed to be among
    // the leaf nodes, so only the leaves need to be scanned.
    QueueItem extract_max() {
        if (heap.empty()) {
            throw runtime_error("Priority queue is empty.");
        }

        int n = static_cast<int>(heap.size());
        int firstLeaf = n / 2;
        int maxIndex = firstLeaf;

        for (int i = firstLeaf + 1; i < n; ++i) {
            if (comesBefore(heap[maxIndex], heap[i])) {
                maxIndex = i;
            }
        }

        QueueItem result = removeAt(maxIndex);

        saveToFile();

        return result;
    }

    // Update the priority of an existing item.
    void update(int id, int newPriority) {
        auto it = idToIndex.find(id);

        if (it == idToIndex.end()) {
            throw runtime_error("ID not found.");
        }

        int index = it->second;

        heap[index].priority = newPriority;

        restoreHeap(index);

        saveToFile();
    }

    // Implements the required delete operation.
    QueueItem deleteItem(int id) {
        auto it = idToIndex.find(id);

        if (it == idToIndex.end()) {
            throw runtime_error("ID not found.");
        }

        QueueItem result = removeAt(it->second);

        saveToFile();

        return result;
    }

    // Check whether the priority queue contains no elements.
    bool is_empty() const {
        return heap.empty();
    }

    // Return the current number of elements.
    int size() const {
        return static_cast<int>(heap.size());
    }
};
