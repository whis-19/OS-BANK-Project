typedef struct {
    long message_type;
    int account_id;
    char transaction_type[20];
    double amount;
    int is_customer;
} Message;
