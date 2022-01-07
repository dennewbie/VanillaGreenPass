//
//  NetWrapper.h
//  NetUilityWrapper
//
//  Created by Denny Caruso on 20/10/21.
//

#ifndef NetWrapper_h
#define NetWrapper_h

#include "UsageUtility.h"
// raiseError

#define QUEUE_SIZE 1024
#define LOG_IP 900
#define LOG_HOSTNAME 901


#define WSOCKET_SCOPE "wsocket"
#define SOCKET_ERROR 100

#define WCONNECT_SCOPE "wconnect"
#define CONNECT_ERROR 101

#define WCLOSE_SCOPE "wclose"
#define CLOSE_ERROR 102

#define WBIND_SCOPE "wbind"
#define BIND_ERROR 103

#define WLISTEN_SCOPE "wlisten"
#define LISTEN_ERROR 104

#define WACCEPT_SCOPE "waccept"
#define ACCEPT_ERROR 105

#define INET_PTON_SCOPE "inet_pton_conversion"
#define INET_PTON_ERROR 106

#define INET_NTOP_SCOPE "inet_ntop_conversion"
#define INET_NTOP_ERROR 107

#define SET_SOCK_OPT_SCOPE "set_sock_options"
#define SET_SOCK_OPT_ERROR 108

#define GET_HOST_BY_NAME_ERROR 109
#define GET_HOST_BY_NAME_SCOPE "gethostbyname2"

#define GET_HOST_BY_ADDR_ERROR 110
#define GET_HOST_BY_ADDR_SCOPE "gethostbyaddr"

#define ADDRESS_ASSIGNMENT_CLIENT_DNS_SCOPE "addresses = retrievedData->h_addr_list assignment in clientDNS.c"
#define ADDRESS_ASSIGNMENT_CLIENT_DNS_ERROR 111

#define CLIENT_LOG_SWITCH_SCOPE "clientLog"
#define CLIENT_LOG_SWITCH_ERROR 112

#define WSENDTO_SCOPE "wsendto"
#define WSENDTO_ERROR 113

#define WRECVFROM_SCOPE "wrecvfrom"
#define WRECVFROM_ERROR 114



void    raiseError          (char * errorScope, int exitCode);
int     wsocket             (int domain, int type, int protocol);
void    wconnect            (int socketFileDescriptor, const struct sockaddr * address, socklen_t addressLength);
void    wclose              (int socketFileDescriptor);
void    wbind               (int socketFileDescriptor, const struct sockaddr * address, socklen_t addressLength);
void    wlisten             (int socketFileDescriptor, int backlog);
int     waccept             (int socketFileDescriptor, struct sockaddr * restrict address, socklen_t * restrict addressLength);
void    clientLog           (int addressFamily, const struct sockaddr_in * clientAddress, short int logType);

ssize_t wsendto                (int socketFileDescriptor, const void * buffer, size_t bufferSize, int flags, const struct sockaddr * destinationAddress, socklen_t destinationAddressLength);
ssize_t wrecvfrom              (int socketFileDescriptor, void * restrict buffer, size_t bufferSize, int flags, struct sockaddr * restrict sourceAddress, socklen_t * restrict sourceAddressLength);


#endif /* NetWrapper_h */
