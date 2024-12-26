#include <stdio.h>
#include <string.h>

#define LOG_DIR "./Logs/"

void log_transaction(const int accountId, const char *message) {
    char fileName[100];
    sprintf(fileName, "%s%d.log", LOG_DIR, accountId);
    FILE *file = fopen(fileName, "a");
    if (file == NULL) {
        return;
    }
    fprintf(file, "%s\n", message);
    fclose(file);
}

int file_exists(const int accountId) {
    char fileName[100];
    sprintf(fileName, "%s%d.log", LOG_DIR, accountId);
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        return 1;
    }
    fclose(file);
    return 0;
}

double get_balance(const int accountId) {
    char fileName[100];
    sprintf(fileName, "%s%d.log", LOG_DIR, accountId);
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Unable to open the file");
        return -1;
    }

    char line[256];
    double last_balance = -1.0;

 
    while (fgets(line, sizeof(line), file)) {
 
        char *balance_str = strstr(line, "balance ");
        if (balance_str) {
            balance_str += 8; 
            double balance = atof(balance_str);
            last_balance = balance;
        }
    }

    fclose(file);

    //printf("The last balance is: %.2f\n", last_balance);
    return last_balance;
}