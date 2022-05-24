#include <strings.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <unistd.h>
 #include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int main()
{
   int sockfd, newsockfd, portno;
   socklen_t clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   // struct addinfo, * First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 8080;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,10);
   clilen = sizeof(serv_addr);
   
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&serv_addr, &clilen);
	std::cout << "hhhhhhh\n";
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }
   
   /* If connection is established then start communicating */
   bzero(buffer,256);
   n = read( newsockfd,buffer,255 );
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   
   printf("Here is the message: %s\n",buffer);
   
   /* Write a response to the client */
   n = write(newsockfd,"I got your message",18);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
      
   return 0;
}