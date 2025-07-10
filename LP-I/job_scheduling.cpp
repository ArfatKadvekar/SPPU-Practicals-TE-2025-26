#include <iostream>
#include <string>
#include <fstream>
#include <climits> 
using namespace std;

// Forward declaration
class Job;

// Generic sort macro
#define Sort(jobList, len, func)                    \
    {                                               \
        for (int i = 1; i < len; ++i)               \
        {                                           \
            Job key = jobList[i];                   \
            int j = i - 1;                          \
                                                    \
            while (j >= 0 && func(jobList[j], key)) \
            {                                       \
                jobList[j + 1] = jobList[j];        \
                j = j - 1;                          \
            }                                       \
            jobList[j + 1] = key;                   \
        }                                           \
    }

class Job
{
public:
    string PID;
    int ArrivalTime;
    int BurstTime;
    int Priority;
    int CT, TAT, WT;
    int RemainingTime; // For preemptive algorithms

    Job()
    {
        PID = "";
        ArrivalTime = 0;
        BurstTime = 0;
        Priority = 0;
        CT = 0;
        TAT = 0;
        WT = 0;
        RemainingTime = 0;
    }

    Job(string pid, int at, int bt, int pr)
    {
        PID = pid;
        ArrivalTime = at;
        BurstTime = bt;
        Priority = pr;
        CT = 0;
        TAT = 0;
        WT = 0;
        RemainingTime = bt; // Initialize remaining time
    }
    
    Job(string pid, int at, int bt, int pr, int ct, int tat, int wt)
    {
        PID = pid;
        ArrivalTime = at;
        BurstTime = bt;
        Priority = pr;
        CT = ct;
        TAT = tat;
        WT = wt;
        RemainingTime = bt;
    }

    void display() const
    {
        cout << "│" << PID << "\t│" << ArrivalTime << "\t│" << BurstTime << "\t│" << Priority
             << "\t  │" << CT << "\t│" << TAT << "\t │" << WT << "\t│" << endl;
    }

    friend class JobQueue;
};

// Comparison functions for different sorting criteria
bool cmpAT(Job a, Job b)
{
    return a.ArrivalTime > b.ArrivalTime;
}

bool cmpBT(Job a, Job b)
{
    if (a.BurstTime == b.BurstTime)
        return a.ArrivalTime > b.ArrivalTime; // If burst time same, sort by arrival time
    return a.BurstTime > b.BurstTime;
}

bool cmpPriority(Job a, Job b)
{
    if (a.Priority == b.Priority)
        return a.ArrivalTime > b.ArrivalTime; // If priority same, sort by arrival time
    return a.Priority > b.Priority; // Assuming lower number = higher priority
}

bool cmpRemainingTime(Job a, Job b)
{
    if (a.RemainingTime == b.RemainingTime)
        return a.ArrivalTime > b.ArrivalTime;
    return a.RemainingTime > b.RemainingTime;
}

class JobQueue
{
    Job *queue;
    int front, rear, capacity;

public:
    JobQueue(int size)
    {
        front = rear = -1;
        capacity = size;
        queue = new Job[capacity];
    }

    ~JobQueue()
    {
        delete[] queue;
    }

    bool isFull()
    {
        return (rear + 1) % capacity == front;
    }

    bool isEmpty()
    {
        return front == -1;
    }

    void enqueue(Job j)
    {
        if (isFull())
        {
            cout << "Queue is Full\n";
            return;
        }

        if (isEmpty())
        {
            front = rear = 0;
        }
        else
        {
            rear = (rear + 1) % capacity;
        }

        queue[rear] = j;
    }

    Job dequeue()
    {
        if (isEmpty())
        {
            cout << "Queue is Empty!\n";
            exit(1);
        }

        Job temp = queue[front];
        if (front == rear)
        {
            front = rear = -1;
        }
        else
        {
            front = (front + 1) % capacity;
        }
        return temp;
    }

    void displayQueue()
    {
        if (isEmpty())
        {
            cout << "Queue is Empty\n";
            return;
        }

        cout << "\nPID\tAT\tBT\tPR\n";
        int i = front;
        while (true)
        {
            queue[i].display();
            if (i == rear)
                break;
            i = (i + 1) % capacity;
        }
    }

    friend class JobScheduling;
};

class JobScheduling
{
    int n;
    Job *jobList;
    Job *originalJobList; // Keep original for resetting
    int timestamp = 0;

public:
    struct SchedulingResult
    {
        double avgTAT;
        double avgWT;
    };
    
    JobScheduling()
    {
        n = 0;
        jobList = nullptr;
        originalJobList = nullptr;
    }

    ~JobScheduling()
    {
        delete[] jobList;
        delete[] originalJobList;
    }
    
