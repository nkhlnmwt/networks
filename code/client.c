#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "msgformat.h" 		/* This is where message protocol has
				   been implemented*/
#include <openssl/crypto.h>

int main(int argc, char *argv[])
{

  msg_init();
  
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
   
  char buffer[MSG_LEN];
  char* base64EncodeOutput;
  char *line = NULL;
  size_t len = 0;
   
  if(argc < 3)
    {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
    }
	
  portno = atoi(argv[2]);	/* Convert to integer */
   
  /* Create a socket point */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
  if(sockfd < 0)
    {
      perror("ERROR opening socket");
      exit(1);
    }
	
  server = gethostbyname(argv[1]); /* This function is
				      obselete. Anyway, we'll have to
				      input the IP address of the
				      host in dotted decimal
				      format. */
   
  if(server == NULL)
    {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
    }
   
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
   
  /* Now connect to the server */
  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR connecting");
    exit(1);
  }
   

  struct msg_prot temp;
  int val = 0;

  /* printf("Trying to send a message of size %d\n", sizeof(temp)); */

  while(1)
    {
      auxzero(&temp);
      printf("Enter type of message: ");
      fflush(stdout);
      bzero(buffer, sizeof(buffer));
      /* fgets(buffer, sizeof(buffer), stdin); */
      /* scanf("%*[^\n]%*c");	/\* For \n characters *\/ */
      scanf("%d", &val);
      temp.msg_type = val;

      if (val == 1)
	{
	  bzero(buffer, sizeof(buffer));
	  printf("Please enter message: ");

	  /* scanf("%s", &buffer); */
	  fgets(buffer, sizeof(buffer), stdin);
	  scanf("%[^\n]s", buffer);

	  Base64Encode(buffer, strlen(buffer), &base64EncodeOutput);
	  /* scanf("%*[^\n]%*c");	/\* For \n characters *\/ */
	  /* Copying message from buffer to protocol structure */
	  strncpy(temp.msg, base64EncodeOutput, MSG_LEN);
	}
   
      /* Send message to the server */
      if(write(sockfd, &temp, sizeof(temp)) < 0)
	{
	  perror("ERROR writing to socket");
	  exit(1);		/* Should I exit? */
	}
   
      /* Waiting for ACK from the server */
      auxzero(&temp);		/* Cleanup temp buffer */
     
      if(read(sockfd, &temp, sizeof(temp)) < 0)
	{
	  perror("ERROR in receiving ACK from the server ");
	  exit(1);
	}
      else
	{
	  printf("Successfully received an ACK from the server\n");
	}
      
      if(val == 3)
	{
	  break;
	}
    }
    
  return 0;
}
