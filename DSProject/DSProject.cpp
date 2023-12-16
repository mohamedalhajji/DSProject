#include <iostream>           // input/output
#include <fstream>           // file handling
#include <queue>            // priority queue
#include <unordered_map>   // standard hash table
#include <vector>         // vector container
#include <string>        // string operations
#include <regex>        // regular expression
#include <functional>  // hash funtion
#include <chrono>     // measuring time
using namespace std;
// MAKE SURE ITS RUNNING ON RELEASE MODE FOR A DRASTIC TIME DECREASE

// tempelate for each entry in custom hash table
template<typename K, typename V>
class hashN {
public:
    K key;            // key of hash node
    V value;         // value of hash node
    bool isDeleted; // deletion flag

    // initializes key, value, and deletion flag
    hashN(const K& key, const V& value) : key(key), value(value), isDeleted(false) {}
};

template<typename K, typename V>
class HT {
private:
    vector<hashN<K, V>*> table; // storage for hash table
    size_t capacity;           // max capacity for HT
    size_t size;              // current size for HT
    hash<K> hash1;           // primary hash function
    double thresh = 0.7;    // threshold for reashing

    // secondary hash function
    size_t hash2(const K& key) const {
        return ((hash1(key) * 2) + 1)   % capacity;
    }

public:
    // initializes table with given capacity
    HT(size_t initialCapacity) : capacity(initialCapacity), size(0) {
        table.resize(capacity, nullptr);
    }

    // insert class member
    void insert(const K& key, const V& value) {
        size_t index1 = hash1(key) % capacity;
        size_t index2 = hash2(key);
        size_t i = 0;
        
        while (table[(index1 + i * index2) % capacity] != nullptr &&
            !table[(index1 + i * index2) % capacity]->isDeleted &&
            table[(index1 + i * index2) % capacity]->key != key) {
            i++;
        }

        if (table[(index1 + i * index2) % capacity] != nullptr) {
            delete table[(index1 + i * index2) % capacity];
        }

        table[(index1 + i * index2) % capacity] = new hashN<K, V>(key, value);
        size++;

        // rehashes if table is 70% full
        if ((static_cast<double>(1.0) * size) / capacity > thresh) {
            rehash();
        }
    }

    // search class member
    V search(const K& key) const {
        size_t index1 = hash1(key) % capacity;
        size_t index2 = hash2(key);
        size_t i = 0;

        while (table[(index1 + i * index2) % capacity] != nullptr) {
            if (table[(index1 + i * index2) % capacity]->key == key && !table[(index1 + i * index2) % capacity]->isDeleted) {
                return table[(index1 + i * index2) % capacity]->value;
            }
            i++;
        }

        return V(); // return default constructed value if not found
    }

    // delete class member
    void remove(const K& key) {
        size_t index1 = hash1(key) % capacity;
        size_t index2 = hash2(key);
        size_t i = 0;

        while (table[(index1 + i * index2) % capacity] != nullptr) {
            if (table[(index1 + i * index2) % capacity]->key == key && !table[(index1 + i * index2) % capacity]->isDeleted) {
                table[(index1 + i * index2) % capacity]->isDeleted = true;
                size--;
                return;
            }
            i++;
        }
    }
    // function to check for prime number
    bool isPrime(size_t number) {
        if (number <= 1) return false;
        if (number <= 3) return true;
        if (number % 2 == 0 || number % 3 == 0) return false;
        for (size_t i = 5; i * i <= number; i += 6) {
            if (number % i == 0 || number % (i + 2) == 0) return false;
        }
        return true;
    }

    // function to get next prime number for table size
    size_t NextPrime(size_t number) {
        while (!isPrime(number)) {
            number++;
        }
        return number;
    }

    // destructor to clean up memory
    ~HT() {
        for (auto& node : table) {
            delete node;
        }
        table.clear();
    }

    // function to rehash table when load factor is too high
    void rehash() {
        vector<hashN<K, V>*> oldTable = table;
        capacity = NextPrime(capacity * 2);
        table.clear();
        table.resize(capacity, nullptr);
        size = 0;

        for (auto& node : oldTable) {
            if (node != nullptr && !node->isDeleted) {
                insert(node->key, node->value);
            }
            delete node;
        }
    }

