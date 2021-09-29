#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int n, status,sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[255];
    // char line[128]; //buffer to store the content read from the file
     char storage[2000]; //array to store the contents of the file
     char path[255]; //buffer to store the content read from popen()
     FILE *fp; //file pointer to open stream returned by popen()
    // FILE *file; //file pointer to the file used for reading/appending
    
     struct sockaddr_in serv_addr, cli_addr;
     
     if (argc < 2) 
     {
         fprintf(stderr,"Kindly provide a port number to establish your connection.\n");
         exit(1);
     }

     //making the TCP socket connection
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
     error("Could not open the socket successfully.");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
         error("Binding was unsuccessful.");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     //TCP connection has been built
     
     //error checking
     if (newsockfd < 0) 
          error("Connection not accepted successfully.");
// Made a temporary file but commented it out becuase fp is already a file pointer and it doesn't need a file and also because we weren't sure about it.
    /* remove("test.txt") == 0; //deleting the file if it already exists
     file = fopen("test.txt", "a"); //opening the file in append mode
*/
     while(1) //continuous loop
     {
           bzero(buffer,256); //emptying the buffer
           n = read(newsockfd,buffer,255); //reading the buffer

           if (n < 0) 
	       error("Could not read from the socket successfully.");

           printf("Client sent this command: %s\n\nResult:\n",buffer);
           
//popen() treats the command as a string and stores the results in an open stream
           fp = popen(buffer,"r"); 

           if(fp==NULL) //in case nothing is passed and saved
           {
		printf("Error: Running was unsuccessful.\n");
	   }

           while(fgets(path,255,fp)!= NULL) //reading the entire open stream
           {   
	       //printf("%s", path); //show the result on server side
	      // fwrite(path, 1, sizeof(path), file); //write the result to file
              strcat(storage,path);
           }

	//fclose(file); //closing the file

	//file = fopen("test.txt", "r"); //opening the file again in read mode

	/*if (file!=NULL)
	{

	//reads the file line by line
	while(fgets(line, sizeof(line), file)!=NULL)
	{
	strcat(storage, line); //concatenates each line to the previous content
	}

	}*/

	status = pclose(fp); //checking the status of closing the iostream
	if(status==-1) //in case the iostream does not close successfully
           {
             exit(EXIT_FAILURE);
           }

	//displaying the final, concatenated results on the client side
	n=write(newsockfd, storage, strlen(storage));

       if (n < 0) error("\nWriting to the socket was unsuccessful."); // incase of error in writing to the socket
     }

     close(newsockfd); //closing the socket
     close(sockfd);
     return 0; 
}
