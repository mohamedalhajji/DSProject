#include <vector>
#include <list>
#include <string>
using namespace std;

// A simple hash function for strings
unsigned int HashFunction(const string& key, int tableSize) {
    unsigned int hashVal = 0;
    for (char ch : key) {
        hashVal += ch;
    }
    return hashVal % tableSize;
}

template<typename K, typename V>
class HashNode {
public:
    K key;
    V value;
    HashNode* next;

    HashNode(K key, V value) : key(key), value(value), next(nullptr) {}
};

template<typename K, typename V>
class HashTable {
private:
    vector<HashNode<K, V>*> table;
    int capacity;
    int size;

    // Hash function as defined earlier
    unsigned int getHashIndex(const K& key) {
        return hashFunction(key, capacity);
    }

public:
    HashTable(int capacity) : capacity(capacity), size(0) {
        table.resize(capacity, nullptr);
    }

    void insert(const K& key, const V& value) {
        void HashTable<K, V>::insert(const K & key, const V & value) {
            unsigned int hashIndex = getHashIndex(key);
            HashNode<K, V>* prev = nullptr;
            HashNode<K, V>* entry = table[hashIndex];

            while (entry != nullptr && entry->key != key) {
                prev = entry;
                entry = entry->next;
            }

            if (entry == nullptr) {
                entry = new HashNode<K, V>(key, value);
                if (prev == nullptr) {
                    // Insert as first bucket
                    table[hashIndex] = entry;
                }
                else {
                    prev->next = entry;
                }
            }
            else {
                // Just update the value
                entry->value = value;
            }
            size++;
        }

    }

    V search(const K& key) {
        V HashTable<K, V>::search(const K & key) {
            unsigned int hashIndex = getHashIndex(key);
            HashNode<K, V>* entry = table[hashIndex];

            while (entry != nullptr) {
                if (entry->key == key) {
                    return entry->value;
                }
                entry = entry->next;
            }

            // Key not found, handle accordingly
            throw std::runtime_error("Key not found");
        }

    }

    void remove(const K& key) {
        void HashTable<K, V>::remove(const K & key) {
            unsigned int hashIndex = getHashIndex(key);
            HashNode<K, V>* entry = table[hashIndex];
            HashNode<K, V>* prev = nullptr;

            while (entry != nullptr && entry->key != key) {
                prev = entry;
                entry = entry->next;
            }

            if (entry == nullptr) {
                // Key not found
                return;
            }

            if (prev == nullptr) {
                // Remove first bucket
                table[hashIndex] = entry->next;
            }
            else {
                prev->next = entry->next;
            }

            delete entry;
            size--;
        }

    }

    // Destructor to clean up the table
    ~HashTable() {
        HashTable<K, V>::~HashTable() {
            for (HashNode<K, V>* headNode : table) {
                while (headNode != nullptr) {
                    HashNode<K, V>* prev = headNode;
                    headNode = headNode->next;
                    delete prev;
                }
            }
        }

    }

    // Other methods like resize/rehashing can be added here
};
