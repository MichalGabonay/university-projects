#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>

 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

int main (int argc, char *argv[] )
{
  int s, n, ch, i;
  char *port, *hostname;
  struct sockaddr_in sin; struct hostent *hptr;
  int logins[50];
  int uids[50];
  int kr=0, qw=0;
  char vstup[10];
  memset(vstup, 0, 9);
  char msg[80];
  int posArg = 0;

  int pSet=0, hSet=0, lSet=0, uSet=0;

  if ( argc < 7 ) {
    fprintf(stderr, "Bad arguments!"); /* input error: need host & port */
    return -1;
  }

  while ((ch = getopt(argc, argv, "p:h:l:u:LUGNHS")) != -1) {
             switch (ch) {
             case 'p':
                     pSet=1;
                     port=optarg;
                     break;
             case 'h':
             		hSet=1;
                    hostname=optarg;
                    break;
             case 'l':
             	lSet=1;
             	uSet=0;
                optind--;

                for( ;optind < argc && *argv[optind] != '-'; optind++){

                      logins[qw]=optind;
                      qw++;         
                }

                break;
             case 'u':
             	uSet=1;
             	lSet=0;
                optind--;
                for( ;optind < argc && *argv[optind] != '-'; optind++){
                      uids[kr]=optind; 
                      kr++;       
                }
                break;
             case 'L':
             		vstup[posArg]='L';
             		posArg++;
                 	break;
             case 'U': 
             		vstup[posArg]='U';
             		posArg++;
                  	break;
             case 'G': 
             		vstup[posArg]='G';
             		posArg++;
                  	break;
             case 'N': 
             		vstup[posArg]='N';
             		posArg++;
                  	break;
             case 'H': 
             		vstup[posArg]='H';
             		posArg++;
                  	break;
             case 'S': 
             		vstup[posArg]='S';
             		posArg++;
                  	break;
             default:
                     break;
             }
     }

   if (pSet == 0 || hSet == 0 || (uSet == 0 && lSet == 0 )){
    fprintf(stderr,"Bad arguments!");
    return -1;
  }

  char  first[160];
  memset(first, 0, 159);
  char buffer[10];

  if(uSet==1){
  	itoa(kr, buffer);
  	strcpy(first, buffer);
  	strcat(first, "u");
  	strcat(first, vstup);
  }
  else{
  	itoa(qw, buffer);
  	strcpy(first, buffer);
  	strcat(first, "l");
  	strcat(first, vstup);
  }

  //printf("%s\n", first);
 
  


  if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0) { /* create socket*/
    perror("error on socket");  /* socket error */
    return -1;
  }

  sin.sin_family = PF_INET;              /*set protocol family to Internet */
  sin.sin_port = htons(atoi(port));  /* set port no. */
  if ( (hptr =  gethostbyname(hostname) ) == NULL){
    fprintf(stderr, "gethostname error: %s", argv[1]);
    return -1;
   }

  memcpy( &sin.sin_addr, hptr->h_addr, hptr->h_length);
  if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ){
    perror("error on connect"); return -1;   /* connect error */
  }


//fprintf(stderr, "%s\n", "ok78");

  if ( write(s, first, strlen(first) +1) < 0 ) {  /* send message to server */
    perror("error on write");    return -1; /*  write error */
  }
  bzero(msg,80);
  if ( read(s, msg, sizeof(msg) ) < 0) {  /* read message from server */
      perror("Chyba pri prijmani zpravy"); return -1; /*  read error */
  }
  bzero(msg,80);



if (uSet==1)
{

	for (i = 0; i < kr; i++)
	{
		if ( write(s, argv[uids[i]], strlen(argv[uids[i]])) < 0 ) {  /* send message to server */
      		perror("error on write");    return -1; /*  write error */
	    }

     // printf("OK3\n");

	    if ( read(s, msg, sizeof(msg) ) < 0) {  /* read message from server */
	      fprintf(stderr, "error on read\n"); return -1; /*  read error */
	    }
	    printf ("%s\n",msg);  /* print message to screen */
	    bzero(msg,80);
		}
}else{
	for (i = 0; i < qw; i++)
	{
		if ( write(s, argv[logins[i]], strlen(argv[logins[i]])+1) < 0 ) {  /* send message to server */
      		perror("error on write");    return -1; /*  write error */
	    }
      //fprintf(stderr, "%s %d\n", argv[logins[i]], strlen(argv[logins[i]]));
      //printf("OK4 %d\n",qw);



      //fprintf(stderr, "%s\n", "OK5");
      //bzero(msg,80);
	    if ( read(s, msg, sizeof(msg) ) < 0) {  /* read message from server */
	      fprintf(stderr, "error on read\n"); return -1; /*  read error */
	    }
      //fprintf(stderr, "%s\n", "ok65");
	    printf ("%s\n",msg);  /* print message to screen */
	    bzero(msg,80);
		}
}  




  /* close connection, clean up socket */
  if (close(s) < 0) { 
    perror("error on close");   /* close error */
    return -1;
  }
}
