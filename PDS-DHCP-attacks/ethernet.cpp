/**************************************
*        Project to PDS - 2018        *
*            DHCP attacks             *
*      Michal Gabonay - xgabon00      *
*  Faculty of Information Technology  *
*    University of Technology Brno    *
**************************************/

#include <errno.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "ethernet.h"
#include "pds-dhcpstarve.h"

/*
 * Get MAC address of given interface
 */
int get_interface_mac(int socket, char *interface_name, u_int8_t *mac,
                      size_t len) {
  struct ifreq ifs;
  int result;

  strcpy(ifs.ifr_name, interface_name);
  result = ioctl(socket, SIOCGIFHWADDR, &ifs);
  if (result != 0) {
    printf("Can not get MAC of interface %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  memcpy((void *)mac, ifs.ifr_addr.sa_data, len);
  return EXIT_SUCCESS;
}

/*
 * Get the index of the interface to send on
 */
int get_interface_index(int socket, char *interface_name) {
  struct ifreq ifs;

  memset(&ifs, 0, sizeof(struct ifreq));
  strncpy(ifs.ifr_name, interface_name, IFNAMSIZ - 1);
  if (ioctl(socket, SIOCGIFINDEX, &ifs) < 0) {
    printf("Can not get index of interface %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  return ifs.ifr_ifindex;
}