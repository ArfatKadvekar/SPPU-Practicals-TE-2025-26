#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <climits>

using namespace std;

struct Block {
    int size;
    bool allocated;
    int processId;
    
    Block(int s) : size(s), allocated(false), processId(-1) {}
};

class MemoryManager {
private:
    vector<Block> memory;
    vector<Block> originalMemory; // Store original state
    int nextFitIndex;
    
public:
    MemoryManager(vector<int>& blockSizes) : nextFitIndex(0) {
        for (int size : blockSizes) {
            Block block(size);
            memory.push_back(block);
            originalMemory.push_back(block);
        }
    }
    
    // Reset memory to original state
    void reset() {
        memory = originalMemory;
        nextFitIndex = 0;
    }
    
    // First Fit Algorithm
    bool firstFit(int processSize, int processId) {
        for (int i = 0; i < memory.size(); i++) {
            if (!memory[i].allocated && memory[i].size >= processSize) {
                memory[i].allocated = true;
                memory[i].processId = processId;
                
                // Split block if necessary
                if (memory[i].size > processSize) {
                    Block newBlock(memory[i].size - processSize);
                    memory[i].size = processSize;
                    memory.insert(memory.begin() + i + 1, newBlock);
                }
                return true;
            }
        }
        return false;
    }
    
    // Best Fit Algorithm
    bool bestFit(int processSize, int processId) {
        int bestIndex = -1;
        int minWaste = INT_MAX;
        
        for (int i = 0; i < memory.size(); i++) {
            if (!memory[i].allocated && memory[i].size >= processSize) {
                int waste = memory[i].size - processSize;
                if (waste < minWaste) {
                    minWaste = waste;
                    bestIndex = i;
                }
            }
        }
        
        if (bestIndex != -1) {
            memory[bestIndex].allocated = true;
            memory[bestIndex].processId = processId;
            
            // Split block if necessary
            if (memory[bestIndex].size > processSize) {
                Block newBlock(memory[bestIndex].size - processSize);
                memory[bestIndex].size = processSize;
                memory.insert(memory.begin() + bestIndex + 1, newBlock);
            }
            return true;
        }
        return false;
    }
    
    // Worst Fit Algorithm
    bool worstFit(int processSize, int processId) {
        int worstIndex = -1;
        int maxWaste = -1;
        
        for (int i = 0; i < memory.size(); i++) {
            if (!memory[i].allocated && memory[i].size >= processSize) {
                int waste = memory[i].size - processSize;
                if (waste > maxWaste) {
                    maxWaste = waste;
                    worstIndex = i;
                }
            }
        }
        
        if (worstIndex != -1) {
            memory[worstIndex].allocated = true;
            memory[worstIndex].processId = processId;
            
            // Split block if necessary
            if (memory[worstIndex].size > processSize) {
                Block newBlock(memory[worstIndex].size - processSize);
                memory[worstIndex].size = processSize;
                memory.insert(memory.begin() + worstIndex + 1, newBlock);
            }
            return true;
        }
        return false;
    }
    
    // Next Fit Algorithm
    bool nextFit(int processSize, int processId) {
        int startIndex = nextFitIndex;
        
        do {
            if (nextFitIndex >= memory.size()) {
                nextFitIndex = 0;
            }
            
            if (!memory[nextFitIndex].allocated && memory[nextFitIndex].size >= processSize) {
                memory[nextFitIndex].allocated = true;
                memory[nextFitIndex].processId = processId;
                
                // Split block if necessary
                if (memory[nextFitIndex].size > processSize) {
                    Block newBlock(memory[nextFitIndex].size - processSize);
                    memory[nextFitIndex].size = processSize;
                    memory.insert(memory.begin() + nextFitIndex + 1, newBlock);
                }
                nextFitIndex = (nextFitIndex + 1) % memory.size();
                return true;
            }
            nextFitIndex = (nextFitIndex + 1) % memory.size();
        } while (nextFitIndex != startIndex);
        
        return false;
    }
    
