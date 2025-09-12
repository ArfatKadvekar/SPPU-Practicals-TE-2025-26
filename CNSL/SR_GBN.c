#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

#define MAX_FRAMES 15

void displayMenu();
int calculateWindowSize(int m, int protocol);
void simulateSelectiveRepeat(int m);
void simulateGoBackN(int m);
int getRandomAck();

int main() {
    int choice, m;
    srand(time(NULL));
    
    while(1) {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                printf("Enter m: ");
                scanf("%d", &m);
                if(m <= 0 || m > 4) {
                    printf("Invalid m! Please enter 1-4\n");
                    break;
                }
                simulateSelectiveRepeat(m);
                break;
                
            case 2:
                printf("Enter m: ");
                scanf("%d", &m);
                if(m <= 0 || m > 4) {
                    printf("Invalid m! Please enter 1-4\n");
                    break;
                }
                simulateGoBackN(m);
                break;
                
            case 3:
                printf("Exiting...\n");
                return 0;
                
            default:
                printf("Invalid choice!\n");
        }
        
        printf("\nPress Enter to continue...");
        getchar();
        getchar();
    }
    
    return 0;
}

void displayMenu() {
    printf("\n");
    printf("----------------------------\n");
    printf("1. Selective Repeat (SR)\n");
    printf("2. Go-Back-N (GBN)\n");
    printf("3. Exit\n");
    printf("----------------------------\n");
}

void simulateSelectiveRepeat(int m) {
    int window_size = calculateWindowSize(m, 1);
    int sequence_space = 1 << m;
    int num_frames;
    
    printf("\nSelective Repeat Protocol\n");
    printf("Window size: %d\n", window_size);
    printf("----------------------------\n");
    
    printf("Enter number of frames: ");
    scanf("%d", &num_frames);
    
    if(num_frames <= 0 || num_frames > MAX_FRAMES) {
        printf("Invalid number of frames!\n");
        return;
    }
    
    int frames_acked[MAX_FRAMES] = {0};
    int frames_sent[MAX_FRAMES] = {0};
    int base = 0;
    int next_seq = 0;
    int total_acks = 0;
    int round = 0;
    
    // Send initial window
    while(next_seq < window_size && next_seq < num_frames) {
        printf("Frame %d sent\n", next_seq);
        frames_sent[next_seq] = 1;
        SLEEP_MS(500);
        next_seq++;
    }
    
    // Process acknowledgments with guaranteed variety
    while(total_acks < num_frames) {
        round++;
        int actions_this_round = 0;
        
        // Create different scenarios based on round number
        for(int i = base; i < next_seq && i < num_frames; i++) {
            if(!frames_acked[i] && actions_this_round < 2) {
                
                // Force different behaviors in different rounds
                int behavior;
                if(round % 4 == 1) {
                    behavior = rand() % 100; // Normal random
                } else if(round % 4 == 2) {
                    behavior = 20; // Force some timeouts
                } else if(round % 4 == 3) {
                    behavior = 80; // Force some ACKs
                } else {
                    behavior = rand() % 50; // Mixed behavior
                }
                
                if(behavior < 35) { // Timeout scenario
                    printf("Timeout! Retransmitting Frame %d\n", i);
                    SLEEP_MS(600);
                    actions_this_round++;
                } else if(behavior < 85) { // ACK received
                    frames_acked[i] = 1;
                    total_acks++;
                    printf("ACK %d received\n", i);
                    SLEEP_MS(400);
                    actions_this_round++;
                    
                    // Slide window if base frame is acknowledged
                    if(i == base) {
                        while(base < num_frames && frames_acked[base]) {
                            base++;
                        }
                        
                        // Send new frames when window slides
                        while(next_seq < base + window_size && next_seq < num_frames) {
                            printf("Frame %d sent\n", next_seq);
                            frames_sent[next_seq] = 1;
                            SLEEP_MS(500);
                            next_seq++;
                        }
                    }
                }
            }
        }
        
        // If no actions happened, force at least one
        if(actions_this_round == 0) {
            for(int i = base; i < next_seq && i < num_frames; i++) {
                if(!frames_acked[i]) {
                    if(rand() % 2 == 0) {
                        printf("Timeout! Retransmitting Frame %d\n", i);
                        SLEEP_MS(600);
                    } else {
                        frames_acked[i] = 1;
                        total_acks++;
                        printf("ACK %d received\n", i);
                        SLEEP_MS(400);
                        
                        if(i == base) {
                            while(base < num_frames && frames_acked[base]) {
                                base++;
                            }
                            while(next_seq < base + window_size && next_seq < num_frames) {
                                printf("Frame %d sent\n", next_seq);
                                frames_sent[next_seq] = 1;
                                SLEEP_MS(500);
                                next_seq++;
                            }
                        }
                    }
                    break;
                }
            }
        }
        
        SLEEP_MS(200);
    }
    
    printf("Final window position: %d\n", base);
}

void simulateGoBackN(int m) {
    int window_size = calculateWindowSize(m, 2);
    int sequence_space = 1 << m;
    int num_frames;
    
    printf("\nGo-Back-N Protocol\n");
    printf("Window size: %d\n", window_size);
    printf("----------------------------\n");
    
    printf("Enter number of frames: ");
    scanf("%d", &num_frames);
    
    if(num_frames <= 0 || num_frames > MAX_FRAMES) {
        printf("Invalid number of frames!\n");
        return;
    }
    
    int base = 0;
    int next_seq = 0;
    
    while(base < num_frames) {
        // Send frames in current window
        while(next_seq < base + window_size && next_seq < num_frames) {
            printf("Frame %d sent\n", next_seq);
            SLEEP_MS(500);
            next_seq++;
        }
        
        SLEEP_MS(800);
        
        // Check for ACK
        if(getRandomAck() && rand() % 4 != 0) {
            printf("ACK %d received\n", base);
            SLEEP_MS(400);
            base++;
        } else {
            printf("Timeout! Retransmitting from Frame %d\n", base);
            next_seq = base;
            SLEEP_MS(600);
        }
    }
    
    printf("Final window position: %d\n", base);
}

int calculateWindowSize(int m, int protocol) {
    int sequence_space = 1 << m;
    
    if(protocol == 1) { // Selective Repeat
        return sequence_space / 2;
    } else { // Go-Back-N
        return sequence_space - 1;
    }
}

int getRandomAck() {
    return (rand() % 100) < 75;
}