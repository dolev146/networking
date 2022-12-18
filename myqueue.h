#ifndef MYQUEUE_H_
#define MYQUEUE_H_



struct node
{
    struct node *next;
    long int *time_in_micro_seconds;
    int *iteration_number;
    int *cubic_is_0_reno_is_1;
};

typedef struct node node_t;
void enqueue(long *time_in_micro_seconds, int *iteration_number, int *cubic_is_0_reno_is_1);
long* dequeue();


extern node_t *head;
extern node_t *tail;

#endif // MYQUEUE_H_