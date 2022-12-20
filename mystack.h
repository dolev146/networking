#ifndef MYSTACK
#define MYSTACK

struct node
{
    struct node *next;
    long int *time_in_micro_seconds;
    int *iteration_number;
    int *cubic_is_0_reno_is_1;
};

typedef struct node node_t;
void push(long *time_in_micro_seconds, int *iteration_number, int *cubic_is_0_reno_is_1);
long *pop();

extern node_t *head;

#endif // MYSTACK