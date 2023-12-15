#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <utility>
#include <unordered_map>
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
    size_t capacity;               // Current capacity of the hash table
    size_t size;                  // Number of elements in the hash table
    bool isRehashing;            // Flag to check if rehashing is in progress

    size_t nextPrime(size_t n) {
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
    unsigned int getHashIndex(const K& key, size_t probe) const {
        size_t hashIndex1 = hash1(key, capacity);
        size_t hashIndex2 = hash2(key, capacity);
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

    // Search method: Finds and returns a pointer to the value associated with a given key
    V search(const K& key) {
        size_t probe = 0;
        while (probe < capacity) {
            size_t hashIndex = getHashIndex(key, probe);
            if (table[hashIndex] != nullptr && !table[hashIndex]->isDeleted) {
                return table[hashIndex]->value; // Found the value
            }
            ++probe;
        }
        return V(); // Return the default value for V
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

    vector<pair<K, V>> getTable() const {
        vector<pair<K, V>> allItems;
        for (auto* node : table) {
            if (node != nullptr && !node->isDeleted) {
                allItems.emplace_back(node->key, node->value);
            }
        }
        return allItems;
    };
};

struct Compare {
    bool operator()(const pair<int, string>& p1, const pair<int, string>& p2) {
            return p1.first < p2.first;
    }
};

string parseFilename(const string& logEntry) {
    size_t startPos = logEntry.find("GET ") + 4;
    size_t endPos = logEntry.find(" HTTP", startPos);
    return (startPos != string::npos && endPos != string::npos) ?
        logEntry.substr(startPos, endPos - startPos) : "";
}

int main()
{
    unordered_map<string, int> fileVisitsTable;
    ifstream logfile("D:\\Downloads\\project\\access_log.txt");

    if (!logfile.is_open()) 
    {
        cerr << "Error opening file" << endl;
        return 1;
    }

    string line;
    while (getline(logfile, line)) {
        string filename = parseFilename(line); // Extract filename from log entry
        if (!filename.empty()) {
            fileVisitsTable[filename]++;
        }
    }
    logfile.close();

    // Check for 'index.html' in the map
    auto it = fileVisitsTable.find("index.html");
    if (it != fileVisitsTable.end()) {
        cout << "'index.html' visit count: " << it->second << endl;
    }
    else {
        cout << "'index.html' was not found in the hash table." << endl;
    }

    // Define the min heap to store the top 10 visited pages
    auto comp = [](const pair<int, string>& p1, const pair<int, string>& p2) {
        return p1.first > p2.first;
        };
    priority_queue<pair<int, string>, vector<pair<int, string>>, decltype(comp)> minHeap(comp);

    for (const auto& entry : fileVisitsTable) {
        minHeap.push(make_pair(entry.second, entry.first));
        if (minHeap.size() > 10) {
            minHeap.pop();
        }
    }

    vector<pair<int, string>> topPages;
    while (!minHeap.empty()) {
        topPages.push_back(minHeap.top());
        minHeap.pop();
    }
    reverse(topPages.begin(), topPages.end());

    for (const auto& page : topPages) {
        cout << "Filename: " << page.second << ", Visits: " << page.first << endl;
    }

    return 0;
}