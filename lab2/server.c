#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024



int debug_mode = 0, demon_mode = 0;
int wait_time = 0;
char *log_path = "/tmp/lab2.log";

char *server_ip = "127.0.0.1";
short server_port = 25552;


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


int parse_params(int argc, char **argv) {
    char *r;
    if ((r = getenv("LAB2WAIT")))    wait_time = atoi(r);
    if ((r = getenv("LAB2LOGFILE"))) log_path = r;
    if ((r = getenv("LAB2ADDR")))    server_ip = r;
    if ((r = getenv("LAB2PORT")))    server_port = atoi(r);
    if ((r = getenv("LAB2DEBUG")))   debug_mode = 1;

    while (1) {
        switch (getopt(argc, argv, "w:l:a:p:dvh")) {
            case 'w':
                wait_time = atoi(optarg);
                break;
            case 'l':
                log_path = optarg;
                break;
            case 'a':
                server_ip = optarg;
                break;
            case 'p':
                server_port = atoi(optarg);
                break;
            case 'd':
                demon_mode = 1;
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

    if (demon_mode && fork()) return 0;

    int res;
    int server_socket;
    struct sockaddr_in server_address, client_address;

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    pcheck(server_socket, "socket failed");

    res = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    pcheck(res, "setsockopt");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = inet_addr(server_ip);

    res = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    pcheck(res, "bind");

    signal(SIGCHLD, remove_child);


    char buffer[BUFFER_SIZE] = {};
    while (1) {
        res = recvfrom(server_socket, buffer, sizeof(buffer), 0,
            (struct sockaddr*) &client_address, (socklen_t*)&(int){sizeof(client_address)});
        if (!fork()) break;
    }

    worker(buffer);
    res = sendto(server_socket, buffer, strlen(buffer), 0,
        (struct sockaddr*) &client_address, sizeof(client_address));

    return 0;
}
