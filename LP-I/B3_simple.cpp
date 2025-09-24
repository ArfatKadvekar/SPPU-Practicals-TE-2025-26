#include <iostream>
#include <vector>
using namespace std;

class MemoryManager {
private:
    vector<int> blocks;
    vector<bool> allocated;
    vector<int> processId;
    vector<int> originalBlocks;
    int nextFitIndex;
    
public:
    MemoryManager(vector<int>& blockSizes) {
        blocks = blockSizes;
        originalBlocks = blockSizes;
        allocated.resize(blockSizes.size(), false);
        processId.resize(blockSizes.size(), -1);
        nextFitIndex = 0;
    }
    
    void reset() {
        blocks = originalBlocks;
        allocated.assign(blocks.size(), false);
        processId.assign(blocks.size(), -1);
        nextFitIndex = 0;
    }
    
    // First Fit Algorithm
    bool firstFit(int processSize, int pid) {
        for (int i = 0; i < blocks.size(); i++) {
            if (!allocated[i] && blocks[i] >= processSize) {
                allocated[i] = true;
                processId[i] = pid;
                return true;
            }
        }
        return false;
    }
    
    // Best Fit Algorithm
    bool bestFit(int processSize, int pid) {
        int bestIndex = -1;
        int minWaste = 999999;
        
        for (int i = 0; i < blocks.size(); i++) {
            if (!allocated[i] && blocks[i] >= processSize) {
                int waste = blocks[i] - processSize;
                if (waste < minWaste) {
                    minWaste = waste;
                    bestIndex = i;
                }
            }
        }
        
        if (bestIndex != -1) {
            allocated[bestIndex] = true;
            processId[bestIndex] = pid;
            return true;
        }
        return false;
    }
    
    // Worst Fit Algorithm
    bool worstFit(int processSize, int pid) {
        int worstIndex = -1;
        int maxWaste = -1;
        
        for (int i = 0; i < blocks.size(); i++) {
            if (!allocated[i] && blocks[i] >= processSize) {
                int waste = blocks[i] - processSize;
                if (waste > maxWaste) {
                    maxWaste = waste;
                    worstIndex = i;
                }
            }
        }
        
        if (worstIndex != -1) {
            allocated[worstIndex] = true;
            processId[worstIndex] = pid;
            return true;
        }
        return false;
    }
    
    // Next Fit Algorithm
    bool nextFit(int processSize, int pid) {
        int start = nextFitIndex;
        
        do {
            if (!allocated[nextFitIndex] && blocks[nextFitIndex] >= processSize) {
                allocated[nextFitIndex] = true;
                processId[nextFitIndex] = pid;
                nextFitIndex = (nextFitIndex + 1) % blocks.size();
                return true;
            }
            nextFitIndex = (nextFitIndex + 1) % blocks.size();
        } while (nextFitIndex != start);
        
        return false;
    }
    
    void displayMemory(string algorithmName, vector<int>& processes) {
        cout << "\n" << algorithmName << " Results:" << endl;
        cout << "Block\tSize\tStatus\tProcess" << endl;
        cout << "--------------------------------" << endl;
        
        for (int i = 0; i < blocks.size(); i++) {
            cout << i + 1 << "\t" << blocks[i] << "\t";
            if (allocated[i]) {
                int pid = processId[i];
                cout << "Used\tP" << pid << " (" << processes[pid-1] << " KB)";
            } else {
                cout << "Free\t-";
            }
            cout << endl;
        }
        
        // Calculate total free memory
        int totalFree = 0;
        for (int i = 0; i < blocks.size(); i++) {
            if (!allocated[i]) {
                totalFree += blocks[i];
            }
        }
        cout << "\nTotal Free Memory: " << totalFree << " KB" << endl;
    }
};

void runAlgorithm(MemoryManager& mm, vector<int>& processes, int choice) {
    string algoName[] = {"", "First Fit", "Best Fit", "Worst Fit", "Next Fit"};
    
    cout << "\n========== " << algoName[choice] << " ==========" << endl;
    mm.reset();
    
    for (int i = 0; i < processes.size(); i++) {
        bool success = false;
        
        if (choice == 1) success = mm.firstFit(processes[i], i + 1);
        else if (choice == 2) success = mm.bestFit(processes[i], i + 1);
        else if (choice == 3) success = mm.worstFit(processes[i], i + 1);
        else if (choice == 4) success = mm.nextFit(processes[i], i + 1);
        
        if (success) {
            cout << "P" << i + 1 << " (" << processes[i] << " KB) - Allocated" << endl;
        } else {
            cout << "P" << i + 1 << " (" << processes[i] << " KB) - Failed" << endl;
        }
    }
    
    mm.displayMemory(algoName[choice], processes);
}

int main() {
    cout << "Memory Allocation Strategies" << endl;
    cout << "============================" << endl;
    
    // Input memory blocks
    int numBlocks;
    cout << "Enter number of memory blocks: ";
    cin >> numBlocks;
    
    vector<int> blocks(numBlocks);
    cout << "Enter block sizes: ";
    for (int i = 0; i < numBlocks; i++) {
        cin >> blocks[i];
    }
    
    // Input processes
    int numProcesses;
    cout << "Enter number of processes: ";
    cin >> numProcesses;
    
    vector<int> processes(numProcesses);
    cout << "Enter process sizes: ";
    for (int i = 0; i < numProcesses; i++) {
        cin >> processes[i];
    }
    
    // Display input
    cout << "\nMemory Blocks: ";
    for (int i = 0; i < blocks.size(); i++) {
        cout << blocks[i] << " ";
    }
    cout << "\nProcess Sizes: ";
    for (int i = 0; i < processes.size(); i++) {
        cout << processes[i] << " ";
    }
    cout << endl;
    
    MemoryManager mm(blocks);
    
    // Simple menu
    int choice;
    do {
        cout << "\nChoose Algorithm:" << endl;
        cout << "1. First Fit" << endl;
        cout << "2. Best Fit" << endl;
        cout << "3. Worst Fit" << endl;
        cout << "4. Next Fit" << endl;
        cout << "5. Run All" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        
        if (choice >= 1 && choice <= 4) {
            runAlgorithm(mm, processes, choice);
        }
        else if (choice == 5) {
            for (int i = 1; i <= 4; i++) {
                runAlgorithm(mm, processes, i);
            }
        }
        else if (choice == 6) {
            cout << "Goodbye!" << endl;
        }
        else {
            cout << "Invalid choice!" << endl;
        }
        
    } while (choice != 6);
    
    return 0;
}