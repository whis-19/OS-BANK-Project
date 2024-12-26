#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "lru.h"
#include "logger.h"
#include "common.h"

#define MAX_CONSUMERS 3


Memory memory;

void process_create(Message message) {
    sleep(2);
    int page_index = find_page(&memory, message.account_id);
    
    if (page_index != -1) {
        printf("Error: Account %d already exists.\n", message.account_id);
    } else {
        add_or_update_page(&memory, message.account_id, message.amount, "Create");
        char res[200];
        sprintf(res, "Created account %d with balance %.2f\n", message.account_id, message.amount);
        printf("%s", res);
        log_transaction(message.account_id, res);
    }
}

void process_deposit(Message message) {
    sleep(2);
    int page_index = find_page(&memory, message.account_id);
    
    if (page_index == -1) {
        printf("Error: Account %d not found.\n", message.account_id);
    } else {
        memory.pages[page_index].balance += message.amount;
        add_or_update_page(&memory, message.account_id, memory.pages[page_index].balance, "Deposit");
        char res[200];
        sprintf(res, "Deposited %.2f to account %d. New balance %.2f\n", message.amount, message.account_id, memory.pages[page_index].balance);
        printf("%s", res);
        log_transaction(message.account_id, res);
    }
}

void process_withdraw(Message message) {
    sleep(2);
    int page_index = find_page(&memory, message.account_id);
    
    if (page_index == -1) {
        printf("Error: Account %d not found.\n", message.account_id);
    } else {
        if (memory.pages[page_index].balance < message.amount) {
            printf("Error: Insufficient funds in account %d.\n", message.account_id);
        } else {
            memory.pages[page_index].balance -= message.amount;
            add_or_update_page(&memory, message.account_id, memory.pages[page_index].balance, "Withdraw");
            char res[200];
            sprintf(res, "Withdrew %.2f from account %d. New balance %.2f\n", message.amount, message.account_id, memory.pages[page_index].balance);
            printf("%s", res);
            log_transaction(message.account_id, res);
        }
    }
}

void process_check_balance(Message message) {
    sleep(2);
    int page_index = find_page(&memory, message.account_id);
    
    if (page_index == -1) {
        printf("Error: Account %d not found.\n", message.account_id);
    } else {
        //printf("Account %d balance: %.2f.\n", message.account_id, memory.pages[page_index].balance);
        message.amount = memory.pages[page_index].balance;
        key_t k;
        int mid;

        k = ftok("account", message.account_id);
        mid = msgget(k, 0666 | IPC_CREAT);
        if (mid == -1) {
            perror("Error accessing message queue");
            exit(1);
        }
        if (msgsnd(mid, &message, sizeof(Message) - sizeof(long), 0) == -1) {
            perror("Error sending message");
        }
    }
}

// Process a transaction
void process_transaction(Message message) {
    int page_index;
    if(file_exists(message.account_id) == 0) {
        add_or_update_page(&memory, message.account_id, message.amount, "Updated");
        page_index = find_page(&memory, message.account_id);
        memory.pages[page_index].balance = get_balance(message.account_id);
    }
    time_t start, end;
    page_index = find_page(&memory, message.account_id);
    if(page_index!= -1){
        pthread_mutex_lock(&(memory.pages[page_index].mutex)); 
    }
    
    start = time(0); 

    if (strcmp(message.transaction_type, "Deposit") == 0) {
        process_deposit(message);
    } else if (strcmp(message.transaction_type, "Create") == 0) {
        process_create(message);
    } else if (strcmp(message.transaction_type, "Withdraw") == 0) {
        process_withdraw(message);
    } else if (strcmp(message.transaction_type, "CheckBalance") == 0) {
        process_check_balance(message);
    } else {
        printf("Error: Invalid transaction type '%s'.\n", message.transaction_type);
    }

    end = time(0); 

    double processing_time = difftime(end, start);

    memory.total_cpu_time += processing_time;
    memory.access_count++; 

  
    memory.end_time = time(0);

    display_memory_map(&memory);
    if(page_index!= -1){
        pthread_mutex_unlock(&(memory.pages[page_index].mutex)); 
    }

}
// Consumer thread
void *consumer_thread(void *arg) {
    key_t key;
    int message_queue_id;

    key = ftok("banking_system", 65);
    message_queue_id = msgget(key, 0666 | IPC_CREAT);

    if (message_queue_id == -1) {
        perror("Error accessing message queue");
        pthread_exit(NULL);
    }

    while (1) {
        Message message;

        if (msgrcv(message_queue_id, &message, sizeof(Message) - sizeof(long), 1, IPC_NOWAIT) != -1) {
            process_transaction(message);
        }
    }
}

int main() {
    init_memory(&memory);

    pthread_t consumers[MAX_CONSUMERS];
    for (int i = 0; i < MAX_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer_thread, NULL);
    }


    for (int i = 0; i < MAX_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}