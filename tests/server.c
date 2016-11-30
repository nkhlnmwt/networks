#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		/* For read and write functions */
#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include "msgformat.h"		/* This is where message protocol is
				   implemented */

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;

   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   /* IPV4, Sequenced, reliable, two-way connection based byte
      stream. Protocol value 0 */

   /* On success, a file descriptor to the socket is returned. On
      error -1 is returned and errno is set appropriately */
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   /* Common practice to make all the bits to 0 */
   portno = 5001;		/* In our program, we'll have to
				   accept this from the command line*/
   
   serv_addr.sin_family = AF_INET; /* IPv4 */
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   /* The htons() function converts the unsigned short integer
       hostshort from host byte order to network byte order. */
   
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }
   
   /* If connection is established then start communicating */
   bzero(buffer,256);

   /* Test starts */
   struct msg_prot temp;
   auxzero(&temp);
   /* Test ends */

   
   n = read(newsockfd, &temp, sizeof(temp));
   /* On success read returns the number of bytes that has been
      read */
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   printf("The message type is: %d\n", temp.msg_type);
   printf("Here is the message: %s\n",temp.msg);
   
   /* Write a response to the client */
   n = write(newsockfd,"Hi there, client! I got your message",36);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
      
   return 0;
}
