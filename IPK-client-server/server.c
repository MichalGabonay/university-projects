#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <pwd.h>

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

 void getOut(char *msg, struct passwd info, char *answer){
  char buffer[50];
  int i;
  int LSet=1, USet=1, GSet=1, HSet=1, NSet=1, SSet=1;
  memset(answer, 0, 49);
  //memset(answer, 0, 159);
  if (strlen(msg) == 2){
    memset(answer, 0, 159);
  }

  for (i=2; i<strlen(msg); i++){
    switch (msg[i]){
      case 'L': if(LSet==1){
      	strcat(answer, info.pw_name); 
      	strcat(answer, " ");
      	LSet=0;
      	}
        break;  
      case 'U': if(USet==1){
      	itoa(info.pw_uid,buffer); 
      	strcat(answer, buffer); 
      	strcat(answer, " ");
      	USet=0;
      	}
        break;
      case 'G': if(GSet==1){
      	itoa(info.pw_gid, buffer); 
      	strcat(answer, buffer); 
      	strcat(answer, " ");
      	GSet=0;
      	}
        break;
      case 'H': if(HSet==1){
      	strcat(answer, info.pw_dir); 
      	strcat(answer, " ");
      	HSet=0;
      	}
        break;
      case 'N': if(NSet==1){
      	strcat(answer, info.pw_gecos);
      	strcat(answer, " ");
      	NSet=0;
      	}
        break;
      case 'S': if(SSet==1){
      	strcat(answer, info.pw_shell); 
      	strcat(answer, " ");
      	SSet=0;
      	}
        break;                
    }
  }
  //fprintf(stderr, "%s\n", answer);
 }

int main (int argc, char *argv[])
{
  int s, t, sinlen;
  struct sockaddr_in sin;
  int i;
  char msg[80];
  char msg2[80];
  struct hostent * hp;
  int j;
  int ch;
 char *port;
 int pSet=0;
 struct passwd info;
 char answer[160];
 memset(answer, 0, 159);
 pid_t pid;
 int pom;

  if (argc != 3) { 
    fprintf(stderr, "Bad arguments!\n"); 
    return -1;
  }

	while ((ch = getopt(argc, argv, "p:")) != -1) {
             switch (ch) {
             case 'p':
             		pSet=1;
                    port = optarg;
                    break;
             case '?':
                    fprintf(stderr, "Bad arguments!\n"); /* input error: need port no! */
   				 	return -1;
             default:
             		fprintf(stderr, "Bad arguments!\n");
                    abort();
             }
     }

   if(pSet==0){
   		fprintf(stderr, "Bad arguments!\n");
        return -1;	
   }

     

  if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0) { /* create socket*/
    fprintf(stderr, "error on socket\n");  /* socket error */
    return -1;
  }

  sin.sin_family = PF_INET;              /*set protocol family to Internet */
  sin.sin_port = htons(atoi(argv[2]));  /* set port no. */
  sin.sin_addr.s_addr  = htonl(INADDR_ANY);   /* set IP addr to any interface */
  if (bind(s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ) {
    fprintf(stderr, "error on bind\n"); return -1;  /* bind error */
  }
  if (listen(s, 5)) { 
    fprintf (stderr, "error on listen\n"); /* listen error*/
    return -1;
  }
  sinlen = sizeof(sin);

  //fprintf(stderr, "%s\n", "OK7");
  

  //if ((pid=fork())==0){
    while (1) {
    	//memset(answer, 0, 159);
      /* accepting new connection request from client,
      socket id for the new connection is returned in t */
      if ( (t = accept(s, (struct sockaddr *) &sin, &sinlen) ) < 0 ) {
        fprintf(stderr, "error on accept\n");  /* accept error */
        return -1;
      }
      
      bzero(msg,sizeof(msg));
      if ( read(t, msg, sizeof(msg) ) <0) {  /* read message from client */
        fprintf(stderr, "error on read\n");         /*  read error */
        return -1;
      }

      if ( write(t, "OK", strlen("OK") ) < 0 ) {  /* echo message back */
			printf("error on write\n");    return -1; /*  write error */
	}
      
      //fprintf(stderr, "%s\n", msg);
      pom=atoi(msg);
      //printf("%d\n", pom);
      //getchar();
      for(i = 0; i<pom; i++){
      	//fprintf(stderr, "%s\n", "som tu");



      	//bzero(msg2,sizeof(msg2));
        if ( read(t, msg2, sizeof(msg2) ) <0) {  
          printf("error on read\n");         
          return -1;
        }
        //fprintf(stderr, "%s %s\n", "dostal som login", msg2);



        if (msg[1] == 'u'){
          if (getpwuid(atoi(msg2)) == NULL){
          	strcpy(answer, "Chyba: nezname UID");
          }
          else{

            info = *getpwuid(atoi(msg2));
            getOut(msg, info, answer);
          }
        }
        else{ //if (msg[1] == 'l'){
        	//fprintf(stderr, "%s\n", "login");
          if(getpwnam(msg2) == NULL){
            strcpy(answer, "Chyba: neznamy login");
            //fprintf(stderr, "%s\n", answer);
          }
          else{
          	//fprintf(stderr, "%s\n", "hladam login");
            info = *getpwnam(msg2);
            getOut(msg, info, answer);
            //fprintf(stderr, "%s\n", answer);
          }
        }
        if ( write(t, answer, strlen(answer) ) < 0 ) {  /* echo message back */
          printf("error on write\n");    return -1; /*  write error */
        }
        memset(answer, 0, 159);
        bzero(msg2,80);
        //printf("%s\n", "som tu 66");
       }

      /* close connection, clean up sockets */
      if (close(t) < 0) { fprintf(stderr, "error on close"); return -1;} 
    } // not reach below
  //fprintf(stderr, "%s\n", "skoncil som while");
    if (close(s) < 0) { fprintf(stderr, "close"); return -1;}
    return 0;
 // }
}
  

