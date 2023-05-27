#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024



void remove_child(int) {
    wait(NULL);
}


void pcheck(int res, char *msg) {
    if (res >= 0) return;
    perror(msg);
    exit(EXIT_FAILURE);
}


void worker(char* buffer) {
    for (int i = 0; buffer[i]; ++i) buffer[i] |= 32;
    sleep(4);
}


int main() {

    int demon_mode = 0;

    if (demon_mode && fork()) return 0;

    int res;
    int server_socket;
    struct sockaddr_in server_address, client_address;

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    pcheck(server_socket, "socket failed");

    res = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    pcheck(res, "setsockopt");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(25552);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    res = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    pcheck(res, "bind");

    signal(SIGCHLD, remove_child);


    char buffer[BUFFER_SIZE] = {};
    while (1) {
        res = recvfrom(server_socket, buffer, sizeof(buffer), 0,
            (struct sockaddr*) &client_address, &(int){sizeof(client_address)});
        if (!fork()) break;
    }

    worker(buffer);
    res = sendto(server_socket, buffer, strlen(buffer), 0,
        (struct sockaddr*) &client_address, sizeof(client_address));

    return 0;
}
