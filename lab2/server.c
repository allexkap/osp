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
#include <time.h>
#include "numwords.h"

#define BUFFER_SIZE 2048



int debug_mode = 0, demon_mode = 0;
int wait_time = 0;
char *log_path = "/tmp/lab2.log";
FILE *log_file = NULL;

char *server_ip = "127.0.0.1";
short server_port = 25552;


int stats[2] = {};



void remove_child(int) {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        ++stats[!WIFEXITED(status) || WEXITSTATUS(status)];
    }
}


const char* now();
void show_stats(int) {
    fprintf(stdout, "Successful requests %d/%d\n", stats[0], stats[0]+stats[1]);
    fprintf(log_file, "%s [%d] Successful requests %d/%d\n",
        now(), getpid(), stats[0], stats[0]+stats[1]);
    fflush(log_file);
}


void safe_exit(int) {
    if (log_file) fclose(log_file);
    exit(EXIT_SUCCESS);
}



void pcheck(int res, char *msg) {
    if (res >= 0) return;
    perror(msg);
    if (stderr = log_file) perror(msg);
    exit(EXIT_FAILURE);
}


const char* now() {
    static char buff[18];
    strftime(buff, 18, "%d.%m.%y %H:%M:%S", localtime(&(time_t){time(0)}));
    return buff;
}


int worker(char* buffer) {

    int error = 0;
    const char *result = NULL;

    int len = strlen(buffer);
    if (buffer[len-1] == '\n') buffer[len-1] = '\0';
    else error = 3;

    if (error);
    else if (!strncmp(buffer, "en\n", 3)) result = value2en(buffer+3);
    else if (!strncmp(buffer, "ru\n", 3)) result = value2ru(buffer+3);
    else error = 1;

    if (error);
    else if (result) sprintf(buffer, "%s\n", result);
    else error = 2;

    if (error) sprintf(buffer, "ERROR %d\n", error);

    sleep(wait_time);

    return error;
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
                fprintf(stdout, "Version 0.42\n");
                return 1;
            case 'h':
                fprintf(stdout, "Usage: ./prog [-w <time>] [-d] [-l <path>] [-a <ip>] [-p <port>]\n");
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

    log_file = fopen(log_path, "w");
    pcheck(!!log_file-1, "log");


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


    struct sigaction action;
    action.sa_flags = SA_RESTART;

    action.sa_handler = safe_exit;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);

    action.sa_handler = show_stats;
    sigaction(SIGUSR1, &action, NULL);

    sigaddset(&action.sa_mask, SIGUSR1);
    action.sa_handler = remove_child;
    sigaction(SIGCHLD, &action, NULL);


    char buffer[BUFFER_SIZE] = {};
    while (1) {
        res = recvfrom(server_socket, buffer, sizeof(buffer), 0,
            (struct sockaddr*) &client_address, (socklen_t*)&(int){sizeof(client_address)});
        if (!fork()) break;
    }

    pcheck(res, "recv");
    buffer[res] = '\0';

    fprintf(log_file, "%s [%d] New request\n", now(), getpid());
    fflush(log_file);

    int ret = worker(buffer);

    res = sendto(server_socket, buffer, strlen(buffer), 0,
        (struct sockaddr*) &client_address, sizeof(client_address));
    pcheck(res, "send");

    if (!ret) fprintf(log_file, "%s [%d] Success\n", now(), getpid());
    else fprintf(log_file, "%s [%d] Error %d\n", now(), getpid(), ret);
    fflush(log_file);

    fclose(log_file);

    return !!ret;
}
