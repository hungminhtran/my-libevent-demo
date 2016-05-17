#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFSIZE 16

void die(char *mess) {
    perror(mess);
    exit(1);
}

int main(char argc, char* argv[]) {
    int sock;
    struct sockaddr_in server;

    if (argc != 4) {
        fprintf(stderr, "USAGE: client <server_ip> <port> <what_you_want_to_say>\n");
        exit(1);
    }

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        die("Create socket fail");
    }

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET; //IP V4
    server.sin_addr.s_addr = inet_addr(argv[1]); //IP 
    server.sin_port = htons(atoi(argv[2])); //port

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        char buffer[1024];
        inet_ntop(AF_INET, &(server.sin_addr), buffer, INET_ADDRSTRLEN);
        fprintf(stderr, "ip: %s", buffer);
        die("Failure to connect with server");
    }
    int echolen = send(sock, argv[3], strlen(argv[3]), 0);
    if (echolen != strlen(argv[3])) {
        die("Mismatch in number of send byte");
    }
    fprintf(stdout, "Recieved:");
    int recieved = 0;
    char buffer[BUFFSIZE] = {0};
    while (recieved < echolen) {
        int bytes = 0;
        if ((bytes = recv(sock, buffer, BUFFSIZE-1, 0)) < 1) {
            die("Failure to recv data from server");
        }
        recieved += bytes;
        buffer[bytes] = '\0';
        fprintf(stdout, "%s", buffer);
    }
    fprintf(stdout, "\n");
    close(sock);
    exit(0);
}