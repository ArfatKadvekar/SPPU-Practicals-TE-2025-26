#include <iostream>
#include <vector>
using namespace std;

// Display memory allocation results
void showResults(string name, vector<int> blocks, vector<int> assigned, vector<int> procs) {
    cout << "\n=== " << name << " ===\n";
    cout << "Block  Size  Status\n";
    
    int freeSpace = 0;
    for (int i = 0; i < blocks.size(); i++) {
        cout << (i+1) << "      " << blocks[i] << "    ";
        if (assigned[i] == -1) {
            cout << "Free\n";
            freeSpace += blocks[i];
        } else {
            cout << "P" << (assigned[i]+1) << " (" << procs[assigned[i]] << " KB)\n";
        }
    }
    cout << "\nFree Memory: " << freeSpace << " KB\n";
}

// First Fit: Use first available block
void firstFit(vector<int> blocks, vector<int> procs) {
    vector<int> assigned(blocks.size(), -1);
    
    for (int i = 0; i < procs.size(); i++) {
        for (int j = 0; j < blocks.size(); j++) {
            if (assigned[j] == -1 && blocks[j] >= procs[i]) {
                assigned[j] = i;
                break;
            }
        }
    }
    showResults("First Fit", blocks, assigned, procs);
}

// Best Fit: Use smallest sufficient block
void bestFit(vector<int> blocks, vector<int> procs) {
    vector<int> assigned(blocks.size(), -1);
    
    for (int i = 0; i < procs.size(); i++) {
        int best = -1;
        for (int j = 0; j < blocks.size(); j++) {
            if (assigned[j] == -1 && blocks[j] >= procs[i]) {
                if (best == -1 || blocks[j] < blocks[best])
                    best = j;
            }
        }
        if (best != -1) assigned[best] = i;
    }
    showResults("Best Fit", blocks, assigned, procs);
}

// Worst Fit: Use largest available block
void worstFit(vector<int> blocks, vector<int> procs) {
    vector<int> assigned(blocks.size(), -1);
    
    for (int i = 0; i < procs.size(); i++) {
        int worst = -1;
        for (int j = 0; j < blocks.size(); j++) {
            if (assigned[j] == -1 && blocks[j] >= procs[i]) {
                if (worst == -1 || blocks[j] > blocks[worst])
                    worst = j;
            }
        }
        if (worst != -1) assigned[worst] = i;
    }
    showResults("Worst Fit", blocks, assigned, procs);
}

// Next Fit: Continue from last position
void nextFit(vector<int> blocks, vector<int> procs) {
    vector<int> assigned(blocks.size(), -1);
    int pos = 0;
    
    for (int i = 0; i < procs.size(); i++) {
        int start = pos;
        do {
            if (assigned[pos] == -1 && blocks[pos] >= procs[i]) {
                assigned[pos] = i;
                pos = (pos + 1) % blocks.size();
                break;
            }
            pos = (pos + 1) % blocks.size();
        } while (pos != start);
    }
    showResults("Next Fit", blocks, assigned, procs);
}

int main() {
    cout << "Memory Allocation Simulator\n\n";
    
    int n;
    cout << "Number of blocks: ";
    cin >> n;
    vector<int> blocks(n);
    cout << "Block sizes: ";
    for (int i = 0; i < n; i++) cin >> blocks[i];
    
    int m;
    cout << "Number of processes: ";
    cin >> m;
    vector<int> procs(m);
    cout << "Process sizes: ";
    for (int i = 0; i < m; i++) cin >> procs[i];
    
    int choice;
    do {
        cout << "\n1. First Fit\n2. Best Fit\n3. Worst Fit\n";
        cout << "4. Next Fit\n5. Run All\n6. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1: firstFit(blocks, procs); break;
            case 2: bestFit(blocks, procs); break;
            case 3: worstFit(blocks, procs); break;
            case 4: nextFit(blocks, procs); break;
            case 5: 
                firstFit(blocks, procs);
                bestFit(blocks, procs);
                worstFit(blocks, procs);
                nextFit(blocks, procs);
                break;
            case 6: cout << "Bye!\n"; break;
            default: cout << "Invalid!\n";
        }
    } while (choice != 6);
    
    return 0;
}
