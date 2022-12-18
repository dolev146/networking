#include "myqueue.h"
#include <stdlib.h>
#include <stdio.h>

node_t *head = NULL;
node_t *tail = NULL;

void enqueue(long int *time_in_micro_seconds, int *iteration_number, int *cubic_is_0_reno_is_1)
{
    node_t *newnode = malloc(sizeof(node_t));
    newnode->time_in_micro_seconds = time_in_micro_seconds;
    newnode->iteration_number = iteration_number;
    newnode->cubic_is_0_reno_is_1 = cubic_is_0_reno_is_1;
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

// return the value
long int *dequeue()
{
    if (head == NULL)
    {
        return NULL;
    }
    else
    {
        long int *result = head->time_in_micro_seconds;
        if(*(head->cubic_is_0_reno_is_1) == 0)
        {
            printf("algorithm is: cubic iter num: %d time: %ld micro_sec \n",
          *(head->iteration_number), *(head->time_in_micro_seconds));
        }
        else if(*(head->cubic_is_0_reno_is_1) == 1){
            printf("algorithm is: reno iter num: %d time: %ld micro_sec  \n",
          *(head->iteration_number), *(head->time_in_micro_seconds));
        }

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