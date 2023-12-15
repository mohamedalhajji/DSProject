#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

string parseFilename(const string& logEntry) {
    size_t startPos = logEntry.find("GET ") + 4;
    size_t endPos = logEntry.find(" HTTP", startPos);
    return (startPos != string::npos && endPos != string::npos) ?
        logEntry.substr(startPos, endPos - startPos) : "";
}

int main() {
    unordered_map<string, int> fileVisitsTable;
    ifstream logfile("D:\\Downloads\\project\\access_log.txt");

    if (!logfile.is_open()) {
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
    auto comp = [](const pair<string, int>& p1, const pair<string, int>& p2) {
        return p1.second < p2.second; // Sorting by visit count, descending
        };
    priority_queue<pair<string, int>, vector<pair<string, int>>, decltype(comp)> minHeap(comp);

    for (const auto& entry : fileVisitsTable) {
        minHeap.push(entry);
        if (minHeap.size() > 10) {
            minHeap.pop();
        }
    }

    vector<pair<string, int>> topPages;
    while (!minHeap.empty()) {
        topPages.push_back(minHeap.top());
        minHeap.pop();
    }
    reverse(topPages.begin(), topPages.end());

    for (const auto& page : topPages) {
        cout << "Filename: " << page.first << ", Visits: " << page.second << endl;
    }

    return 0;
}