/**************************************
*        Project to PDS - 2018        *
*            DHCP attacks             *
*      Michal Gabonay - xgabon00      *
*  Faculty of Information Technology  *
*    University of Technology Brno    *
**************************************/

#define DHCP_MAX_MTU 1500

struct msg_t
{
	char		buff[DHCP_MAX_MTU];
	uint32_t	len;
	int socket_fd;
	struct sockaddr_in address;
};

struct dhcp_lease
{
	uint32_t client_mac;
	uint32_t client_ip;

};

//dhcp_packet_t *parse_msg(char buff[], int len);
dhcp_packet_t * process_dhcp_msg(void *rcv_msg);
dhcp_packet_t *parse_msg(char *buff);
dhcp_packet_t *create_offer(dhcp_packet_t *rcv_packet);
dhcp_packet_t *create_ack(dhcp_packet_t *rcv_packet);
uint32_t alocate_ip();