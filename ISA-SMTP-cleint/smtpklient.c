/**************************************
*          Project to ISA             *
*            SMTP server              *
*  by Michal Gabonay - xgabon00       *
*  Faculty of Information Technology  *
*  University of Technology Brno      *
**************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <getopt.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>


#define h_addr h_addr_list[0]

typedef enum {
    s_a = 0, //S_START
    s_b, //S_EMAIL
    s_c, //S_NEXTEMAIL
    s_e, //S_MSG
    s_f, //S_END
}konecnyAutomat;

//function for reading massege from server
void readMessage(int socket)
{
	char msg[256];
	memset((char *) &msg, 0, sizeof(msg));
	sleep(0.5);
	if ( read(socket, msg, sizeof(msg) ) < 0) {  // read message from server 
	      	fprintf(stderr, "error on read\n"); exit(-1); //  read error 
	}
	printf ("%s\n",msg);  // print message to screen 
	//memset((char *) &msg, 0, sizeof(msg));
}

//function for sending massege to server
void writeMessage(char *sprava, int socket)
{
	printf("%s", sprava);
    if ( write(socket, sprava, strlen(sprava)) < 0 ) {  /* send message to server */
    	perror("error on write");    
      	exit(-1); /*  write error */
	}
}

//function that will start socket connection with server and send messages(emails)
int sendingMail(int rowCount, int mailCount, int *emStart, int *emLen, int *emMsg, 
		char *sendFileString, int *msgStart, int *msgLen, char * ipArdress, int portNum, int seconds)
{
	char request[128];
	char *emailMsg = NULL;

	//IPv4
	struct sockaddr_in  serv_addr;
	struct in_addr addr;
	struct hostent	*hptr;
	//IPv6
	struct sockaddr_in6 serv_addr6;
    struct hostent *hptr6;
    struct in6_addr addr6;

	int sockfd, n;

	//IPv4
	if(strstr(ipArdress, ".") != NULL)
	{
		// Create communication endpoint.
	    if ( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
	        perror("server: can't open stream socket");
	        return(-1);
	    } 	

	    //set proper sturcture stuff
	    serv_addr.sin_family      = PF_INET;
	    serv_addr.sin_port        = htons(portNum);   
	    inet_aton(ipArdress, &addr);
	    //get the proper form of address by gethostbyaddr
	   	if ( (hptr = gethostbyaddr(&addr, sizeof(addr), PF_INET)) == NULL) {
	        perror("gethostbyname error for host");
			return(-1);
	    }
	    //set memory right
	    memcpy(&serv_addr.sin_addr, hptr->h_addr, hptr->h_length);

	    // Connect to the server.
	    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
	        perror("client: can't connect to server");
	        return(-1);
    	}

	}
	//IPv6
	else
    {
    	//create socket in socket descriptor s
        if( (sockfd = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
        {
            perror("error on socket");
            return 1;
        }
        //set proper sturcture stuff
        bzero((char *) &serv_addr6, sizeof(serv_addr6));
        serv_addr6.sin6_flowinfo = 0;
        serv_addr6.sin6_family = PF_INET6;
        serv_addr6.sin6_port = htons(portNum);
        serv_addr6.sin6_addr = in6addr_any;
        inet_pton(AF_INET6, ipArdress, &addr6);
        //get the proper form of address by gethostbyaddr
        if( (hptr6 = gethostbyaddr(&addr6, sizeof(addr6), PF_INET6)) == NULL)
        {
            fprintf(stderr, "get host by addr error\n");
            return 1;
        }
        //set memory to be right
        memcpy(&serv_addr6.sin6_addr, hptr6->h_addr, hptr6->h_length);
        //connect to the server
        if(connect (sockfd, (struct sockaddr *)&serv_addr6, sizeof(serv_addr6)) < 0)
        {
            perror("error on connect");
            return 1;
        }

    }	

	readMessage(sockfd);
	//send EHLO message for start smtp comunication
    writeMessage("EHLO isa.local\r\n", sockfd); 
	readMessage(sockfd);

    char email[20];
    bzero(email, 20);
    char returnedMsg[256];
    bzero(returnedMsg, 256);


    for (int r = 0; r < rowCount; ++r)
    {
    	//message from mail is sending
    	writeMessage("MAIL FROM:<xgabon00@isa.local>\r\n", sockfd);
		readMessage(sockfd); //Read response from server

		//for every recipient
	    for (int i = 0; i < mailCount; ++i)
	   	{
	   		if (emMsg[i] == r+1)
	   		{
		   		for (int j = emStart[i]; j < emLen[i] + emStart[i]; ++j)
		   		{
		   			sprintf(email, "%s%c", email, sendFileString[j]); //geting email adress 			
		   		}
		   		//sending message to server, for whom is mail
		   		sprintf(request, "RCPT TO:<%s>\r\n", email);
		   		writeMessage(request, sockfd);
				memset(returnedMsg, 0, 255);
	            if((n = read(sockfd, returnedMsg, sizeof(returnedMsg))) < 0)
	            {
	                perror("error on read");
	            }
	            //check if recipient exist
	            if (strstr(returnedMsg, "250") == NULL)
	            {
	                fprintf(stderr, "Error, recipient %s probably doesn't exist!\nReturned: %s\n",email, returnedMsg);
	            }
	            printf("%s\n", returnedMsg);
			   	bzero(email, 20);
		   	}
	   	}

	   	//send message that content of mail is strarting
   		writeMessage("DATA\r\n", sockfd);
		readMessage(sockfd); //Read response from server

	    emailMsg = malloc(sizeof(char)*(msgLen[r]+1));
	    bzero(emailMsg, sizeof(char)*(msgLen[r]+1));

	    for (int k = msgStart[r]; k < msgLen[r] + msgStart[r]; ++k)
   		{
   			sprintf(emailMsg, "%s%c", emailMsg, sendFileString[k]); //geting email content
   		}

	    sprintf(request, "%s\r\n", emailMsg);
   		writeMessage(request, sockfd); //send email message
		bzero(emailMsg, sizeof(char)*(msgLen[r]+1));

   		writeMessage("\r\n.\r\n", sockfd);
		readMessage(sockfd); //Read response from server

   		writeMessage("RSET\r\n", sockfd);
		readMessage(sockfd); //Read response from server
   		
   	} 
    if( seconds > 0) //holding server connection
      sleep(seconds);

   	writeMessage("QUIT\r\n", sockfd);
	readMessage(sockfd); //Read response from server

	free(emailMsg);
	/* close connection, clean up socket */

	
	if (close(sockfd) < 0) { 
	   	perror("error on close");   /* close error */
	   	return -1;
	}

}

int main (int argc, char *argv[])
{
	char *ipArdress = "127.0.0.1";
	char *filePath = NULL;
	int reqFlag = 0;
	int ch, c, n, stop = 0, sockfd;
	int portNum = 25;
	int seconds = 0;
	int mailCount = 0, rowCount = 1, charCount = 0;
	FILE *fp = NULL;
	konecnyAutomat stavAutomatu = s_a;

	if ( argc < 2 ) { 		//nezadany ziaden argument pri spusteni
    fprintf(stderr, "Bad arguments! \n"); 
    return -1;
  	}

  	while ((ch = getopt(argc, argv, "-a:-p:-i:-w:")) != -1) {
  		switch (ch) {
  			case 'a':
	  			ipArdress = (char *) malloc(strlen(optarg) + 1);
	   			strcpy(ipArdress, optarg);  //IP adresa zmenena z povodnej 127.0.0.1
	  			break;
	  		case 'p':
	  			portNum = atoi(optarg); //port zmeneny z povodnej hodnoty 25
	  			break;
	  		case 'i':
	  			reqFlag = 1;
	  			filePath = malloc(strlen(optarg) + 1);
	  			strcpy(filePath, optarg);  //cesta k suboru na odosielanie*/
	   			
	  			break;
	  		case 'w':
	  			seconds = atoi(optarg);  // zmena dlzky cakania z 0
	  			break;
	  		default:
	            fprintf(stderr, "Bad arguments!\n");
	            abort();
        
  		}

  	}

  	if (reqFlag == 0 ) //povinny argument s cestou suboru na odoslanie nebol zadany
  	{
	  	fprintf(stderr, "Missing sending file! \n"); 
	    return -1;
	}
  
  	if (seconds > 3600) 
  	{
       fprintf(stderr, "Max value is 3600 seconds! \n");
       return -1;
  	}
  	if (seconds<0)
  	{
  	 	fprintf(stderr, "Value of time cant be under 0. \n");
       	return -1;
  	}	

	fp = fopen(filePath,"r");

	if(fp == NULL) 
   	{
      	perror("Error in opening file ");
      	return(-1);
   	}

   	//prvy priechod suborom na zistenie par dolezitych informacii pre allokaciu pamati a kontrolov spravnosti subora
   	while(1)
   	{
	    c = fgetc(fp);

	    switch(stavAutomatu){
	    	case s_a:

	    		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
   	  				c == '-' || c == '_' || c == '@' || c == '.')
      			{
      				stavAutomatu = s_b;
      				mailCount++;
      			} else {
      				fprintf(stderr, "Zle zadany mail! \n");
      				exit(-1);
      			}
      			break;
	    	case s_b:
	    		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
   	  				c == '-' || c == '_' || c == '@' || c == '.')
      			{
      				stavAutomatu = s_b;

      			} else if (c == ' ')
      				{
      					stavAutomatu = s_c;
      				}
      			else if (c == ',')
      				{
      					stavAutomatu = s_a;
      				}
      			else {
      				fprintf(stderr, "Zle zadany mail!\n");
      				exit(-1);
      			}
      			break;
	    	case s_c:
	    		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
   	  				c == '-' || c == '_' || c == '@' || c == '.')
      			{
      				stavAutomatu = s_e;
      			} else if (c == '\n')
      				{
      					stavAutomatu = s_a;
      					rowCount++;
      				}
      			else {
      				fprintf(stderr, "Zle zadana sprava!\n");
      				exit(-1);
      			}
      			break;
	    	case s_e:
	    		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
   	  				c == '-' || c == '_' || c == '@' || c == '.' || c == ' ')
      			{
      				stavAutomatu = s_e;
      			} else if (c == EOF)
      				{	    		
      					stavAutomatu = s_f;
      					break;
      				}
      			else if (c == '\n')
      				{
      					stavAutomatu = s_a;
      					rowCount++;
      				}
      			else {
      				fprintf(stderr, "Zle zadana sprava! \n");
      				exit(-1);
      			}
      			break;
	    	case s_f:
	    		stop = 1;
	    		break;

	    }
	    if (stop == 1)
	    {
	    	break;
	    }
	    charCount++;
   	}

   	//printf("pocet mailov je: %d\n", mailCount);
    //printf("pocet sprav je: %d\n", rowCount);
    //printf("pocet znakov v sendfile: %d\n", charCount);

   	//allocate new variables
   	int emStart[mailCount-1]; 
   	int emLen[mailCount-1]; 
   	int emMsg[mailCount-1]; 
   	int msgStart[rowCount-1];
   	int msgLen[rowCount-1];
   	char *sendFileString = malloc(sizeof(char)*charCount); //whole sending file 
   	int emPositin = 0;

	fclose(fp);

   	fp = fopen(filePath,"r");
   	stavAutomatu = s_a;
   	charCount = 0;
   	stop = 0;
   	mailCount=0;
   	rowCount = 1;

   	//druhy priechod, na zsikanie a ulozenie dolezitych informacii pre odoslanie mailu
	while(1)
   	{
	    c = fgetc(fp);

	    switch(stavAutomatu){
	    	case s_a:

	    		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
   	  				c == '-' || c == '_' || c == '@' || c == '.')
      			{
      				stavAutomatu = s_b;
      				emStart[emPositin] = charCount;
      				mailCount++;
      				
      			} else {
      				fprintf(stderr, "Zle zadany mail! \n");
      				exit(-1);
      			}
      			break;
	    	case s_b:
	    		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
   	  				c == '-' || c == '_' || c == '@' || c == '.')
      			{
      				stavAutomatu = s_b;

      			} else if (c == ' ')
      				{
      					emLen[emPositin] = (charCount - emStart[emPositin]);
      					emMsg[emPositin] = rowCount;
      					emPositin++;
      					stavAutomatu = s_c;
      				}
      			else if (c == ',')
      				{
      					emLen[emPositin] = (charCount - emStart[emPositin]);
      					emMsg[emPositin] = rowCount;
      					emPositin++;
      					stavAutomatu = s_a;
      				}
      			else {
      				fprintf(stderr, "Zle zadany mail!\n");
      				exit(-1);
      			}
      			break;
	    	case s_c:
	    		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
   	  				c == '-' || c == '_' || c == '@' || c == '.')
      			{
      				stavAutomatu = s_e;
      				msgStart[rowCount-1] = charCount;
      			} else if (c == '\n')
      				{
      					msgLen[rowCount-1] = 0;
      					msgStart[rowCount-1] = 0;
      					stavAutomatu = s_a;
      					rowCount++;
      				}
      			else {
      				fprintf(stderr, "Zle zadana sprava!\n");
      				exit(-1);
      			}
      			break;
	    	case s_e:
	    		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
   	  				c == '-' || c == '_' || c == '@' || c == '.' || c == ' ')
      			{
      				stavAutomatu = s_e;
      			} else if (c == EOF)
      				{	    		
      					stavAutomatu = s_f;
      					msgLen[rowCount-1] = charCount-msgStart[rowCount-1];
      					break;
      				}
      			else if (c == '\n')
      				{
      					msgLen[rowCount-1] = charCount-msgStart[rowCount-1];
      					stavAutomatu = s_a;
      					rowCount++;
      				}
      			else {
      				fprintf(stderr, "Zle zadana sprava! \n");
      				exit(-1);
      			}
      			break;
	    	case s_f:
	    		stop = 1;
	    		break;

	    }
	    if (stop == 1)
	    {
	    	break;
	    }
	    sendFileString[charCount] = c;
	    charCount++;
   	}
   	fclose(fp);

   	//doing the main thing of program
    if(sendingMail(rowCount, mailCount, emStart, emLen, emMsg, sendFileString, msgStart, msgLen, ipArdress, portNum, seconds) == -1)
    {
    	fprintf(stderr, "Error with sending mail!");
    }
   	
	//printf("KONIEC!!!!\n");
    free(filePath);
    free(sendFileString);

}