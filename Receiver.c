#include <stdio.h>
#include <stdio.h>
#include <sys/time.h>
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
#include <stdio.h>
#include <time.h>
#include <errno.h>

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
int send_int(int num, int fd);
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
    // recieve the first part of the file                            // will use them to check the timing
    struct timeval start_t_cubic, end_t_cubic, tval_result_cubic; // will use them to check the timing

    struct timeval start_t_reno, end_t_reno, tval_result_reno; // will use them to check the timing

    uint32_t id1 = 1234;
    uint32_t id2 = 5678;
    uint32_t xor = id1 ^ id2;

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

        gettimeofday(&start_t_cubic, NULL); // start the time

        while (num_of_bytes < BUFSIZE / 2)
        {
            recv(client_socket, client_message, 1, 0);
            num_of_bytes++;
        }

        gettimeofday(&end_t_cubic, NULL); // finish count for first part of the file

        bzero(client_message, BUFSIZE);
        timersub(&end_t_cubic, &start_t_cubic, &tval_result_cubic); // the total time cubic

        printf("Time elapsed: %ld.%06ld\n", (long int)tval_result_cubic.tv_sec, (long int)tval_result_cubic.tv_usec);
        // store the time elapsed in a variable
        long int time_elapsed_cubic = tval_result_cubic.tv_sec * 1000000 + tval_result_cubic.tv_usec;
        printf("the time elapsed in micro seconds is %ld \n", time_elapsed_cubic);

        // 6. Send back the authentication to the sender.
        // send thhe xor to the client
        printf("sending the xor to the client %d \n", xor);
        send_int(xor, client_socket);

        bzero(client_message, BUFSIZE);

        // change the algorithm to reno
        const char *cc_algo = "reno"; // the CC algorithm to use (in this case, "reno")
        check(setsockopt(server_socket, IPPROTO_TCP, TCP_CONGESTION, cc_algo, strlen(cc_algo)),
              "setsockopt failed");

        printf("Congestion control algorithm changed to %s\n", cc_algo);

        // recive the second part of the file
        gettimeofday(&start_t_reno, NULL); // start the time

        // recive a file of half mega bytes
        while (num_of_bytes < BUFSIZE)
        {
            recv(client_socket, client_message, 1, 0);
            num_of_bytes++;
        }

        gettimeofday(&end_t_reno, NULL); // finish count for first part of the file
        timersub(&end_t_reno, &start_t_reno, &tval_result_reno); // the total time reno
        printf("Time elapsed: %ld.%06ld\n", (long int)tval_result_reno.tv_sec, (long int)tval_result_reno.tv_usec);
        // store the time elapsed in a variable
        long int time_elapsed_reno = tval_result_reno.tv_sec * 1000000 + tval_result_reno.tv_usec;
        printf("the time elapsed in micro seconds is %ld \n", time_elapsed_reno);

        // calculate the average time
        long int average_time = (time_elapsed_cubic + time_elapsed_reno) / 2;
        
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
            printf("the time of the cubic is %ld micro seconds\n", time_elapsed_cubic);
            printf("the time of the reno is %ld micro seconds\n", time_elapsed_reno);
            printf("the average time is: %ld micro seconds\n", average_time);
            
            close(client_socket);
            printf("closing client socket \n");
            return;
        }
    }
}

// https://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c
// struct timeval tval_before, tval_after, tval_result;
// gettimeofday(&tval_before, NULL);
// // Some code you want to time, for example:
// sleep(1);
// gettimeofday(&tval_after, NULL);
// timersub(&tval_after, &tval_before, &tval_result);
// printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
// #include <stdio.h>
// #include <sys/time.h>

// int main() {
//   struct timeval start, end;  // Declare timeval structures

//   gettimeofday(&start, NULL);  // Get start time

//   // Do some work here...
//   for (int i = 0; i < 1000000000; i++);

//   gettimeofday(&end, NULL);  // Get end time

//   // Calculate elapsed time in milliseconds
//   double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;

//   printf("Elapsed time: %.f milliseconds\n", elapsed_ms);

//   return 0;
// }

// https://www.tutorialspoint.com/c_standard_library/c_function_difftime.htm#:~:text=The%20C%20library%20function%20double,i.e.%20(time1%20%2D%20time2).

// #include <stdio.h>
// #include <time.h>

// int main () {
//    time_t start_t, end_t;
//    double diff_t;

//    printf("Starting of the program...\n");
//    time(&start_t);

//    printf("Sleeping for 5 seconds...\n");
//    sleep(5);

//    time(&end_t);
//    diff_t = difftime(end_t, start_t);

//    printf("Execution time = %f\n", diff_t);
//    printf("Exiting of the program...\n");

//    return(0);
// }

// https://stackoverflow.com/questions/9140409/transfer-integer-over-a-socket-in-c
int send_int(int num, int fd)
{
    int32_t conv = htonl(num);
    char *data = (char *)&conv;
    int left = sizeof(conv);
    int rc;
    do
    {
        rc = write(fd, data, left);
        if (rc < 0)
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                // use select() or epoll() to wait for the socket to be writable again
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
    return 0;
}
