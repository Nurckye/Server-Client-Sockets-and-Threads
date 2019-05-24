#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 30168
#define SA struct sockaddr

typedef struct{
  int sockFD;
  unsigned int lenght; //lenght of the PACKET
  char name[50]; // name of the PACKET
  char* toBeSent;
}PACK_INFO; // information about the data being sent.


void* send_data(void* ptr)
{
  PACK_INFO* sent_data_info = (PACK_INFO*)ptr;
  char buff[50];
  strcpy(buff,sent_data_info->name);
  write(sent_data_info->sockFD, buff, sizeof(buff));
  sprintf (buff, "%d", sent_data_info->lenght);
  write(sent_data_info->sockFD, buff, sizeof(buff));
  write(sent_data_info->sockFD, sent_data_info->toBeSent, sent_data_info->lenght);
  close(sent_data_info->sockFD);

  free(sent_data_info->toBeSent);
  free(ptr);
  pthread_exit(0);
}

int get_file_size(FILE* fp)
{
  fseek(fp, 0, 2);
  int aux = ftell(fp);
  fseek(fp, 0, 0);
  return aux;
}
int main()
{
  //int sockfd, connfd, len;
  int sockfd, connfd;
  socklen_t len;
  struct sockaddr_in servaddr, cli;
  sockfd = socket(AF_INET, SOCK_STREAM, 0); //creeaza socketul
  if (sockfd == -1) {
      printf("Socket creation failed, exiting...\n");
      exit(0);
  }
  else
      printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr)); // toti bitii 0
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //netlong hostlong from host byte order to network byte order.
  servaddr.sin_port = htons(PORT); //from host byte order to network byte order.

  int enable = 1; // sa refoloseasca o adresa deja folosita
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    printf("setsockopt(SO_REUSEADDR) failed");
  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
  {
      printf("socket bind failed, exiting...\n");
      exit(0);
  }
  else
      printf("Socket successfully binded..\n");
    ////
  if ((listen(sockfd, 5)) != 0) {
      printf("Listen failed...\n");
      exit(0);
  }
  else
      printf("Server listening..\n");


  FILE *fp = fopen("file.txt","rb+");
  PACK_INFO *sent_data_info;
  pthread_t thread[15];
  int i= 0;
  while (1)
  {
      connfd = accept(sockfd, (SA*)&cli, &len);
      if (connfd <= 0)
          {
            close(sockfd);
            printf("Server closed ... ");
            exit(0);
          }
      else
      {
        sent_data_info = (PACK_INFO*) malloc(sizeof(PACK_INFO));
        strcpy(sent_data_info->name,"file.txt");
        sent_data_info->lenght = get_file_size(fp);
        sent_data_info->toBeSent = (char*) malloc(sent_data_info->lenght * sizeof(char));
        fread(sent_data_info->toBeSent, sent_data_info->lenght, 1, fp);
        sent_data_info->sockFD = connfd;
        if(pthread_create(&thread[i], 0, send_data, sent_data_info))
          {
            printf("Error creating thread");
            exit(0);
          }
        pthread_detach(thread[i]);
        i++;
        if(i >= 15)
          i = 0;
      }
  }
  return 0;
}
