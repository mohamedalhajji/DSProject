#include <vector>
#include <string>
#include <iostream>
using namespace std;

// Primary hash function: Sums ASCII values of characters in the key and mods by table size
unsigned int hash1(const string& key, int tableSize) {
    unsigned int hashVal = 0;
    for (char ch : key) {
        hashVal += ch;
    }
    return hashVal % tableSize;
}

// Secondary hash function for double hashing: Computes a secondary hash to avoid clustering
unsigned int hash2(const string& key, int tableSize) {
    unsigned int hashVal = 0;
    for (char ch : key) {
        hashVal = 37 * hashVal + ch;
    }
    return (tableSize - 1) - (hashVal % (tableSize - 1));
}

// HashNode class: Represents an individual node in the hash table with a key, value, and deletion flag.
template<typename K, typename V>
class HashNode {
public:
    K key;            // Key of the hash node
    V value;         // Value of the hash node
    bool isDeleted; // Flag to mark the node as logically deleted

    // Constructor: Initializes the key, value, and sets isDeleted flag to false
    HashNode(K key, V value) : key(key), value(value), isDeleted(false) {}
};

// HashTable class represents the hash table itself.
template<typename K, typename V>
class HashTable {
private:
    vector<HashNode<K, V>*> table;  // Dynamic array for the hash table
    int capacity;                  // Current capacity of the hash table
    int size;                     // Number of elements in the hash table
    bool isRehashing;            // Flag to check if rehashing is in progress
    int nextPrime(int n) {
        if (n <= 1) return 2;
        if (n <= 3) return 3;

        if (n % 2 == 0) n++;

        while (true) {
            bool isPrime = true;
            for (int i = 2; i * i <= n; i++) {
                if (n % i == 0) {
                    isPrime = false;
                    break;
                }
            }
            if (isPrime) return n;
            n += 2;
        }
    }

    // Computes the hash index for a given key and probe sequence
    unsigned int getHashIndex(const K& key, int probe) {
        int hashIndex1 = hash1(key, capacity);
        int hashIndex2 = hash2(key, capacity);
        return (hashIndex1 + probe * hashIndex2) % capacity;
    }

public:
    // Constructor: Initializes the hash table with a given capacity and default values
    HashTable(int capacity) : capacity(capacity), size(0), isRehashing(false) {
        table.resize(capacity, nullptr);
    }

    // Insert method: Inserts a key-value pair into the hash table
    void insert(const K& key, const V& value) {
        // Check for rehashing need before insertion
        if (!isRehashing && static_cast<double>(size) / capacity > 0.7) {
            rehash();
        }
        int probe = 0;
        int hashIndex;
        do {
            hashIndex = getHashIndex(key, probe);
            if (table[hashIndex] == nullptr || table[hashIndex]->isDeleted) {
                if (table[hashIndex] == nullptr)
                    table[hashIndex] = new HashNode<K, V>(key, value);
                else {
                    table[hashIndex]->key = key;
                    table[hashIndex]->value = value;
                    table[hashIndex]->isDeleted = false;
                }
                size++;
                return;
            }
            probe++;
        } while (probe < capacity);
    }

    // Search method: Finds and returns the value associated with a given key
    V search(const K& key) {
        int probe = 0; // Probing counter
        int hashIndex;
        // Continuously probe until the key is found or an empty slot is encountered
        while (probe < capacity) {
            hashIndex = getHashIndex(key, probe);
            if (table[hashIndex] == nullptr)
                throw runtime_error("Key not found"); // Key not in table
            if (!table[hashIndex]->isDeleted && table[hashIndex]->key == key)
                return table[hashIndex]->value; // Return found value
            probe++;
        }
        throw runtime_error("Key not found"); // Key not found after full probe
    }

    // Remove method: Logically deletes a key-value pair from the hash table
    void remove(const K& key) {
        int probe = 0; // Probing counter
        int hashIndex;
        // Probe to find the key
        while (probe < capacity) {
            hashIndex = getHashIndex(key, probe);
            if (table[hashIndex] == nullptr)
                return; // Key not in table
            if (!table[hashIndex]->isDeleted && table[hashIndex]->key == key) {
                table[hashIndex]->isDeleted = true; // Mark as deleted
                size--;
                return;
            }
            probe++;
        }
    }

    // Destructor: Cleans up the dynamic memory used by the hash table
    ~HashTable() {
        for (auto& node : table) {
            if (node != nullptr) {
                delete node; // Delete each node
            }
        }
    }

    // Rehash method: Resizes the table and rehashes all elements when load factor is high
    void rehash() {
        isRehashing = true;
        // Create a new larger table
        vector<HashNode<K, V>*> oldTable = table;
        table = vector<HashNode<K, V>*>(nextPrime(2 * capacity), nullptr);
        size = 0; // Reset size
        capacity = table.size();

        // Re-insert elements from old table
        for (auto node : oldTable) {
            if (node != nullptr && !node->isDeleted) {
                insert(node->key, node->value);
                delete node; // Delete old node
            }
        }
        isRehashing = false; // Sets isDeleted flag back to false
    }
};

int main() {

    HashTable<string, int> MyTable(100);
    int a;
    MyTable.insert("Mohammed", 1);
    a = MyTable.search(1);
    cout << a;





}