    vector<pair<K, V>> getValues() const {
        vector<pair<K, V>> values;
        for (auto& entry : table) {
            if (entry != nullptr && !entry->isDeleted) {
                values.emplace_back(entry->key, entry->value);
            }
        }
        return values;
    }

    HT(size_t iCapacity, double iThresh)
        :capacity(iCapacity), size(0), thresh(iThresh) {
        table.resize(capacity, nullptr);
    }
};

// function to extract the file name from the log entry
static string analyzeFilename(const string& logEntry) {
    regex pattern(R"(GET\s+(\d+\.gif|\d+\.html)\s+HTTP)"); // regex pattern for files
    smatch matches;                                       // stores matches

    if (regex_search(logEntry, matches, pattern)) {
        return matches[1]; // if pattern is matched then return captured file name
    }

    return ""; // if no valid name then return empty

}

// using both custom hash table and unordered map (standard library) to compare
int main() {
    // custom hash table
    HT<string, int> customHT(1000000); // predifined capacity

    // unordered map hash table
    unordered_map<string, int> UoMTable(1000000); // predifined capacity

    // opening file
    ifstream file("D:\\Downloads\\project\\access_log.txt");
    // error message if unsuccessful opening
    if (!file.is_open()) {
        cerr << "Please insert the correct file destination" << endl;
        return 1;
    }

    string site;
    // start timing for both hash tables
    auto startCustom = chrono::high_resolution_clock::now();
    auto startUoM = chrono::high_resolution_clock::now();

    while (getline(file, site)) {
        string filename = analyzeFilename(site);
        if (!filename.empty()) {
            // operations on both tables
            customHT.insert(filename, customHT.search(filename) + 1);
            UoMTable[filename]++;
        }
    }

    // end timing for both tables
    auto endUoM = chrono::high_resolution_clock::now();
    auto endCustom = endUoM;

    file.close(); //closes file

    // minheap
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> minheapCustom;

    // populate min heap with visit counts for custom table
    for (const auto& count : customHT.getValues()) {
        if (minheapCustom.size() < 10) { // make sure we only keep the top 10
            minheapCustom.push(make_pair(count.second, count.first));
        }
        else if (count.second > minheapCustom.top().first) {
            minheapCustom.pop();
            minheapCustom.push(make_pair(count.second, count.first));
        }
    }

    // extract top web pages from heap and store them in custom table
    vector<pair<int, string>> topWebCustom;
    while (!minheapCustom.empty()) {
        topWebCustom.push_back(minheapCustom.top());
        minheapCustom.pop();
    }
    reverse(topWebCustom.begin(), topWebCustom.end()); // reverse to get descending order

    // Min heap to store top 10 visited pages for UoMTable
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> minheapUoM;

    // populate min heap with visit counts for UoM table
    for (const auto& count : UoMTable) {
        if (minheapUoM.size() < 10) {
            minheapUoM.push(make_pair(count.second, count.first));
        }
        else if (count.second > minheapUoM.top().first) {
            minheapUoM.pop();
            minheapUoM.push(make_pair(count.second, count.first));
        }
    }

    // extract top web pages from heap and store them in UoM table
    vector<pair<int, string>> topWebUoM;
    while (!minheapUoM.empty()) {
        topWebUoM.push_back(minheapUoM.top());
        minheapUoM.pop();
    }
    reverse(topWebUoM.begin(), topWebUoM.end()); // reverse to get descending order

    // print top 10 visited web pages
    cout << "The top 10 visited web pages are:" << endl << endl;    
    for (const auto& page : topWebCustom) {
        cout << page.second << " : " << page.first << " visits!" << endl;
    }

    // calculate and print the elapsed times for both tables
    auto customTime = chrono::duration_cast<chrono::seconds>(endCustom - startCustom);
    auto UoMTime = chrono::duration_cast<chrono::seconds>(endUoM - startUoM);
    auto totalTime = customTime + UoMTime;

    cout << endl << "Total time elapsed: " << totalTime.count() << " seconds" << endl;
    cout << endl << "Custom hash table time: " << customTime.count() << " seconds" << endl;
    cout << "Unordered_map time: " << UoMTime.count() << " seconds" << endl;

    return 0;
}