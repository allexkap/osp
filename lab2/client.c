#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024



void pcheck(int res, char *msg) {
    if (res >= 0) return;
    perror(msg);
    exit(EXIT_FAILURE);
}


int main() {

    int res;
    int client_socket;
    struct sockaddr_in server_address, client_address;

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    pcheck(client_socket, "socket failed");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(25552);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");


    char buffer[BUFFER_SIZE] = "NIT HEH";
    res = sendto(client_socket, buffer, strlen(buffer), 0,
        (struct sockaddr*) &server_address, sizeof(server_address));
    res = recvfrom(client_socket, buffer, sizeof(buffer), 0,
        (struct sockaddr*) &server_address, &(int){sizeof(server_address)});

    printf("Received %d bytes [%s]\n", res, buffer);

    return 0;
}