    void takeInputFromFile(const string &filename)
    {
        ifstream file(filename);
        if (!file)
        {
            cerr << "Failed to open file.\n";
            exit(1);
        }

        file >> n;

        jobList = new Job[n];
        originalJobList = new Job[n];

        for (int i = 0; i < n; ++i)
        {
            string pid;
            int at, bt, pr;
            file >> pid >> at >> bt >> pr;
            jobList[i] = Job(pid, at, bt, pr);
            originalJobList[i] = Job(pid, at, bt, pr); // Keep original copy
        }

        file.close();
    }
    
    void resetJobs()
    {
        // Reset jobList to original state
        for (int i = 0; i < n; i++)
        {
            jobList[i] = originalJobList[i];
        }
        timestamp = 0;
    }
    
    void chart(Job j)
    {
        cout << "\033[41m" << j.PID << " " << "\033[0m";
    }
    
    void displayJobs()
    {
        cout << endl;
        cout << "+-------------------------------------------------------+\n";
        cout << "│PID\t│AT\t│BT\t│PR\t  │CT\t│TAT\t │WT\t│\n";
        cout << "│-------------------------------------------------------│\n";
        for (int i = 0; i < n; ++i)
            jobList[i].display();
        cout << "+-------------------------------------------------------+\n";
    }

    SchedulingResult FCFS()
    {
        cout << "FCFS: " << endl;
        Sort(jobList, n, cmpAT);

        double totalTAT = 0, totalWT = 0;

        for (int i = 0; i < n; i++)
        {
            if (timestamp < jobList[i].ArrivalTime)
                timestamp = jobList[i].ArrivalTime;

            chart(jobList[i]);
            timestamp += jobList[i].BurstTime;

            jobList[i].CT = timestamp;
            jobList[i].TAT = jobList[i].CT - jobList[i].ArrivalTime;
            jobList[i].WT = jobList[i].TAT - jobList[i].BurstTime;

            totalTAT += jobList[i].TAT;
            totalWT += jobList[i].WT;

            cout << "\033[44m" << timestamp << " " << "\033[0m";
        }
        cout << endl;

        double avgTAT = (double)totalTAT / n;
        double avgWT = (double)totalWT / n;

        cout << "Avg.TAT = " << avgTAT << "\nAvg.WT = " << avgWT << endl;

        SchedulingResult result;
        result.avgTAT = avgTAT;
        result.avgWT = avgWT;
        return result;
    }
    
SchedulingResult SJF()
    {
        cout << "SJF (Preemptive - SRTF): " << endl;
        Sort(jobList, n, cmpAT); // First sort by arrival time

        double totalTAT = 0, totalWT = 0;
        bool *completed = new bool[n];
        for (int i = 0; i < n; i++) completed[i] = false;

        int completedJobs = 0;
        int currentJob = -1;
        
        while (completedJobs < n)
        {
            int shortestJob = -1;
            int shortestTime = INT_MAX;
            
            // Find the job with shortest remaining time that has arrived and not completed
            for (int i = 0; i < n; i++)
            {
                if (!completed[i] && jobList[i].ArrivalTime <= timestamp && 
                    jobList[i].RemainingTime < shortestTime)
                {
                    shortestTime = jobList[i].RemainingTime;
                    shortestJob = i;
                }
            }
            
            if (shortestJob == -1)
            {
                timestamp++; // No job available, increment time
                continue;
            }
            
            // If we're switching to a different job, show the chart
            if (currentJob != shortestJob)
            {
                if (currentJob != -1)
                {
                    cout << "\033[44m" << timestamp << " " << "\033[0m";
                }
                chart(jobList[shortestJob]);
                currentJob = shortestJob;
            }
            
            // Execute for 1 time unit
            timestamp++;
            jobList[shortestJob].RemainingTime--;
            
            // Check if job is completed
            if (jobList[shortestJob].RemainingTime == 0)
            {
                jobList[shortestJob].CT = timestamp;
                jobList[shortestJob].TAT = jobList[shortestJob].CT - jobList[shortestJob].ArrivalTime;
                jobList[shortestJob].WT = jobList[shortestJob].TAT - jobList[shortestJob].BurstTime;
                
                totalTAT += jobList[shortestJob].TAT;
                totalWT += jobList[shortestJob].WT;
                
                completed[shortestJob] = true;
                completedJobs++;
                
                cout << "\033[44m" << timestamp << " " << "\033[0m";
                currentJob = -1; // Reset current job
            }
        }
        cout << endl;

        double avgTAT = (double)totalTAT / n;
        double avgWT = (double)totalWT / n;

        cout << "Avg.TAT = " << avgTAT << "\nAvg.WT = " << avgWT << endl;

        delete[] completed;
        
        SchedulingResult result;
        result.avgTAT = avgTAT;
        result.avgWT = avgWT;
        return result;
    }
    