    // Display memory state
    void displayMemory(const string& algorithmName) {
        cout << "\n" << algorithmName << " Memory State:" << endl;
        cout << "Block\tSize\tStatus\t\tProcess" << endl;
        cout << "-----\t----\t------\t\t-------" << endl;
        
        for (int i = 0; i < memory.size(); i++) {
            cout << i + 1 << "\t" << memory[i].size << "\t";
            if (memory[i].allocated) {
                cout << "Allocated\tP" << memory[i].processId;
            } else {
                cout << "Free\t\t-";
            }
            cout << endl;
        }
    }
    
    // Calculate fragmentation
    void calculateFragmentation(const string& algorithmName) {
        int totalFree = 0;
        int largestFree = 0;
        int freeBlocks = 0;
        
        for (const auto& block : memory) {
            if (!block.allocated) {
                totalFree += block.size;
                freeBlocks++;
                if (block.size > largestFree) {
                    largestFree = block.size;
                }
            }
        }
        
        cout << "\n" << algorithmName << " Statistics:" << endl;
        cout << "Total free memory: " << totalFree << " KB" << endl;
        cout << "Largest free block: " << largestFree << " KB" << endl;
        cout << "Number of free blocks: " << freeBlocks << endl;
        if (totalFree > 0) {
            cout << "External fragmentation: " << (totalFree - largestFree) << " KB" << endl;
        } else {
            cout << "External fragmentation: 0 KB (No free memory)" << endl;
        }
    }
};

void runAlgorithm(MemoryManager& mm, const vector<int>& processSizes, int choice) {
    string algoNames[] = {"", "First Fit", "Best Fit", "Worst Fit", "Next Fit"};
    
    cout << "\n" << string(50, '=') << endl;
    cout << algoNames[choice] << " ALGORITHM" << endl;
    cout << string(50, '=') << endl;
    
    mm.reset();
    
    for (int i = 0; i < processSizes.size(); i++) {
        bool success = false;
        
        switch (choice) {
            case 1:
                success = mm.firstFit(processSizes[i], i + 1);
                break;
            case 2:
                success = mm.bestFit(processSizes[i], i + 1);
                break;
            case 3:
                success = mm.worstFit(processSizes[i], i + 1);
                break;
            case 4:
                success = mm.nextFit(processSizes[i], i + 1);
                break;
        }
        
        if (success) {
            cout << "Process P" << (i + 1) << " (" << processSizes[i] << " KB) allocated successfully" << endl;
        } else {
            cout << "Process P" << (i + 1) << " (" << processSizes[i] << " KB) could not be allocated" << endl;
        }
    }
    
    mm.displayMemory(algoNames[choice]);
    mm.calculateFragmentation(algoNames[choice]);
}

