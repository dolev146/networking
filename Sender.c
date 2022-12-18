#include <netdb.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>

#define PORT 3000
#define SA struct sockaddr
// Define the maximum length for the file contents
#define MAX_FILE_LENGTH 1048576 // 1 MB in bytes

int receive_int(int *num, int fd);

int main()
{

    // Open the file in read-only mode
    FILE *file = fopen("1mb.txt", "r");
    if (!file)
    {
        perror("Error opening file");
        return 1;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Calculate the size of the first and second parts
    long first_part_size = file_size / 2;
    long second_part_size = file_size - first_part_size;

    // Read the first part of the file
    char *first_part = malloc(first_part_size);
    if (!first_part)
    {
        perror("Error allocating memory");
        fclose(file);
        return 1;
    }
    fread(first_part, 1, first_part_size, file);

    // Read the second part of the file
    char *second_part = malloc(second_part_size);
    if (!second_part)
    {
        perror("Error allocating memory");
        fclose(file);
        return 1;
    }
    fread(second_part, 1, second_part_size, file);

    // Close the file
    fclose(file);

    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        return 1;
    }
    else
    {
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        close(sockfd);
        return 1;
    }
    else
    {
        printf("connected to the server..\n");
        char file_again;
        uint32_t id1 = 1234;
        uint32_t id2 = 5678;
        uint32_t xor = id1 ^ id2;

        while (1)
        {

            // set the cc algorithm to cubic with setsockopt
            int cubic = 1;
            setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, &cubic, sizeof(cubic));

            // send the first part of the file like the way we did in reciever.c
            while (first_part_size > 0)
            {
                send(sockfd, first_part, 1, 0);
                first_part_size--;
            }

            printf("First part sent \n");
            // TODO recieve the authentication from the receiver
            // we get the xor message from the receiver

            
            // init a int pointer
            int *num = (int *)malloc(sizeof(int));
            receive_int(num, sockfd);
            printf("Received int = %d\n", *num);
            if (*num != xor)
            {
                printf("Authentication failed \n");
                break;
            }


            // set the cc algorithm to reno with setsockopt
            int reno = 0;
            setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, &reno, sizeof(reno));

            // send the second part of the file
            while(second_part_size > 0)
            {
                send(sockfd, second_part, 1, 0);
                second_part_size--;
            }

            printf("Second part sent \n");

            // prompt the user what to do: "Send the file again? (y/n)" and wait for the user's input
            // if the user enters 'y', send the file again
            // if the user enters 'n', break the loop
            printf("Send the file again? (y/n): ");

            scanf(" %c", &file_again);
            if (file_again == 'y')
            {
                // tell the receiver to send the file again
                send(sockfd, "again", 5, 0);
                continue;
            }else{
                // Send an exit message to the receiver.
                // The receiver will close the connection and exit
                send(sockfd, "exit", 4, 0);
                break;
            }

            // // prompt the user what to do: "Exit? (y/n)" and wait for the user's input
            // // if the user enters 'y', break the loop
            // // if the user enters 'n', continue the loop
            // printf("Exit? (y/n): ");

            // scanf(" %c", &exit_program);
            // if (exit_program == 'y')
            // {
            //     // Send an exit message to the receiver.
            //     // The receiver will close the connection and exit
            //     send(sockfd, "exit", 4, 0);
            //     break;
            // }

            // just send continue to the receiver
            // send(sockfd, "continue", 8, 0);
            // exit_program = 0;
            file_again = 0;
        }

    }
    close(sockfd);
    printf("Connection closed  on line 214\n");
    free(first_part);
    printf("free first part on line 216\n");
    free(second_part);
    printf("free second part on line 218\n");
    return 0;
}

// https://stackoverflow.com/questions/9140409/transfer-integer-over-a-socket-in-c
int receive_int(int *num, int fd)
{
    int32_t ret;
    char *data = (char *)&ret;
    int left = sizeof(ret);
    int rc;
    do
    {
        rc = read(fd, data, left);
        if (rc <= 0)
        { /* instead of ret */
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                // use select() or epoll() to wait for the socket to be readable again
            }
            else if (errno != EINTR)
            {
                return -1;
            }
        }
        else
        {
            data += rc;
            left -= rc;
        }
    } while (left > 0);
    *num = ntohl(ret);
    return 0;
}