#include <iostream>
#include <vector>
#include <climits>
#include <queue>
#include <stack>

using namespace std;

const int INF = INT_MAX;

void dijkstra(const vector<vector<int>>& graph, int start, int end) {
    int n = graph.size();
    vector<int> dist(n, INF);         
    vector<bool> visited(n, false);    
    vector<int> prev(n, -1);          

    dist[start] = 0;

    for (int i = 0; i < n - 1; ++i) {
        int u = -1;
        for (int j = 0; j < n; ++j) {
            if (!visited[j] && (u == -1 || dist[j] < dist[u]))
                u = j;
        }

        if (dist[u] == INF)
            break;

        visited[u] = true;

        for (int v = 0; v < n; ++v) {
            if (graph[u][v] != 0 && !visited[v]) {
                int newDist = dist[u] + graph[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    prev[v] = u;
                }
            }
        }
    }

    if (dist[end] == INF) {
        cout << "No path exists from " << start << " to " << end << endl;
        return;
    }

    cout << "Shortest distance from " << start << " to " << end << ": " << dist[end] << endl;

    stack<int> path;
    for (int at = end; at != -1; at = prev[at])
        path.push(at);

    cout << "Path: ";
    while (!path.empty()) {
        cout << path.top();
        path.pop();
        if (!path.empty()) cout << " -> ";
    }
    cout << endl;
}

int main() {
    int n;
    cout << "Enter number of nodes: ";
    cin >> n;

    vector<vector<int>> graph(n, vector<int>(n));
    cout << "Enter adjacency matrix (0 for no direct connection):\n";
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            cin >> graph[i][j];

    int start, end;
    cout << "Enter start node (0 to " << n-1 << "): ";
    cin >> start;
    cout << "Enter end node (0 to " << n-1 << "): ";
    cin >> end;

    dijkstra(graph, start, end);

    return 0;
}