#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 30168
#define SA struct sockaddr
typedef struct{
  unsigned int lenght; //lenght of the PACKET
  char name[50]; // name of the PACKET
}PACK_INFO; // information about the data being recieved.

void recieve_data(int sockfd, char** data, PACK_INFO* recieved_data_info)
{
  char len[50],name[50];
  read(sockfd, name, 50);
  strcpy(recieved_data_info->name, name);
  read(sockfd, len, 50);
  recieved_data_info -> lenght = atoi(len);
  *data = (char* )malloc(recieved_data_info -> lenght * sizeof(char));
  read(sockfd, *data, recieved_data_info -> lenght + 1);
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
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
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
      printf("setsockopt(SO_REUSEADDR) failed");
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
      printf("connection with the server failed...\n");
      exit(0);
    }
    else
        printf("connected to the server..\n");

    PACK_INFO recieved_data_info;
    char *recieved_data;
    recieve_data(sockfd, &recieved_data, &recieved_data_info);
    printf("\nFile name: %s\nSize: %d bytes\nOutput: %s \n",recieved_data_info.name, recieved_data_info.lenght, recieved_data);
    FILE *output = fopen("output.txt","wb+");
    fwrite(recieved_data,sizeof(recieve_data),1,output);
    close(sockfd);
    return 0;
}
