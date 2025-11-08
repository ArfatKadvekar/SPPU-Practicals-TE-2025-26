#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

bool isPresent(vector<int> frames, int page) {
    for (int f : frames) {
        if (f == page) return true;
    }
    return false;
}

void showFrames(vector<int> frames) {
    for (int i = 0; i < frames.size(); i++)
        cout << "+-----+ ";
    cout << "\n";
    
    for (int f : frames)
        cout << "|  " << setw(2) <<f << "  |";
    cout << "\n";
    
    for (int i = 0; i < frames.size(); i++)
        cout << "+-----+ ";
    cout << "\n";
}

void FIFO(vector<int> pages, int capacity) {
    cout << "\n--- FIFO Page Replacement ---\n";
    vector<int> frames;
    int pos = 0, faults = 0;
    
    for (int page : pages) {
        if (!isPresent(frames, page)) {
            if (frames.size() < capacity) {
                frames.push_back(page);
            } else {
                frames[pos] = page;
                pos = (pos + 1) % capacity;
            }
            faults++;
            cout << "Page " << page << " caused a page fault.\n";
            cout << "\033[42m Page MISS.\033[0m\n";
        } else {
            cout << "\033[41m Page " << page << " HIT.\033[0m\n";
        }
        showFrames(frames);
    }
    cout << "Total Page Faults = " << faults << "\n";
}

void LRU(vector<int> pages, int capacity) {
    cout << "\n--- LRU Page Replacement ---\n";
    vector<int> frames, lastUsed;
    int faults = 0;
    
    for (int i = 0; i < pages.size(); i++) {
        int found = -1;
        
        for (int j = 0; j < frames.size(); j++) {
            if (frames[j] == pages[i]) {
                found = j;
                break;
            }
        }
        
        if (found == -1) {
            if (frames.size() < capacity) {
                frames.push_back(pages[i]);
                lastUsed.push_back(i);
            } else {
                int lru = 0;
                for (int j = 1; j < frames.size(); j++) {
                    if (lastUsed[j] < lastUsed[lru]) lru = j;
                }
                frames[lru] = pages[i];
                lastUsed[lru] = i;
            }
            faults++;
            cout << "Page " << pages[i] << " caused a page fault.\n";
            cout << "\033[42m Page MISS.\033[0m\n";
        } else {
            lastUsed[found] = i;
            cout << "\033[41m Page " << pages[i] << " HIT.\033[0m\n";
        }
        showFrames(frames);
    }
    cout << "Total Page Faults = " << faults << "\n";
}

void Optimal(vector<int> pages, int capacity) {
    cout << "\n--- Optimal Page Replacement ---\n";
    vector<int> frames;
    int faults = 0;
    
    for (int i = 0; i < pages.size(); i++) {
        if (!isPresent(frames, pages[i])) {
            if (frames.size() < capacity) {
                frames.push_back(pages[i]);
            } else {
                int replace = 0, farthest = i;
                for (int j = 0; j < frames.size(); j++) {
                    int next = pages.size();
                    for (int k = i + 1; k < pages.size(); k++) {
                        if (pages[k] == frames[j]) {
                            next = k;
                            break;
                        }
                    }
                    if (next > farthest) {
                        farthest = next;
                        replace = j;
                    }
                }
                frames[replace] = pages[i];
            }
            faults++;
            cout << "Page " << pages[i] << " caused a page fault.\n";
            cout << "\033[42m Page MISS.\033[0m\n";
        } else {
            cout << "\033[41m Page " << pages[i] << " HIT.\033[0m\n";
        }
        showFrames(frames);
    }
    cout << "Total Page Faults = " << faults << "\n";
}

int main() {
    int n, capacity;
    
    cout << "Enter number of pages: ";
    cin >> n;
    
    vector<int> pages(n);
    cout << "Enter page reference string: ";
    for (int i = 0; i < n; i++) {
        cin >> pages[i];
    }
    
    cout << "Enter frame capacity: ";
    cin >> capacity;
    
    cout << "\nPage Reference String: ";
    for (int p : pages) cout << p << " ";
    cout << "\nFrame Capacity: " << capacity << "\n";
    
    int choice;
    do {
        cout << "\n--- Menu ---\n";
        cout << "1. FIFO\n";
        cout << "2. LRU\n";
        cout << "3. Optimal\n";
        cout << "4. Run All\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1: FIFO(pages, capacity); break;
            case 2: LRU(pages, capacity); break;
            case 3: Optimal(pages, capacity); break;
            case 4:
                FIFO(pages, capacity);
                LRU(pages, capacity);
                Optimal(pages, capacity);
                break;
            case 5: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while(choice != 5);
    
    return 0;
}
