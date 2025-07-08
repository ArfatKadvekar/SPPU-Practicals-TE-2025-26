#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class Job
{
    string PID;
    int ArrivalTime;
    int BurstTime;
    int Priority;
    int CT, TAT, WT;

public:
    Job()
    {
        PID = "";
        ArrivalTime = 0;
        BurstTime = 0;
        Priority = 0;
        CT = 0; 
        TAT=0;
        WT=0;
    }

    Job(string pid, int at, int bt, int pr)
    {
        PID = pid;
        ArrivalTime = at;
        BurstTime = bt;
        Priority = pr;
    }

    void display() const
    {
        cout << "│" << PID << "\t│" << ArrivalTime << "    │" << BurstTime << "    │" << Priority
             << "\t  │" << CT << "    │" << TAT << "     │" << WT << "    │" << endl;
    }

    friend class JobQueue;
};

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
            exit(1); // throwing exception
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

public:
    JobScheduling()
    {
        n = 0;
        jobList = nullptr;
    }

    ~JobScheduling()
    {
        delete[] jobList;
    }
    void takeInputFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Failed to open file.\n";
            exit(1);
        }

        file >> n;
        jobList = new Job[n];

        for (int i = 0; i < n; ++i) {
            string pid;
            int at, bt, pr;
            file >> pid >> at >> bt >> pr;
            jobList[i] = Job(pid, at, bt, pr);
        }

        file.close();
    }
     void displayJobs() {
        cout << "\n┌───────┬─────┬─────┬─────┬─────┬──────┬─────┐\n";
        cout << "│PID\t│AT   │BT   │PR   │CT   │TAT   │WT   │\n";
        cout << "├───────┼─────┼─────┼─────┼─────┼──────┼─────┤\n";
        for (int i = 0; i < n; ++i)
            jobList[i].display();
        cout << "└───────┴─────┴─────┴─────┴─────┴──────┴─────┘\n";
    }
  
};

int main()
{
   JobScheduling js;
    js.takeInputFromFile("jobs.txt");
    //js.FCFS();

    js.displayJobs();

    return 0;

    return 0;
}
