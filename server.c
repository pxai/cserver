#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "readConf.c"

#define SIZE 512

static int children;

int main (int argc, char *argv[]) {
        int pid;
        int serverSocket, sock_tmp;
        struct sockaddr_in serverAddress;
        struct sockaddr_in clientAddress;
        int clientAddressSize;
        struct hostent *hostEntity;
        int received;
        char buffer[SIZE];
        
        // ezarpen file kargatzeko aldagaiak
        FILE * file;
       char value[MAX];
       char param[MAX];
       char line[MAX];
        char c = ' ';
        int PORT = 31337;
        int CHILDREN = 5;
        int CHILDREN_FORK = 0;
        int counter = 0;

        children = 0;
        
        if (!(file = fopen("config.txt","r")))
       {
               printf("Error reading file: %s\n", strerror(errno));
               return 1;
       }

       printf("Ok, file opened! \n");

       while (readLine(file,line, MAX) != EOF)
       {
               if (processLine(line, MAX, param, value))
               {
                       printf("Here you have a param: %s = %s\n", param, value);
                       
                       if (!strcmp(param,"port"))        { PORT = atoi(value); }
                       if (!strcmp(param,"children"))        { CHILDREN = atoi(value); }
                       if (!strcmp(param,"children_fork"))        { CHILDREN_FORK = atoi(value); }
               }
       }

       fclose(file);
       
       
        printf("OK, ready to start server\n");

        if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0) ) == -1 ) {
                printf("Errorea creating socket: %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(PORT);
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

        if ( bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(struct sockaddr))  == -1 ) {
                printf("Error binding socket to address: %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        if ( listen(serverSocket, 10) == -1 ) {
                printf("Error trying to listen %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

       for (counter=0;counter<CHILDREN;counter++) 
                if (pid) 
                        pid = fork();
        
        children = counter;

        printf("Server> OK, server on port %i, awaiting requests.(children: %d) \n",PORT,children);
                
        for(;;) {
                
                clientAddressSize = sizeof(struct sockaddr_in);

                  if ( (sock_tmp = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientAddressSize)) == -1) {
                        printf("Error accepting connection %s\n", strerror(errno));
                        exit(EXIT_FAILURE);
                }

                printf("Server> OK, new connection. (children: %d) \n",children);

                if ( (pid = fork()) < 0 ) {
                        printf("Error on fork %s\n", strerror(errno));
                } else { children++;}
                
                if (pid == 0) {
                        close(serverSocket);

                        while ( (received = recv(sock_tmp,buffer, SIZE-1,0)) > 0 ) {
                                buffer[received] = '\0';
                                printf("Server> OK data received: %s (size: %ld) (sem:%d) \n", buffer, strlen(buffer), children);
                        }
                        printf("Server> data was cut. (sem:%d)\n", children);
                        children--;
                        exit(0);
                }
                
                close(sock_tmp);
        }
        exit(EXIT_SUCCESS);
}