    SchedulingResult RoundRobin(int quantum = 25)
    {
        cout << "Round Robin (Quantum = " << quantum << "): " << endl;
        Sort(jobList, n, cmpAT);

        JobQueue readyQueue(n * 2); // Generous size for queue
        double totalTAT = 0, totalWT = 0;
        int completedJobs = 0;
        int currentJob = 0;

        while (completedJobs < n)
        {
            // Add newly arrived jobs to ready queue
            while (currentJob < n && jobList[currentJob].ArrivalTime <= timestamp)
            {
                readyQueue.enqueue(jobList[currentJob]);
                currentJob++;
            }

            if (readyQueue.isEmpty())
            {
                timestamp++; // No job in queue, increment time
                continue;
            }

            Job current = readyQueue.dequeue();
            chart(current);
            
            int executeTime = min(quantum, current.RemainingTime);
            timestamp += executeTime;
            current.RemainingTime -= executeTime;

            cout << "\033[44m" << timestamp << " " << "\033[0m";

            if (current.RemainingTime == 0)
            {
                // Job completed
                current.CT = timestamp;
                current.TAT = current.CT - current.ArrivalTime;
                current.WT = current.TAT - current.BurstTime;
                
                totalTAT += current.TAT;
                totalWT += current.WT;
                completedJobs++;
                
                // Update original job in jobList
                for (int i = 0; i < n; i++)
                {
                    if (jobList[i].PID == current.PID)
                    {
                        jobList[i] = current;
                        break;
                    }
                }
            }
            else
            {
                // Job not completed, add back to queue after checking for new arrivals
                while (currentJob < n && jobList[currentJob].ArrivalTime <= timestamp)
                {
                    readyQueue.enqueue(jobList[currentJob]);
                    currentJob++;
                }
                readyQueue.enqueue(current);
            }
        }
        cout << endl;

        double avgTAT = (double)totalTAT / n;
        double avgWT = (double)totalWT / n;

        cout << "Avg.TAT = " << avgTAT << "\nAvg.WT = " << avgWT << endl;

        SchedulingResult result;
        result.avgTAT = avgTAT;
        result.avgWT = avgWT;
        return result;
    }
    
    SchedulingResult Priority()
    {
        cout << "Priority (Non-Preemptive): " << endl;
        Sort(jobList, n, cmpAT); // First sort by arrival time

        double totalTAT = 0, totalWT = 0;
        bool *completed = new bool[n];
        for (int i = 0; i < n; i++) completed[i] = false;

        int completedJobs = 0;
        while (completedJobs < n)
        {
            int highestPriorityJob = -1;
            int highestPriority = INT_MAX;
            
            // Find the highest priority job that has arrived and not completed
            for (int i = 0; i < n; i++)
            {
                if (!completed[i] && jobList[i].ArrivalTime <= timestamp && 
                    jobList[i].Priority < highestPriority) // Lower number = higher priority
                {
                    highestPriority = jobList[i].Priority;
                    highestPriorityJob = i;
                }
            }
            
            if (highestPriorityJob == -1)
            {
                timestamp++; // No job available, increment time
                continue;
            }
            
            // Execute the highest priority job
            chart(jobList[highestPriorityJob]);
            timestamp += jobList[highestPriorityJob].BurstTime;
            
            jobList[highestPriorityJob].CT = timestamp;
            jobList[highestPriorityJob].TAT = jobList[highestPriorityJob].CT - jobList[highestPriorityJob].ArrivalTime;
            jobList[highestPriorityJob].WT = jobList[highestPriorityJob].TAT - jobList[highestPriorityJob].BurstTime;
            
            totalTAT += jobList[highestPriorityJob].TAT;
            totalWT += jobList[highestPriorityJob].WT;
            
            completed[highestPriorityJob] = true;
            completedJobs++;
            
            cout << "\033[44m" << timestamp << " " << "\033[0m";
        }
        cout << endl;

        double avgTAT = (double)totalTAT / n;
        double avgWT = (double)totalWT / n;

        cout << "Avg.TAT = " << avgTAT << "\nAvg.WT = " << avgWT << endl;

        delete[] completed;
        
        SchedulingResult result;
        result.avgTAT = avgTAT;
        result.avgWT = avgWT;
        return result;
    }
};

int main()
{
    JobScheduling js;
    js.takeInputFromFile("jobs.txt");
    
    cout << "Original Jobs:" << endl;
    js.displayJobs();
    
    // Test FCFS
    js.FCFS();
    js.displayJobs();
    
    // Reset and test SJF
    js.resetJobs();
    js.SJF();
    js.displayJobs();
    
    // Reset and test Round Robin
    js.resetJobs();
    js.RoundRobin(25);
    js.displayJobs();
    
    // Reset and test Priority
    js.resetJobs();
    js.Priority();
    js.displayJobs();
    
    return 0;
}