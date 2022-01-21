//
//  NetWrapper.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 20/10/21.
//

// CHECKED
#include "NetWrapper.h"

int wsocket (int domain, int type, int protocol) {
    int socketFileDescriptor;
    if ((socketFileDescriptor = socket(domain, type, protocol)) == -1) raiseError(WSOCKET_SCOPE, SOCKET_ERROR);
    return socketFileDescriptor;
}

void wconnect (int socketFileDescriptor, const struct sockaddr * address, socklen_t addressLength) {
    if (connect(socketFileDescriptor, address, addressLength) == -1) raiseError(WCONNECT_SCOPE, CONNECT_ERROR);
}

void wclose (int socketFileDescriptor) {
    if (close(socketFileDescriptor) == -1) raiseError(WCLOSE_SCOPE, CLOSE_ERROR);
}

void wbind (int socketFileDescriptor, const struct sockaddr * address, socklen_t addressLength) {
    if (bind(socketFileDescriptor, address, addressLength) == -1) raiseError(WBIND_SCOPE, BIND_ERROR);
}

void wlisten (int socketFileDescriptor, int backlog) {
    if (listen(socketFileDescriptor, backlog) == -1) raiseError(WLISTEN_SCOPE, LISTEN_ERROR);
}

int waccept (int socketFileDescriptor, struct sockaddr * restrict address, socklen_t * restrict addressLength) {
    int communicationSocketFileDescriptor;
    if ((communicationSocketFileDescriptor = accept(socketFileDescriptor, address, addressLength)) == -1) raiseError(WACCEPT_SCOPE, ACCEPT_ERROR);
    return communicationSocketFileDescriptor;
}

// verifico se l'array di caratteri passato risulta essere un IP valido in formato IPv4 o meno
void checkIP (char * IP_string) {
    char tempBuffer[16];
    if ((inet_pton(AF_INET, (const char * restrict) IP_string, (void *) tempBuffer)) <= 0) raiseError(CHECK_IP_SCOPE, CHECK_IP_ERROR);
}
