#include "mystack.h"
#include <stdlib.h>
#include <stdio.h>

node_t *head = NULL;

void push(long *time_in_micro_seconds, int *iteration_number, int *cubic_is_0_reno_is_1)
{
    node_t *newnode = malloc(sizeof(node_t));
    newnode->time_in_micro_seconds = time_in_micro_seconds;
    newnode->iteration_number = iteration_number;
    newnode->cubic_is_0_reno_is_1 = cubic_is_0_reno_is_1;
    newnode->next = NULL;
    if (head == NULL)
    {
        head = newnode;
    }
    else
    {
        newnode->next = head;
        head = newnode;
    }
}

long *pop()
{
    if (head == NULL)
    {
        return NULL;
    }
    else
    {
        long *result = head->time_in_micro_seconds;
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
        free(temp);
        return result;
    }
}