int main() {
    cout << "Memory Placement Strategies Simulation" << endl;
    cout << "======================================" << endl;
    
    // Get memory block sizes from user
    int numBlocks;
    cout << "\nEnter number of memory blocks: ";
    cin >> numBlocks;
    
    if (numBlocks <= 0) {
        cout << "Invalid number of blocks!" << endl;
        return 1;
    }
    
    vector<int> blockSizes(numBlocks);
    cout << "Enter memory block sizes (in KB): ";
    for (int i = 0; i < numBlocks; i++) {
        cin >> blockSizes[i];
        if (blockSizes[i] <= 0) {
            cout << "Invalid block size! Block sizes must be positive." << endl;
            return 1;
        }
    }
    
    // Get process sizes from user
    int numProcesses;
    cout << "Enter number of processes: ";
    cin >> numProcesses;
    
    if (numProcesses <= 0) {
        cout << "Invalid number of processes!" << endl;
        return 1;
    }
    
    vector<int> processSizes(numProcesses);
    cout << "Enter process sizes (in KB): ";
    for (int i = 0; i < numProcesses; i++) {
        cin >> processSizes[i];
        if (processSizes[i] <= 0) {
            cout << "Invalid process size! Process sizes must be positive." << endl;
            return 1;
        }
    }
    
    // Display input summary
    cout << "\n" << string(50, '-') << endl;
    cout << "INPUT SUMMARY" << endl;
    cout << string(50, '-') << endl;
    cout << "Memory Blocks: ";
    for (int i = 0; i < blockSizes.size(); i++) {
        cout << blockSizes[i];
        if (i < blockSizes.size() - 1) cout << ", ";
    }
    cout << " KB" << endl;
    
    cout << "Process Sizes: ";
    for (int i = 0; i < processSizes.size(); i++) {
        cout << processSizes[i];
        if (i < processSizes.size() - 1) cout << ", ";
    }
    cout << " KB" << endl;
    
    MemoryManager mm(blockSizes);
    
    // Menu for algorithm selection
    int choice;
    do {
        cout << "\n" << string(50, '-') << endl;
        cout << "ALGORITHM SELECTION MENU" << endl;
        cout << string(50, '-') << endl;
        cout << "1. First Fit" << endl;
        cout << "2. Best Fit" << endl;
        cout << "3. Worst Fit" << endl;
        cout << "4. Next Fit" << endl;
        cout << "5. Run All Algorithms" << endl;
        cout << "6. Enter New Data" << endl;
        cout << "7. Exit" << endl;
        cout << "\nEnter your choice (1-7): ";
        cin >> choice;
        
        switch (choice) {
            case 1:
            case 2:
            case 3:
            case 4:
                runAlgorithm(mm, processSizes, choice);
                break;
                
            case 5:
                // Run all algorithms
                for (int i = 1; i <= 4; i++) {
                    runAlgorithm(mm, processSizes, i);
                    if (i < 4) {
                        cout << "\nPress Enter to continue to next algorithm...";
                        cin.ignore();
                        cin.get();
                    }
                }
                break;
                
            case 6:
                // Enter new data
                cout << "\nEnter number of memory blocks: ";
                cin >> numBlocks;
                
                if (numBlocks <= 0) {
                    cout << "Invalid number of blocks!" << endl;
                    break;
                }
                
                blockSizes.resize(numBlocks);
                cout << "Enter memory block sizes (in KB): ";
                for (int i = 0; i < numBlocks; i++) {
                    cin >> blockSizes[i];
                    if (blockSizes[i] <= 0) {
                        cout << "Invalid block size! Block sizes must be positive." << endl;
                        break;
                    }
                }
                
                cout << "Enter number of processes: ";
                cin >> numProcesses;
                
                if (numProcesses <= 0) {
                    cout << "Invalid number of processes!" << endl;
                    break;
                }
                
                processSizes.resize(numProcesses);
                cout << "Enter process sizes (in KB): ";
                for (int i = 0; i < numProcesses; i++) {
                    cin >> processSizes[i];
                    if (processSizes[i] <= 0) {
                        cout << "Invalid process size! Process sizes must be positive." << endl;
                        break;
                    }
                }
                
                // Recreate memory manager with new data
                mm = MemoryManager(blockSizes);
                
                // Display new input summary
                cout << "\n" << string(50, '-') << endl;
                cout << "NEW INPUT SUMMARY" << endl;
                cout << string(50, '-') << endl;
                cout << "Memory Blocks: ";
                for (int i = 0; i < blockSizes.size(); i++) {
                    cout << blockSizes[i];
                    if (i < blockSizes.size() - 1) cout << ", ";
                }
                cout << " KB" << endl;
                
                cout << "Process Sizes: ";
                for (int i = 0; i < processSizes.size(); i++) {
                    cout << processSizes[i];
                    if (i < processSizes.size() - 1) cout << ", ";
                }
                cout << " KB" << endl;
                break;
                
            case 7:
                cout << "\nThank you for using the Memory Placement Strategies Simulator!" << endl;
                break;
                
            default:
                cout << "\nInvalid choice! Please enter a number between 1 and 7." << endl;
                break;
        }
        
    } while (choice != 7);
    
    return 0;
}