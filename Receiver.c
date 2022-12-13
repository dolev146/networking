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

// 3. Receive the first part of the file.
// 4. Measure the time, it took to receive the first part.
// 5. Save the time
#define fileSize 1048576
#define SERVERPORT 3000
#define BUFSIZE 1048576
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

char client_message[BUFSIZE];
void handle_connection(int client_socket, int server_socket);
int check(int exp, const char *msg);

int main(int argc, char **argv)
{

    int server_socket, client_socket, addr_size;
    SA_IN server_addr, client_addr;

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to craete socket");

    // initialize the adress struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    check(bind(server_socket, (SA *)&server_addr, sizeof(server_addr)), "Bind Failed!");
    check(listen(server_socket, SERVER_BACKLOG), "Listen Failed!");

    while (true)
    {
        printf("Waiting for connections... \n");
        // wait for and eventually accept an incomming connection

        addr_size = sizeof(SA_IN);
        check(client_socket = accept(server_socket, (SA *)&client_addr, (socklen_t *)&addr_size), "accept failed");
        printf("connected! on socket %d \n", client_socket);

        // the the thing we want to do with the connection
        handle_connection(client_socket, server_socket);
    }
    close(server_socket);
    printf("closing the server socket");
}

int check(int exp, const char *msg)
{
    if (exp == SOCKETERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}

void handle_connection(int client_socket, int server_socket)
{
    // recieve the first part of the file
    clock_t start_cubic, end_cubic; // will use them to check the timing
    uint32_t id1 = 1234;
    uint32_t id2 = 5678;
    uint32_t xor = id1 ^ id2;
    uint32_t converted_number = htonl(xor);
    // printf("the xor is %d \n", converted_number);
    int num_of_bytes = 0;
    while (1)
    {

        bzero(client_message, BUFSIZE);

        // set the algorithm to cubic
        const char *cc = "cubic";
        if (setsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, cc, strlen(cc)) != 0)
        {
            printf("setsockopt failed \n");
            return;
        }
        printf("Congestion control algorithm changed to %s\n", cc);

        start_cubic = clock(); // start_cubic to count the time
        // recive a file of half mega bytes

        while (num_of_bytes < BUFSIZE / 2)
        {
            num_of_bytes += recv(client_socket, client_message, BUFSIZE / 2, 0);
        }
        // recv(client_socket, client_message, BUFSIZE / 2, 0);

        bzero(client_message, BUFSIZE);

        end_cubic = clock(); // finish count for first part of the file

        double differance_between_start_cubic_to_end_cubic = end_cubic - start_cubic; // the total time cubic

        // 6. Send back the authentication to the sender.
        // figure out how to send the authentication to the sender and send it! TODO
        // send the authentication to the sender
        // send thhe xor to the client
        printf("the xor is %d, sending it to the client \n", converted_number);
        if (write(client_socket, &converted_number, sizeof(converted_number)) < 0)
        {
            printf("Send failed \n");
            return;
        }

        bzero(client_message, BUFSIZE);

        // change the algorithm to reno
        const char *cc_algo = "reno"; // the CC algorithm to use (in this case, "reno")
        check(setsockopt(server_socket, IPPROTO_TCP, TCP_CONGESTION, cc_algo, strlen(cc_algo)),
              "setsockopt failed");

        printf("Congestion control algorithm changed to %s\n", cc_algo);

        // recive the second part of the file
        clock_t start_reno = clock(); // start to count the time
        // recive a file of half mega bytes
        while(num_of_bytes < BUFSIZE)
        {
            num_of_bytes += recv(client_socket, client_message, BUFSIZE / 2, 0);
        }


        recv(client_socket, client_message, BUFSIZE / 2, 0);
        clock_t end_reno = clock(); // finish count for first part of the file

        double differance_between_start_reno_to_end_reno = end_reno - start_reno; // the total time of reno

        // calculate the average time
        double average_time = (differance_between_start_cubic_to_end_cubic + differance_between_start_reno_to_end_reno) / 2;

        // if you get the exit message from the client, close the socket and exit
        recv(client_socket, client_message, 1024, 0);
        // if the client send the message "again" the server will recive the file again
        if (strcmp(client_message, "again") == 0)
        {
            continue;
        }

        if (strcmp(client_message, "exit") == 0)
        {
            // print out the times
            printf("the time of cubic is %f\n", differance_between_start_cubic_to_end_cubic);
            printf("the time of reno is %f\n", differance_between_start_reno_to_end_reno);
            printf("the average time is: %f\n", average_time);

            close(client_socket);
            printf("closing client socket \n");
            return;
        }
    }
}