#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "common.h"


int main() {
    key_t key;
    int message_queue_id;

    key = ftok("banking_system", 65);
    message_queue_id = msgget(key, 0666 | IPC_CREAT);
    if (message_queue_id == -1) {
        perror("Error accessing message queue");
        exit(1);
    }

    while (1) {
        Message message;
        message.message_type = 1; // Set message type for requests
        message.is_customer = 1;

        printf("Enter transaction details:\n");
        printf("Account ID: ");
        scanf("%d", &message.account_id);
        if (message.account_id <= 0) {
            printf("%s", "Account ID Cannot Be Negative\n");
            continue;
        }
        printf("Transaction Type (1: Create, 2: Deposit, 3: Withdraw, 4: CheckBalance): ");
        int transaction_choice;
        scanf("%d", &transaction_choice);
        
        switch (transaction_choice) {
            case 1:
                strcpy(message.transaction_type, "Create");
                printf("Initial Amount: ");
                scanf("%lf", &message.amount);
                break;
            case 2:
                strcpy(message.transaction_type, "Deposit");
                printf("Amount: ");
                scanf("%lf", &message.amount);
                if (message.amount <= 0) {
                    printf("%s", "Amount Cannot Be Negative");
                    continue;
                }
                break;
            case 3:
                strcpy(message.transaction_type, "Withdraw");
                printf("Amount: ");
                scanf("%lf", &message.amount);
                if (message.amount <= 0) {
                    printf("%s", "Amount Cannot Be Negative");
                    continue;
                }
                break;
            case 4:
                {
                    strcpy(message.transaction_type, "CheckBalance");
                    message.amount = 0.0; 
                }        
                break;
            default:
                printf("Invalid transaction type.\n");
                continue; 
        }

        if (msgsnd(message_queue_id, &message, sizeof(Message) - sizeof(long), 0) == -1) {
            perror("Error sending message");
        } else {
            if (transaction_choice == 4)
            {
                printf("Transaction request sent: Account %d, Type %s\n",
                   message.account_id, message.transaction_type);
                
                key_t k;
                int mid;

                k = ftok("account", message.account_id);
                mid = msgget(k, 0666 | IPC_CREAT);
                if (mid == -1) {
                    perror("Error accessing message queue");
                    exit(1);
                }
                msgrcv(mid, &message, sizeof(Message) - sizeof(long), 1, 0);
                printf("Account %d balance: %.2f.\n", message.account_id, message.amount);
            } else {
                printf("Transaction request sent: Account %d, Type %s, Amount %.2f\n",
                   message.account_id, message.transaction_type, message.amount);
            }
        }

        printf("Enter '0' to quit or any other key to continue: ");
        char exit_choice[10];
        scanf("%s", exit_choice);
        if (strcmp(exit_choice, "0") == 0) {
            break;
        }
    }

    // Clean up message queue
    msgctl(message_queue_id, IPC_RMID, NULL);

    return 0;
}