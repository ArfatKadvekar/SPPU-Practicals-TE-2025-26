#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>
using namespace std;

struct Job {
    string pid;   // Process ID
    int at;       // Arrival Time
    int bt;       // Burst Time
    int pr;       // Priority
    int ct = 0;   // Completion Time
    int tat = 0;  // Turnaround Time
    int wt = 0;   // Waiting Time
    int rt = 0;   // Remaining Time
};

Job jobs[100], original[100];
int n, Time = 0;

// ---------------- INPUT ----------------
void input() {
    ifstream file("jobs.txt");
    file >> n;
    for (int i = 0; i < n; i++) {
        file >> jobs[i].pid >> jobs[i].at >> jobs[i].bt >> jobs[i].pr;
        jobs[i].rt = jobs[i].bt;
        original[i] = jobs[i];
    }
    file.close();
}

// ---------------- RESET ----------------
void reset() {
    for (int i = 0; i < n; i++) 
    jobs[i] = original[i];
    Time = 0;
}

// ---------------- METRICS ----------------
void calcMetrics(Job &j) {
    j.tat = j.ct - j.at;
    j.wt  = j.tat - j.bt;
}

// ---------------- DISPLAY ----------------
void display() {
    cout << "\n-------------------------------------------------------------\n";
    cout << "PID\tAT\tBT\tPR\tCT\tTAT\tWT\n";
    cout << "-------------------------------------------------------------\n";
    for (int i = 0; i < n; i++)
        cout << jobs[i].pid << "\t" << jobs[i].at << "\t" << jobs[i].bt << "\t"
             << jobs[i].pr << "\t" << jobs[i].ct << "\t" << jobs[i].tat << "\t" << jobs[i].wt << "\n";
}

void printGantt(string pid, int t) { cout << pid << " " << t << " "; }

// ---------------- FCFS ----------------
void fcfs() {
    cout << "\n--- FCFS ---\n";

    sort(jobs, jobs + n, [](Job &a, Job &b) { return a.at < b.at; });

    double tTAT = 0, tWT = 0;
    for (int i = 0; i < n; i++) {
        if (Time < jobs[i].at) Time = jobs[i].at;
        printGantt(jobs[i].pid, Time);
        Time += jobs[i].bt;
        jobs[i].ct = Time;
        calcMetrics(jobs[i]);
        tTAT += jobs[i].tat;
        tWT += jobs[i].wt;
        cout << Time << " ";
    }

    cout << "\nAvg.TAT = " << tTAT / n << "  Avg.WT = " << tWT / n << "\n";
    display();
}

// ---------------- SJF (Preemptive) ----------------
void sjf() {
    cout << "\n--- SJF (Preemptive) ---\n";

    sort(jobs, jobs + n, [](Job &a, Job &b) { return a.at < b.at; });

    bool done[100] = {false};
    int completed = 0, prev = -1;
    double tTAT = 0, tWT = 0;

    while (completed < n) {
        int idx = -1, minBT = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!done[i] && jobs[i].at <= Time) {
                if (jobs[i].rt < minBT || (jobs[i].rt == minBT && (idx == -1 || jobs[i].at < jobs[idx].at))) {
                    minBT = jobs[i].rt;
                    idx = i;
                }
            }
        }

        if (idx == -1) { Time++; continue; }

        if (prev != idx) {
            if (prev != -1) cout << Time << " ";
            printGantt(jobs[idx].pid, Time);
            prev = idx;
        }

        jobs[idx].rt--; Time++;

        if (jobs[idx].rt == 0) {
            jobs[idx].ct = Time;
            calcMetrics(jobs[idx]);
            tTAT += jobs[idx].tat;
            tWT += jobs[idx].wt;
            done[idx] = true;
            completed++;
            cout << Time << " ";
            prev = -1;
        }
    }

    cout << "\nAvg.TAT = " << tTAT / n << "  Avg.WT = " << tWT / n << "\n";
    display();
}

// ---------------- Round Robin ----------------
void roundRobin(int q) {
    cout << "\n--- Round Robin (q=" << q << ") ---\n";

    sort(jobs, jobs + n, [](Job &a, Job &b) { return a.at < b.at; });

    int readyQueue[200], f = 0, r = 0;
    bool ready[100] = {false};
    int completed = 0, idx = 0;
    double tTAT = 0, tWT = 0;

    // Initial arrivals
    while (idx < n && jobs[idx].at <= Time) {
        readyQueue[r++] = idx;
        ready[idx] = true;
        idx++;
    }

    while (completed < n) {
        if (f == r) { // Queue empty
            if (idx < n) {
                Time = jobs[idx].at;
                readyQueue[r++] = idx;
                ready[idx] = true;
                idx++;
            }
            continue;
        }

        int i = readyQueue[f++];
        printGantt(jobs[i].pid, Time);

        int exec = min(q, jobs[i].rt);
        jobs[i].rt -= exec;
        Time += exec;
        cout << Time << " ";

        while (idx < n && jobs[idx].at <= Time) {
            if (!ready[idx]) {
                readyQueue[r++] = idx;
                ready[idx] = true;
            }
            idx++;
        }

        if (jobs[i].rt == 0) {
            jobs[i].ct = Time;
            calcMetrics(jobs[i]);
            tTAT += jobs[i].tat;
            tWT += jobs[i].wt;
            completed++;
        } else readyQueue[r++] = i;
    }

    cout << "\nAvg.TAT = " << tTAT / n << "  Avg.WT = " << tWT / n << "\n";
    display();
}

// ---------------- Priority (Non-Preemptive) ----------------
void priority() {
    cout << "\n--- Priority (Non-Preemptive) ---\n";

    sort(jobs, jobs + n, [](Job &a, Job &b) { return a.at < b.at; });

    bool done[100] = {false};
    int completed = 0;
    double tTAT = 0, tWT = 0;

    while (completed < n) {
        int idx = -1, best = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!done[i] && jobs[i].at <= Time) {
                if (jobs[i].pr < best || (jobs[i].pr == best && (idx == -1 || jobs[i].at < jobs[idx].at))) {
                    best = jobs[i].pr;
                    idx = i;
                }
            }
        }

        if (idx == -1) { Time++; continue; }

        printGantt(jobs[idx].pid, Time);
        Time += jobs[idx].bt;
        
        jobs[idx].ct = Time;
        calcMetrics(jobs[idx]);
        tTAT += jobs[idx].tat;
        tWT += jobs[idx].wt;
        done[idx] = true;
        completed++;
        cout << Time << " ";
    }

    cout << "\nAvg.TAT = " << tTAT / n << "  Avg.WT = " << tWT / n << "\n";
    display();
}

// ---------------- MAIN ----------------
int main() {
    input();
    cout << "Original Jobs:"; display();

    fcfs(); reset();
    sjf(); reset();
    roundRobin(25); reset();
    priority();
}
