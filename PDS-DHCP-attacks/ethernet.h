/**************************************
*        Project to PDS - 2018        *
*            DHCP attacks             *
*      Michal Gabonay - xgabon00      *
*  Faculty of Information Technology  *
*    University of Technology Brno    *
**************************************/

int get_interface_mac(int socket, char *interface_name, u_int8_t *mac,
                      size_t len);
int get_interface_index(int socket, char *interface_name);