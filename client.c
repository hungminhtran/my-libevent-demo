#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFSIZE 4096

void die(char *mess) {
    fprintf(stderr, "die: %s\n", mess);
    fflush(stdout);
    exit(1);
}

int countTotalNumOfDigit(int num) {
    int result = 0;
    while (num > 0) {
        num = num /10;
        result ++;
    }
    return result;
}

int main(char argc, char* argv[]) {
    int sock;
    struct sockaddr_in server;

    if (argc < 4) {
        fprintf(stderr, "USAGE: client <server_ip> <port> <what_you_want_to_say>\n");
        exit(1);
    }

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        die("Create socket fail");
    }

    memset(&server, 0, sizeof(server));

    //Need more error checking for these function
    server.sin_family = AF_INET; //IP V4
    server.sin_addr.s_addr = inet_addr(argv[1]); //convert ip the old ways
    inet_pton(AF_INET, argv[1], &(server.sin_addr)); //conver ip the new ways, compatibility with IP6
    server.sin_port = htons(atoi(argv[2])); //port

    struct timeval timeout;      
    timeout.tv_sec = 4;
    timeout.tv_usec = 0;

    if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        die("setsockopt failed\n");

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        char buffer[1024];
        inet_ntop(AF_INET, &(server.sin_addr), buffer, INET_ADDRSTRLEN);
        fprintf(stderr, "ip: %s", buffer);
        die("Failure to connect with server");
    }
    int temp = atoi(argv[3]);
    int echolen = send(sock, &temp, strlen(argv[3]), 0);
    // int echolen = send(sock, argv[3], strlen(argv[3]), 0);
    if (echolen != strlen(argv[3])) {
        die("Mismatch in number of send byte");
    }
    char buffer[BUFFSIZE] = {0};
    int bytes = 0;
    memset(buffer, '\0', sizeof(buffer));
    int totalLen = 0;
    while ((bytes = recv(sock, buffer, BUFFSIZE-1, 0)) > 0) {
        totalLen +=  (int)strlen(buffer);
        if (argc == 4)
            fprintf(stdout, "total len %d buffer %s buffsize %d buff len %d\n", totalLen, buffer, BUFFSIZE, (int)strlen(buffer));
        memset(buffer, 0, BUFFSIZE);
    }
    if (argc == 5 || argc == 4)
        fprintf(stdout, "\nbuffsize %d\nbuffer %s\ntotal len: %d__\n", BUFFSIZE,buffer, totalLen);
    close(sock);
    exit(0);
}