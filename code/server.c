#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		/* For read and write functions */
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "msgformat.h"		/* This is where message protocol is
				   implemented */


int main(int argc, char *argv[])
{

  msg_init(); 			/* Do certain initialization */
  
  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  char* base64DecodeOutput;
  size_t test;
  int pid;			/* Stores processed id of child */

  if(argc < 2)
    {
      portno = 5001; 		/* Default port */
    }
  else
    {
      portno = atoi(argv[1]);
    }

  /* First call to socket() function */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  /* IPV4, Sequenced, reliable, two-way connection based byte
     stream. Protocol value 0 */

  /* On success, a file descriptor to the socket is returned. On
     error -1 is returned and errno is set appropriately */
   
  if(sockfd < 0)
    {
      perror("ERROR opening socket");
      exit(1);
    }
   
  /* Initialize socket structure */
  bzero((char *) &serv_addr, sizeof(serv_addr));
  /* Common practice to make all the bits to 0 */
   
  serv_addr.sin_family = AF_INET; /* IPv4 */
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  /* The htons() function converts the unsigned short integer
     hostshort from host byte order to network byte order. */
   
   
  /* Now bind the host address using bind() call.*/
  if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
      perror("ERROR on binding");
      exit(1);
    }
      
  /* Now start listening for the clients, here process will
   * go in sleep mode and will wait for the incoming connection
   */
   
  listen(sockfd,5);
  clilen = sizeof(cli_addr);

  while(1)			/* Loop for concurrently concurrent
				   connections */
    {
   
      /* Accept actual connection from the client */
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
	
      if (newsockfd < 0)
	{
	  perror("ERROR on accept");
	  exit(1);
	}

      pid = fork();

      if(pid < 0)
	{
	  perror("Error forking");
	  exit(1);
	}

      if(pid == 0)
	{
	  while (1)			/* Loop for multiple messages */
	    {
	      /* If connection is established then start communicating */
	      /* bzero(buffer,256); */

	      struct msg_prot temp;
	      auxzero(&temp);
   
	      /* On success read returns the number of bytes that has been
		 read */
   
	      if (read(newsockfd, &temp, sizeof(temp)) < 0)
		{
		  perror("ERROR reading from socket");
		  exit(1);		/* Is break more apt? */
		}

	      /* TODO: Display the message according to the message type */
	      printf("The message type is: %d\n", temp.msg_type);
	      switch (temp.msg_type)
		{
		case 1:
		  printf("Encoded message (ClientFD: %d): %s\n", newsockfd, temp.msg);
		  Base64Decode(temp.msg, &base64DecodeOutput, &test);
		  printf("Decoded message (ClientFD: %d): %s\n",newsockfd, base64DecodeOutput);
		  break;
		case 2:
		  perror("Recieved an ACK instead of message.");
		  break;		/* Should I exit */
		case 3:
		  printf("Client has request to stop communication.\n");
		  break;
		}

	      /* Send an ack message */
   
	      if(write(newsockfd, &ack, sizeof(ack)) < 0)
		{
		  perror("ERROR sending an ACK to the socket.\n");
		  /* exit(1); */
		}
	      else
		{
		  printf("Successfully sent an ACK to the client (ClientFD: %d).\n", newsockfd);
		}

	      if(temp.msg_type == 3)	/* Request to terminate connection */
		{
		  printf("Stopping the connection to the client.\n");
		  shutdown(newsockfd, SHUT_RDWR); /* Disallowing any
						     further
						     transmissions and
						     receptions */
		  close(newsockfd);
		  return 0;
		}
	    }
	}
    }
      
  return 0;
}
