#include <vector>
#include <string>
using namespace std;

// A simple primary hash function for strings
unsigned int hash1(const string& key, int tableSize) {
    unsigned int hashVal = 0;
    for (char ch : key) {
        hashVal += ch;
    }
    return hashVal % tableSize;
}

// A secondary hash function for double hashing
unsigned int hash2(const string& key, int tableSize) {
    unsigned int hashVal = 0;
    for (char ch : key) {
        hashVal = 37 * hashVal + ch;
    }
    return (tableSize - 1) - (hashVal % (tableSize - 1));
}

template<typename K, typename V>
class HashNode {
public:
    K key;
    V value;
    bool isDeleted;

    HashNode(K key, V value) : key(key), value(value), isDeleted(false) {}
};

template<typename K, typename V>
class HashTable {
private:
    vector<HashNode<K, V>*> table;
    int capacity;
    int size;

    unsigned int getHashIndex(const K& key, int probe) {
        int hashIndex1 = hash1(key, capacity);
        int hashIndex2 = hash2(key, capacity);
        return (hashIndex1 + probe * hashIndex2) % capacity;
    }

public:
    HashTable(int capacity) : capacity(capacity), size(0) {
        table.resize(capacity, nullptr);
    }

    void insert(const K& key, const V& value) {
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

    V search(const K& key) {
        int probe = 0;
        int hashIndex;
        while (probe < capacity) {
            hashIndex = getHashIndex(key, probe);
            if (table[hashIndex] == nullptr)
                throw runtime_error("Key not found");
            if (!table[hashIndex]->isDeleted && table[hashIndex]->key == key)
                return table[hashIndex]->value;
            probe++;
        }
        throw runtime_error("Key not found");
    }

    void remove(const K& key) {
        int probe = 0;
        int hashIndex;
        while (probe < capacity) {
            hashIndex = getHashIndex(key, probe);
            if (table[hashIndex] == nullptr)
                return;
            if (!table[hashIndex]->isDeleted && table[hashIndex]->key == key) {
                table[hashIndex]->isDeleted = true;
                size--;
                return;
            }
            probe++;
        }
    }

    ~HashTable() {
        for (auto& node : table) {
            if (node != nullptr) {
                delete node;
            }
        }
    }

    void rehash() {
        vector<HashNode<K, V>*> oldTable = table;
        table = vector<HashNode<K, V>*>(nextPrime(2 * capacity), nullptr);
        size = 0;
        capacity = table.size();

        for (auto node : oldTable) {
            if (node != nullptr && !node->isDeleted) {
                insert(node->key, node->value);
                delete node;
            }
        }
    }

    void insert(const K& key, const V& value) {

        if (static_cast<double>(size) / capacity > 0.7) {
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

    V search(const K& key) {
        int probe = 0;
        int hashIndex;
        while (probe < capacity) {
            hashIndex = getHashIndex(key, probe);
            if (table[hashIndex] == nullptr)
                throw runtime_error("Key not found");
            if (!table[hashIndex]->isDeleted && table[hashIndex]->key == key)
                return table[hashIndex]->value;
            probe++;
        }
        throw runtime_error("Key not found");
    }

    void remove(const K& key) {
        int probe = 0;
        int hashIndex;
        while (probe < capacity) {
            hashIndex = getHashIndex(key, probe);
            if (table[hashIndex] == nullptr)
                return;
            if (!table[hashIndex]->isDeleted && table[hashIndex]->key == key) {
                table[hashIndex]->isDeleted = true;
                size--;
                return;
            }
            probe++;
        }
    }

    ~HashTable() {
        for (auto& node : table) {
            if (node != nullptr) {
                delete node;
            }
        }
    }

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
};
