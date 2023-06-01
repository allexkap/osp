#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048



int debug_mode = 0;

char *server_ip = "127.0.0.1";
short server_port = 25552;


void pcheck(int res, char *msg) {
    if (res >= 0) return;
    perror(msg);
    exit(EXIT_FAILURE);
}


int parse_params(int argc, char **argv) {
    char *r;
    if ((r = getenv("LAB2ADDR")))  server_ip = r;
    if ((r = getenv("LAB2PORT")))  server_port = atoi(r);
    if ((r = getenv("LAB2DEBUG"))) debug_mode = 1;

    while (1) {
        switch (getopt(argc, argv, "a:p:vh")) {
            case 'a':
                server_ip = optarg;
                break;
            case 'p':
                server_port = atoi(optarg);
                break;
            case 'v':
                fprintf(stdout, "version\n");
                return 1;
            case 'h':
                fprintf(stdout, "help\n");
                return 1;
            case -1:
                return 0;
            default:
                return 1;
        }
    }
}


int main(int argc, char **argv) {

    if (parse_params(argc, argv)) {
        return 1;
    }

    int res;
    int client_socket;
    struct sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    pcheck(client_socket, "socket failed");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip);

    res = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    pcheck(res, "connect");


    char buffer[BUFFER_SIZE] = "en\n1984\n";
    res = send(client_socket, buffer, strlen(buffer), 0);
    pcheck(res, "send");
    res = recv(client_socket, buffer, sizeof(buffer), 0);
    pcheck(res, "recv");
    buffer[res] = '\0';

    printf("Received %d bytes [%s]\n", res, buffer);

    return 0;
}
