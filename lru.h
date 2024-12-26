#ifndef MEMORY_LRU_H
#define MEMORY_LRU_H

#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define MAX_MEMORY_PAGES 5 

typedef struct {
    int account_id;       
    double balance;        
    char transaction[50]; 
    int last_used;          
    time_t timestamp;       
    int waiting_time;       
    pthread_mutex_t mutex;  
} Page;

typedef struct {
    Page pages[MAX_MEMORY_PAGES];
    int current_time;
    int next_rr_index;      
    int total_waiting_time; 
    int access_count;   
    double total_cpu_time; 
    double total_time;    
    time_t start_time;    
    time_t end_time;   
} Memory;

void init_memory(Memory *memory) {
    for (int i = 0; i < MAX_MEMORY_PAGES; i++) {
        memory->pages[i].account_id = -1; 
        memory->pages[i].balance = 0.0;
        strcpy(memory->pages[i].transaction, "");
        memory->pages[i].last_used = -1;
        memory->pages[i].timestamp = 0;
        memory->pages[i].waiting_time = 0;
        pthread_mutex_init(&(memory->pages[i].mutex), NULL);
    }
    memory->current_time = 0;
    memory->next_rr_index = 0;
    memory->total_waiting_time = 0;
    memory->access_count = 0;
    memory->total_cpu_time = 0.0; 
    memory->total_time = 0.0; 
    memory->start_time = time(NULL);
    memory->end_time = 0;
}

// Find page by account ID
int find_page(Memory *memory, int account_id) {
    for (int i = 0; i < MAX_MEMORY_PAGES; i++) {
        if (memory->pages[i].account_id == account_id) {
            return i; 
        }
    }
    return -1;
}

// Find least recently used page
int find_lru_page(Memory *memory) {
    int lru_index = 0;
    for (int i = 1; i < MAX_MEMORY_PAGES; i++) {
        if (memory->pages[i].last_used < memory->pages[lru_index].last_used) {
            lru_index = i;
        }
    }
    return lru_index;
}

// Find round robin page
int find_rr_page(Memory *memory) {
    int rr_index = memory->next_rr_index;
    memory->next_rr_index = (memory->next_rr_index + 1) % MAX_MEMORY_PAGES;
    return rr_index;
}

// Add or update page
void add_or_update_page(Memory *memory, int account_id, double balance, const char *transaction) {
    int page_index = find_page(memory, account_id);

    if (page_index != -1) {
        // Update existing page
        memory->pages[page_index].balance = balance;
        strcpy(memory->pages[page_index].transaction, transaction);
        memory->pages[page_index].last_used = memory->current_time++;
        memory->pages[page_index].timestamp = time(NULL);
        printf("Updated page for account %d.\n", account_id);
    } else {
        int rr_index = find_rr_page(memory);

        // Replace RR page
        memory->pages[rr_index].account_id = account_id;
        memory->pages[rr_index].balance = balance;
        strcpy(memory->pages[rr_index].transaction, transaction);
        memory->pages[rr_index].last_used = memory->current_time++;
        memory->pages[rr_index].timestamp = time(NULL); 

        // Calculate waiting time
        memory->pages[rr_index].waiting_time = memory->current_time;
        memory->total_waiting_time += memory->pages[rr_index].waiting_time;
        memory->access_count++;
        
        printf("Replaced RR page for account %d.\n", account_id);
    }
}

// Display memory map
void display_memory_map(Memory *memory) {
    printf("\nMemory Map:\n");
    printf("Page\tAccount ID\tBalance\t\tLast Transaction\tLast Used\tTimestamp\tWaiting Time\n");
    for (int i = 0; i < MAX_MEMORY_PAGES; i++) {
        if (memory->pages[i].account_id != -1) {
            printf("%d\t%d\t\t%.2f\t\t%s\t\t%d\t\t%s\t%d\n",
                   i,
                   memory->pages[i].account_id,
                   memory->pages[i].balance,
                   memory->pages[i].transaction,
                   memory->pages[i].last_used,
                   ctime(&memory->pages[i].timestamp), 
                   memory->pages[i].waiting_time);
        } else {
            printf("%d\tEmpty\t\t-\t\t-\t\t-\t\t-\t\t-\n", i);
        }
    }
    if (memory->access_count > 0) {
        printf("Average Waiting Time: %.2f\n", (double)memory->total_waiting_time / memory->access_count);
        printf("Total CPU Time: %.2f seconds\n", memory->total_cpu_time);
        printf("Total Elapsed Time: %.2f seconds\n", difftime(memory->end_time, memory->start_time));
        printf("CPU Utilization Time: %.2f percent\n", (double)memory->total_cpu_time / difftime(memory->end_time, memory->start_time)*100);
        
    } else {
        printf("No transactions processed yet.\n");
    }
}

#endif