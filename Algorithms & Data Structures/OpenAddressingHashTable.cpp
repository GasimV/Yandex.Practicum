#include <iostream>

#define NIL -1 // Represents an empty slot

using namespace std;

class OpenAddressingHashTable {
private:
    int* table; // Dynamic array for storing keys
    int TABLE_SIZE; // Hash table size

    // Hash function with linear probing
    int hashFunction(int key, int i) {
        return (key + i) % TABLE_SIZE; // h(k, i) = (h'(k) + i) % m
    }

public:
    // Constructor - Initializes table with given size
    OpenAddressingHashTable(int size) {
        TABLE_SIZE = size;
        table = new int[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = NIL;
        }
    }

    // Hash-Insert(T, k)
    int HashInsert(int key) {
        int i = 0;
        int q;
        do {
            q = hashFunction(key, i);
            if (table[q] == NIL) {
                table[q] = key;
                return q;
            }
            i++;
        } while (i < TABLE_SIZE);

        cout << "Error: Hash table overflow!" << endl;
        return -1; // Indicating failure
    }

    // Hash-Search(T, k)
    int HashSearch(int key) {
        int i = 0;
        int q;
        do {
            q = hashFunction(key, i);
            if (table[q] == key) {
                return q; // Key found at index q
            }
            if (table[q] == NIL) {
                return -1; // Key not present
            }
            i++;
        } while (i < TABLE_SIZE);

        return -1; // Key not found
    }

    // Hash-Delete(T, q)
    void HashDelete(int key) {
        int q = HashSearch(key);
        if (q == -1) {
            cout << "Error: Key " << key << " not found in hash table!" << endl;
            return;
        }
    
        // Step 1: Empty the slot
        table[q] = NIL;
    
        // Step 2: Shift keys to maintain search correctness
        int q_prime = q;
        while (true) {
            q_prime = (q_prime + 1) % TABLE_SIZE; // Move to next slot in linear probing
            int k_prime = table[q_prime];
    
            if (k_prime == NIL) {
                return; // Stop when an empty slot is found
            }
    
            // Compute original hash location of k_prime
            int originalIndex = k_prime % TABLE_SIZE;
    
            // If k_prime originally belonged before q_prime but was placed later due to probing,
            // it should be moved back into the now-empty slot q.
            if ((q_prime > q && (originalIndex <= q || originalIndex > q_prime)) ||
                (q_prime < q && (originalIndex <= q && originalIndex > q_prime))) {
                table[q] = k_prime; // Move key into the freed slot
                table[q_prime] = NIL; // Free up q_prime
                q = q_prime; // Continue checking the next slot
            }
        }
    }

    // Display Hash Table
    void PrintTable() {
        cout << "Hash Table: " << endl;
        for (int i = 0; i < TABLE_SIZE; i++) {
            if (table[i] == NIL) {
                cout << "[" << i << "]: NIL" << endl;
            } else {
                cout << "[" << i << "]: " << table[i] << endl;
            }
        }
        cout << endl;
    }

    // Destructor - Free allocated memory
    ~OpenAddressingHashTable() {
        delete[] table;
    }
};

int main() {
    int tableSize;
    cout << "Enter table size: ";
    cin >> tableSize;

    OpenAddressingHashTable hashTable(tableSize);

    // Insert elements
    hashTable.HashInsert(74);
    hashTable.HashInsert(43);
    hashTable.HashInsert(93);
    hashTable.HashInsert(18);
    hashTable.HashInsert(82);
    hashTable.HashInsert(38);
    hashTable.HashInsert(92);

    cout << "After Insertions:" << endl;
    hashTable.PrintTable();

    // Test search operation
    cout << "Testing Search Operation:" << endl;
    int searchKeys[] = {43, 74, 100}; // Searching for existing & non-existing keys
    for (int key : searchKeys) {
        int result = hashTable.HashSearch(key);
        if (result != -1)
            cout << "Key " << key << " found at index " << result << endl;
        else
            cout << "Key " << key << " not found in the table." << endl;
    }
    cout << endl;

    // Delete key 43
    hashTable.HashDelete(43);

    cout << "After Deleting 43:" << endl;
    hashTable.PrintTable();

    return 0;
}