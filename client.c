#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256]; //buffer to hold the contents of the command messge
    
    //condition to check whether file name, host, and port are all provided
    if (argc < 3) 
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    //creating TCP connection using a socket
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //error checking
    if (sockfd < 0) 
        error("Unable to open the socket.");

    server = gethostbyname(argv[1]);

    if (server == NULL) //in case server is not found
    {
        fprintf(stderr,"Host not found.\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
   
//if connection could not be established
 if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("Could not connect to the socket.");

    printf("Client, type your command: "); //taking the command as user's input

    while(1)
    {
        bzero(buffer,256); //clearing the buffer
        fgets(buffer,255,stdin);

        n = write(sockfd,buffer,strlen(buffer)); //writing to the buffer

        if (n < 0) 
             error("Unable to write to the socket.");

        bzero(buffer,256);
        n = read(sockfd,buffer,255); //reading from the socket and storing in variable n

        if (n < 0) 
             error("Unable to read from the socket.");

        printf("Server sent this result: %s\n",buffer); //printing the results sent by the server
    }

    close(sockfd); //closing the socket

    return 0;
}
