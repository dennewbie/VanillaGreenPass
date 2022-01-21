//
//  NetWrapper.h
//  NetUilityWrapper
//
//  Created by Denny Caruso on 20/10/21.
//

// CHECKED
#ifndef NetWrapper_h
#define NetWrapper_h

#include "UsageUtility.h"



#define LISTEN_QUEUE_SIZE 1024

#define SOCKET_ERROR 100
#define WSOCKET_SCOPE "wsocket"

#define CONNECT_ERROR 101
#define WCONNECT_SCOPE "wconnect"

#define CLOSE_ERROR 102
#define WCLOSE_SCOPE "wclose"

#define BIND_ERROR 103
#define WBIND_SCOPE "wbind"

#define LISTEN_ERROR 104
#define WLISTEN_SCOPE "wlisten"

#define ACCEPT_ERROR 105
#define WACCEPT_SCOPE "waccept"

#define INET_PTON_ERROR 106
#define INET_PTON_SCOPE "inet_pton_conversion"

#define INET_NTOP_ERROR 107
#define INET_NTOP_SCOPE "inet_ntop_conversion"

#define SET_SOCK_OPT_ERROR 108
#define SET_SOCK_OPT_SCOPE "set_sock_options"

#define CHECK_IP_ERROR 109
#define CHECK_IP_SCOPE "checkIP"



void    raiseError          (char * errorScope,         int exitCode                                                            );
int     wsocket             (int domain,                int type,                           int protocol                        );
void    wconnect            (int socketFileDescriptor,  const struct sockaddr * address,    socklen_t addressLength             );
void    wclose              (int socketFileDescriptor                                                                           );
void    wbind               (int socketFileDescriptor,  const struct sockaddr * address,    socklen_t addressLength             );
void    wlisten             (int socketFileDescriptor,  int backlog                                                             );
int     waccept             (int socketFileDescriptor,  struct sockaddr * restrict address, socklen_t * restrict addressLength  );
void    checkIP             (char * IP_string                                                                                   );

#endif /* NetWrapper_h */
