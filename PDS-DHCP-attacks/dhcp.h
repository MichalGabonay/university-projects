/**************************************
*        Project to PDS - 2018        *
*            DHCP attacks             *
*      Michal Gabonay - xgabon00      *
*  Faculty of Information Technology  *
*    University of Technology Brno    *
**************************************/

#include <stdint.h>

/**** DHCP definitions ****/
#define MAX_DHCP_CHADDR_LENGTH 16
#define MAX_DHCP_SNAME_LENGTH 64
#define MAX_DHCP_FILE_LENGTH 128
#define MAX_DHCP_OPTIONS_LENGTH 312

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

#define DHCPDISCOVER 1
#define DHCPOFFER 2
#define DHCPREQUEST 3
#define DHCPACK 5

#define DHCP_OPTION_MESSAGE_TYPE 53
#define DHCP_OPTION_REQUESTED_ADDRESS 50
#define DHCP_OPTION_SERVER_IDENTIFIER 54
#define DHCP_OPTION_LEASE_TIME 51
#define DHCP_OPTION_ROUTER 3
#define DHCP_OPTION_DNS_SERVER 6
#define DHCP_OPTION_DOMAIN_NAME 15
#define DHCP_OPTION_SUBNET_MASK 1



#define ETHERNET_HARDWARE_ADDRESS 1 /* used in htype field of dhcp packet */
#define ETHERNET_HARDWARE_ADDRESS_LENGTH 6 /* length of Ethernet hardware addresses */

#define BOOTREQUEST 1
#define BOOTREPLY 2

#define DHCP_BROADCAST_FLAG 32768

#define BUFFER_SIZE 1024

typedef struct dhcp_packet_struct {
  uint8_t op;    /* packet type */
  uint8_t htype; /* type of hardware address for this machine (Ethernet, etc) */
  uint8_t hlen;  /* length of hardware address (of this machine) */
  uint8_t hops;  /* hops */
  uint32_t xid;  /* random transaction id number - chosen by this machine */
  uint16_t secs; /* seconds used in timing */
  uint16_t flags;  /* flags */
  uint32_t ciaddr; /*IP address of this machine (if we already have one) */
  uint32_t yiaddr; /* IP address of this machine (offered by the DHCP server) */
  uint32_t siaddr; /* IP address of DHCP server */
  uint32_t giaddr; /* IP address of DHCP relay */
  unsigned char
      chaddr[MAX_DHCP_CHADDR_LENGTH]; /* hardware address of this machine */
  char sname[MAX_DHCP_SNAME_LENGTH];  /* name of DHCP server */
  char file[MAX_DHCP_FILE_LENGTH];    /* boot file name (used for diskless
                                         booting?) */
  unsigned char options[MAX_DHCP_OPTIONS_LENGTH]; /* options */
  uint32_t magic_cookie;

} dhcp_packet_t;

static const unsigned char broadcast_mac[] = {0xff, 0xff, 0xff,
                                              0xff, 0xff, 0xff};

int send_discovery_packet(int socket, unsigned char *interface_mac,
                          int interface_index);
int send_request_packet(int socket, unsigned char *interface_mac,
                        int interface_index, void *requested_addr,
                        uint8_t requested_addr_len, void *server_identifier,
                        uint8_t server_id_len);

int create_dhcp_packet(unsigned char *buffer, size_t bufflen, void *data,
                       size_t dhcplen, unsigned char *interface_mac);
size_t get_dhcp_packet_size(dhcp_packet_t *msg);
unsigned short ip_csum(unsigned short *data, int words);

dhcp_packet_t *parse_dhcp_offer(uint8_t *stream, uint32_t *server_id);

int dhcp_get_option(dhcp_packet_t *dhcp, uint8_t optcode, void *buffer,
                    size_t bufsize, size_t *optlen);

void generate_mac_addr(void *buffer);
void init_sock_addr(struct sockaddr_ll *lladdr, int interface_index);
int create_server_dhcp_packet(unsigned char *buffer, size_t bufflen, void *data,
                       size_t datalen, unsigned char *interface_mac,
                       char *src_ip, uint32_t dst_ip);