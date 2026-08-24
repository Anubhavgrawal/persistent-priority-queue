#include <cstdio>
#include <iostream>
#include "module.cpp"

using namespace std;

void printItem(const QueueItem& item) {
    cout << "ID: " << item.id
         << " | Value: " << item.value
         << " | Priority: " << item.priority
         << '\n';
}

int main() {
    // Reset demo data so the demonstration is repeatable.
    remove("data/queue.dat");

    PriorityQueue pq;

    cout << "=== Persistent Priority Queue Demo ===\n\n";

    // Insert
    cout << "1. INSERT\n";

    pq.insert(1, "Fix login bug", 5);
    pq.insert(2, "Database issue", 2);
    pq.insert(3, "Deploy application", 8);
    pq.insert(4, "Send email", 3);

    cout << "Inserted 4 items.\n\n";

    // Peek
    cout << "2. PEEK\n";
    printItem(pq.peek());

    // Extract minimum
    cout << "\n3. EXTRACT MIN\n";
    printItem(pq.extract_min());

    // Extract maximum
    cout << "\n4. EXTRACT MAX\n";
    printItem(pq.extract_max());

    // Update
    cout << "\n5. UPDATE\n";
    pq.update(1, 1);
    cout << "Updated ID 1 priority to 1.\n";
    printItem(pq.peek());

    // Delete
    cout << "\n6. DELETE\n";
    printItem(pq.deleteItem(1));

    // Is empty
    cout << "\n7. IS EMPTY\n";
    cout << "Queue empty: "
         << (pq.is_empty() ? "Yes" : "No")
         << '\n';

    cout << "\nQueue size: " << pq.size() << '\n';

    // Persistence
    cout << "\n8. PERSISTENCE\n";
    cout << "Queue state saved to data/queue.dat\n";

    cout << "\n=== Demo Completed Successfully ===\n";

    return 0;
}