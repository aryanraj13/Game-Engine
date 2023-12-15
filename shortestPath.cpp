#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>
using namespace std;

#define INF numeric_limits<int>::max()
static double sizeofshortest=0;

// Define a structure for representing edges
struct Edge {
    int to;
    int weight;
};

// Function to perform Dijkstra's algorithm
void dijkstra(vector<vector<Edge>>& graph, int source, vector<int>& dist, vector<int>& prev) {
    int V = graph.size();

    dist.assign(V, INF);
    prev.assign(V, -1);

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    dist[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (d > dist[u])
            continue;

        for (const Edge& e : graph[u]) {
            int v = e.to;
            int w = e.weight;

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
}

// Function to print the shortest path from source to target
void printShortestPath(vector<int>& prev, int target) {
    vector<int> path;
    for (int at = target; at != -1; at = prev[at])
        path.push_back(at);

    cout << "Shortest Path: ";
    for (int i = path.size() - 1; i >= 0; --i) {
        cout << path[i];
        if (i != 0)
            cout << " -> ";
    }
    sizeofshortest=path.size();
    cout << endl;
}

// Function to compare paths
bool comparePaths(const vector<int>& shortestPath, const vector<int>& samplePath) {
    if (shortestPath.size() != samplePath.size())
        return false;

    for (size_t i = 0; i < shortestPath.size(); ++i) {
        if (shortestPath[i] != samplePath[i])
            return false;
    }

    return true;
}

// Function to calculate the score based on path similarity
double calculateScore(const vector<int>& shortestPath, const vector<int>& userPath) {
    // Calculate score based on the formula provided
    // double shortestPathLength = shortestPath.size();
    double totalPathLength = userPath.size();
    double score = (sizeofshortest / totalPathLength) * 100;

    return score;
}

int main() {
    // Example graph representation (Adjacency List)
    int V = 10; // Number of tiles
    vector<vector<Edge>> graph(V);

    graph[0].push_back({1, 1});
    graph[0].push_back({2, 3});
    graph[1].push_back({3, 4}); 
    graph[1].push_back({4, 2});
    graph[2].push_back({5, 2});
    graph[3].push_back({6, 5}); 
    graph[3].push_back({7, 2});
    graph[4].push_back({8, 3});
    graph[5].push_back({8, 1}); 
    graph[5].push_back({9, 4});
    graph[6].push_back({9, 2});
    graph[7].push_back({9, 3});
    graph[8].push_back({9, 1});

    // Source and target nodes
    int start = 0;
    int end = 9;

    // Vectors to store distances and previous nodes
    vector<int> dist, prev;

    // Apply Dijkstra's algorithm
    dijkstra(graph, start, dist, prev);

    // Print shortest path to the target node
    printShortestPath(prev, end);

    // Sample paths taken by different users
    vector<vector<int>> samplePaths = {
    {0, 1, 3, 7, 8, 9},                   // Shortest path
    {0, 1, 3, 7, 8, 9, 9, 9, 9, 9},       // Extended by repeating the last node
    {0, 1, 3, 7, 8, 9, 4, 5, 6, 7, 8, 9}, // Extended by adding more nodes
    {0, 1, 3, 7, 8, 9, 2, 5, 8, 9}      // Extended by adding alternative nodes
    // You can create variations of extended paths similarly
};

    // Calculate score for each user path and store in a max heap
    priority_queue<double> leaderboard;

    for (int i = 0; i < samplePaths.size(); ++i) {
        double score = calculateScore(prev, samplePaths[i]);

        // Store score in the max heap
        leaderboard.push(score);
    }

    // Display leaderboard
    cout << "Leaderboard:" << endl;
    while (!leaderboard.empty()) {
        cout << leaderboard.top() << endl;
        leaderboard.pop();
    }

    return 0;
}