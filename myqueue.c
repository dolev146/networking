#include "myqueue.h"
#include <stdlib.h>

node_t *head = NULL;
node_t *tail = NULL;

void enqueue(long *value)
{
    node_t *newnode = malloc(sizeof(node_t));
    newnode->value = value;
    newnode->next = NULL;
    if (tail == NULL)
    {
        head = newnode;
    }
    else
    {
        tail->next = newnode;
    }
    tail = newnode;
}


// return the polonger to a value
long *dequeue()
{
    if (head == NULL)
    {
        return NULL;
    }
    else
    {
        long *result = head->value;
        node_t *temp = head;
        head = head->next;
        if (head == NULL)
        {
            tail = NULL;
        }
        free(temp);
        return result;
    }
}