#ifndef MYQUEUE_H_
#define MYQUEUE_H_

struct node
{
    struct node *next;
    long *value;
};

typedef struct node node_t;
void enqueue(long *value);
long* dequeue();

#endif // MYQUEUE_H_