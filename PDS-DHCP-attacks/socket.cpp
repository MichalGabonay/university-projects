/**************************************
*        Project to PDS - 2018        *
*            DHCP attacks             *
*      Michal Gabonay - xgabon00      *
*  Faculty of Information Technology  *
*    University of Technology Brno    *
**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <sys/socket.h>

#include "socket.h"

// sending socket
int create_dhcp_send_socket() {

  int sock;
  int sockopt;

  sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

  if (sock == -1) {
    printf("Can not create dhcp socket: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  if (0 !=
      setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &sockopt, sizeof(sockopt))) {
    printf("Can not set send socket options: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  return sock;
}

// receiving socket
int create_dhcp_recv_socket() {

  int sock;
  int sockopt;
  sock = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));

  if (sock == -1) {
    printf("Can not create dhcp socket: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  if (0 !=
      setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &sockopt, sizeof(sockopt))) {
    printf("Can not set recv socket options: %s\n", strerror(errno));
    close(sock);
    return EXIT_FAILURE;
  }

  return sock;
}

int create_socket_for_server() {

  int sock = 0;
  int so_reuseaddr = 1;
  struct sockaddr_in server_address;
  
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(67);
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);

  if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    printf("Can not create dhcp socket: %s", strerror(errno));
    return EXIT_FAILURE;
  }

  if (0 != setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr))) {
    printf("Can not set socket options: %s\n", strerror(errno));
    close(sock);
    return EXIT_FAILURE;
  }

  if(bind(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
  {
    printf("Can not bind socket with the address! %s", strerror(errno));
    close(sock);
    return EXIT_FAILURE;
  }
  return sock;
}