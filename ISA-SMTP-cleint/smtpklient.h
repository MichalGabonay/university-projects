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

typedef enum {
    s_a = 0, //S_START
    s_b, //S_EMAIL
    s_c, //S_NEXTEMAIL
    s_e, //S_MSG
    s_f, //S_END
}konecnyAutomat;

void readMessage(int socket);
void writeMessage(char *sprava, int socket);
int sendingMail(int rowCount, int mailCount, int *emStart, int *emLen, int *emMsg, char *sendFileString, int *msgStart, int *msgLen, char * ipArdress, int portNum, int seconds);
