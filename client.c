#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//must larger than 10 (total digit of 2^32) for reading the size of buffer
// #define BUFFSIZE 10 // for debug
#define BUFFSIZE 2048

void die(char *mess) {
    fprintf(stderr, "die: %s", mess);
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
    int echolen = send(sock, argv[3], strlen(argv[3]), 0);
    if (echolen != strlen(argv[3])) {
        die("Mismatch in number of send byte");
    }
    fprintf(stdout, "Recieved:");
    char buffer[BUFFSIZE] = {0};
    int bytes = recv(sock, buffer, BUFFSIZE-1, 0);
    int recvLen = strlen(buffer);  
    if (bytes > 0) {
        int totalLen = atoi(buffer);
        totalLen += countTotalNumOfDigit(totalLen) + 1; //add 1 for space between number and message
        fprintf(stdout, "len: %d, first %d byte: %s", totalLen, BUFFSIZE, buffer);
        totalLen -= strlen(buffer);
        while (totalLen > 0 && bytes > 0) {
            memset(buffer, 0, sizeof(buffer));
            bytes = recv(sock, buffer, BUFFSIZE-1, 0);
            totalLen -= strlen(buffer);
            recvLen += strlen(buffer);
            // if (totalLen < 4)
            //     fflush(stdout);
        }
    }
    else 
        die("Error when recv data\n");
    fprintf(stdout, "last %d byte: %s\n", BUFFSIZE, buffer);
    fprintf(stdout, "len: %d\n", recvLen);
    close(sock);
    exit(0);
}