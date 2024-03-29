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
unsigned short server_port = 25552;


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
                fprintf(stdout, "Version 0.42\n");
                return 1;
            case 'h':
                fprintf(stdout, "Usage: ./prog [-a <ip>] [-p <port>] [number [lang]]\n");
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

    char *lang = "en", *number = "1984";
    if (optind != argc) number = argv[optind++];
    if (optind != argc) lang = argv[optind++];


    int res;
    int client_socket;
    struct sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    pcheck(client_socket, "socket");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip);

    res = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    pcheck(res, "connect");


    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s\n%s\n", lang, number);
    if (debug_mode)
        fprintf(stdout, "Sending %d bytes to %s:%hu with request %s (%s)\n",
            res, server_ip, server_port, number, lang);

    res = send(client_socket, buffer, strlen(buffer), 0);
    pcheck(res, "send");
    res = recv(client_socket, buffer, sizeof(buffer), 0);
    pcheck(res, "recv");
    buffer[res] = '\0';

    if (debug_mode)
        fprintf(stdout, "Received %d bytes from %s:%hu\nAnswer: ",
            res, server_ip, server_port);
    fprintf(stdout, "%s", buffer);

    return 0;
}
