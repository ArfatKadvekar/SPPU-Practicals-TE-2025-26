#include <stdio.h>
#include <stdbool.h>

#define MAX_FRAMES 10
#define MAX_PAGES 50

bool isPageinFrame(int frames[], int frame_count, int page)
{
    for (int i = 0; i < frame_count; i++)
    {
        if (frames[i] == page)
            return true;
    }
    return false;
}

void printFrames(int frames[], int frame_count)
{
    for (int i = 0; i < frame_count; i++)
        printf("+-----+ ");
    printf("\n");

    for (int i = 0; i < frame_count; i++)
        printf("|  %2d  |", frames[i]);
    printf("\n");

    for (int i = 0; i < frame_count; i++)
        printf("+-----+ ");
    printf("\n");
}



void FIFO(int pages[], int n, int frame_size)
{
    int frames[MAX_FRAMES];
    int rear = 0, front = 0, count = 0;
    int page_faults = 0;
    for (int i = 0; i < n; i++)
    {
        int page = pages[i];
        if (!isPageinFrame(frames, count, page))
        {
            page_faults++;
            if (count < frame_size)
            {
                frames[rear] = page;
                rear = (rear + 1) % frame_size;
                count++;
            }
            else
            {
                frames[front] = page;
                front = (front + 1) % frame_size;
                rear = front;
            }
            printf("Page %d caused a page fault.\n", page);
            printf("\033[42m Page MISS.\033[0m \n");
        }
        else
        {
            printf("\033[41m Page %d HIT.\033[0m \n", page);
        }
        printFrames(frames, count);
    }

    printf("Total Page Faults = %d\n", page_faults);
}

void LRU(int pages[], int n, int frame_size) {
    printf("\n--- LRU Page Replacement ---\n");
    int frames[MAX_FRAMES];
    int recent_index[MAX_FRAMES];
    int count = 0;
    int page_faults = 0;

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        int found_index = -1;

        for (int j = 0; j < count; j++) {
            if (frames[j] == page) {
                found_index = j;
                break;
            }
        }

        if (found_index == -1) {
            page_faults++;
            if (count < frame_size) {
                frames[count] = page;
                recent_index[count] = i;
                count++;
            } else {
                int lru_index = 0;
                int min_recent = recent_index[0];
                for (int j = 1; j < count; j++) {
                    if (recent_index[j] < min_recent) {
                        min_recent = recent_index[j];
                        lru_index = j;
                    }
                }
                frames[lru_index] = page;
                recent_index[lru_index] = i;
            }
            printf("Page %d caused a page fault.\n", page);
            printf("\033[42m Page MISS.\033[0m \n");
        } else {
            recent_index[found_index] = i;
            printf("\033[41m Page %d HIT.\033[0m \n", page);
        }
        printFrames(frames, count);
    }
    printf("Total Page Faults = %d\n", page_faults);
}

void Optimal(int pages[], int n, int frame_size) {
    printf("\n--- Optimal Page Replacement ---\n");
    int frames[MAX_FRAMES];
    int count = 0;
    int page_faults = 0;

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        if (!isPageinFrame(frames, count, page)) {
            page_faults++;
            if (count < frame_size) {
                frames[count++] = page;
            } else {
                int index_to_replace = -1;
                int farthest = i;

                for (int j = 0; j < count; j++) {
                    int k;
                    for (k = i + 1; k < n; k++) {
                        if (pages[k] == frames[j]) break;
                    }
                    if (k == n) {
                        index_to_replace = j;
                        break;
                    }
                    if (k > farthest) {
                        farthest = k;
                        index_to_replace = j;
                    }
                }
                if (index_to_replace == -1) index_to_replace = 0;
                frames[index_to_replace] = page;
            }
            printf("Page %d caused a page fault.\n", page);
            printf("\033[42m Page MISS.\033[0m \n");
        } else {
            printf("\033[41m Page %d HIT.\033[0m \n", page);
        }
        printFrames(frames, count);
    }
    printf("Total Page Faults = %d\n", page_faults);
}


int main() {
    int pages[] = {3,1,2,1,6,5,1,3};
    int n = sizeof(pages)/sizeof(pages[0]);
    int frame_size = 3;

    FIFO(pages, n, frame_size);
    LRU(pages, n, frame_size);
    Optimal(pages, n, frame_size);

    return 0;
}