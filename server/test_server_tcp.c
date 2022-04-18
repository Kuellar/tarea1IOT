#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#define PORT 5001
#define SA struct sockaddr

/** PROTOCOL **/
#define BYTE sizeof(char)
#define ID_DEVICE_SPACE 2
#define MAC_SPACE 6
#define ID_PROTOCOL_SPACE 1
#define LENG_MSG_SPACE 2
#define DATA_1_SPACE 1
#define DATA_2_SPACE 1
#define DATA_3_SPACE 4
#define DATA_4_SPACE 1
#define DATA_5_SPACE 4
#define DATA_6_SPACE 1
#define DATA_N_SPACE 4

void get_protocol_0(unsigned char *arr, unsigned char prot, unsigned int lmesg) {
    unsigned int device = 9;
    unsigned long long int mac = 68465;
    unsigned char protocol = prot;
    unsigned int leng_mesg = lmesg;
    unsigned int data_1 = 1;
    unsigned int data_2 = 50;
    time_t data_3 = time(NULL);

    int i = 0;
    memcpy(arr, &device, BYTE * ID_DEVICE_SPACE);
    i += ID_DEVICE_SPACE;
    memcpy(&(arr[i]), &mac, BYTE * MAC_SPACE);
    i += MAC_SPACE;
    memcpy(&(arr[i]), &protocol, BYTE * ID_PROTOCOL_SPACE);
    i += ID_PROTOCOL_SPACE;
    memcpy(&(arr[i]), &leng_mesg, BYTE * LENG_MSG_SPACE);
    i += LENG_MSG_SPACE;
    memcpy(&(arr[i]), &data_1, BYTE * DATA_1_SPACE);
    i += DATA_1_SPACE;
    memcpy(&(arr[i]), &data_2, BYTE * DATA_2_SPACE);
    i += DATA_2_SPACE;
    memcpy(&(arr[i]), &data_3, BYTE * DATA_3_SPACE);
}

void get_protocol_1(unsigned char *arr, unsigned char prot, unsigned int lmesg) {
    get_protocol_0(arr, prot, lmesg);

    unsigned int data_4 = 123;
    unsigned int data_5 = 12345;
    unsigned int data_6 = 123;
    unsigned int data_7 = 12345;

    int i = 17;
    memcpy(&(arr[i]), &data_4, BYTE * DATA_4_SPACE);
    i += DATA_4_SPACE;
    memcpy(&(arr[i]), &data_5, BYTE * DATA_5_SPACE);
    i += DATA_5_SPACE;
    memcpy(&(arr[i]), &data_6, BYTE * DATA_6_SPACE);
    i += DATA_6_SPACE;
    memcpy(&(arr[i]), &data_7, BYTE * DATA_N_SPACE);
}

void get_protocol_2(unsigned char *arr, unsigned char prot, unsigned int lmesg) {
    get_protocol_1(arr, prot, lmesg);

    unsigned int data_8 = 12345;

    int i = 27;
    memcpy(&(arr[i]), &data_8, BYTE * DATA_N_SPACE);
}

void get_protocol_3(unsigned char *arr, unsigned char prot, unsigned int lmesg) {
    get_protocol_2(arr, prot, lmesg);

    unsigned int data_9 = 1234;
    unsigned int data_10 = 5678;
    unsigned int data_11 = 9123;
    unsigned int data_12 = 4567;
    unsigned int data_13 = 8901;
    unsigned int data_14 = 2345;

    int i = 31;
    memcpy(&(arr[i]), &data_9, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_10, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_11, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_12, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_13, BYTE * DATA_N_SPACE);
    i += DATA_N_SPACE;
    memcpy(&(arr[i]), &data_14, BYTE * DATA_N_SPACE);
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    //////////////////////////////////////////////////////////////
    unsigned char payload[55];
    bzero(payload, sizeof(payload));
    get_protocol_0(payload, 0, 6);

    // SEND DATA PROTOCOL 0
    write(sockfd, payload, sizeof(payload));

    // RECIEVE
    bzero(payload, sizeof(payload));
    read(sockfd, payload, BYTE);

    bzero(payload, sizeof(payload));
    get_protocol_1(payload, 1, 16);

    // SEND DATA PROTOCOL 1
    write(sockfd, payload, sizeof(payload));

    // RECIEVE
    bzero(payload, sizeof(payload));
    read(sockfd, payload, BYTE);

    bzero(payload, sizeof(payload));
    get_protocol_2(payload, 2, 20);
    // SEND DATA PROTOCOL 2
    write(sockfd, payload, sizeof(payload));

    // RECIEVE
    bzero(payload, sizeof(payload));
    read(sockfd, payload, BYTE);

    bzero(payload, sizeof(payload));
    get_protocol_3(payload, 3, 44);
    // SEND DATA PROTOCOL 3
    write(sockfd, payload, sizeof(payload));
    //////////////////////////////////////////////////////////////
    // close the socket
    close(sockfd);

    return 0;
}