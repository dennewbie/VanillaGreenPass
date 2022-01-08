//
//  NetWrapper.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 20/10/21.
//

#include "NetWrapper.h"

int wsocket (int domain, int type, int protocol) {
    int socketFileDescriptor;
    
    if ((socketFileDescriptor = socket(domain, type, protocol)) == -1) raiseError(WSOCKET_SCOPE, SOCKET_ERROR);
    
    return socketFileDescriptor;
}

void wconnect (int socketFileDescriptor, const struct sockaddr * address, socklen_t addressLength) {
    if (connect(socketFileDescriptor, address, addressLength) == -1 && errno != EAFNOSUPPORT) raiseError(WCONNECT_SCOPE, CONNECT_ERROR);
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

void clientLog (int addressFamily, const struct sockaddr_in * clientAddress, short int logType) {
    char buffer[BUFFER_SIZE * 4];
    struct hostent * retrievedData;
    
    switch (logType) {
        case LOG_IP:
            if (inet_ntop(addressFamily, & (clientAddress->sin_addr), buffer, (socklen_t) sizeof(buffer)) == NULL) raiseError(INET_NTOP_SCOPE, INET_NTOP_ERROR);
            
            if (fprintf(stdout, "Request from host with IP %s -> port %d\n", buffer, ntohs(clientAddress->sin_port)) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
            break;
        case LOG_HOSTNAME:
            if ((retrievedData = gethostbyaddr((const void *) & (clientAddress->sin_addr), sizeof(clientAddress->sin_addr), clientAddress->sin_family)) == NULL) raiseError_herror(GET_HOST_BY_ADDR_SCOPE, GET_HOST_BY_ADDR_ERROR);
            
            if (fprintf(stdout, "Request from host with name %s -> port %d\n", retrievedData->h_name, ntohs(clientAddress->sin_port)) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
            break;
        default:
            raiseError(CLIENT_LOG_SWITCH_SCOPE, CLIENT_LOG_SWITCH_ERROR);
            break;
    }
}

ssize_t wsendto (int socketFileDescriptor, const void * buffer, size_t bufferSize, int flags, const struct sockaddr * destinationAddress, socklen_t destinationAddressLength) {
    ssize_t nWrite;
    if ((nWrite = sendto(socketFileDescriptor, buffer, bufferSize, flags, destinationAddress, destinationAddressLength)) < 0) raiseError(WSENDTO_SCOPE, WSENDTO_ERROR);
    return nWrite;
}

ssize_t wrecvfrom (int socketFileDescriptor, void * restrict buffer, size_t bufferSize, int flags, struct sockaddr * restrict sourceAddress, socklen_t * restrict sourceAddressLength) {
    ssize_t nRead;
    if ((nRead = recvfrom(socketFileDescriptor, buffer, bufferSize, flags, sourceAddress, sourceAddressLength)) < 0) raiseError(WRECVFROM_SCOPE, WRECVFROM_ERROR);
    return nRead;
}

// verifico se l'array di caratteri passato risulta essere un IP valido in formato IPv4 (o IPv6) o meno
void checkIP (char * IP_string) {
    char tempBuffer[16];
    if (((inet_pton(AF_INET, IP_string, (void *) tempBuffer)) <= 0) && ((inet_pton(AF_INET6, IP_string, (void *) tempBuffer)) <= 0)) raiseError(CHECK_IP_SCOPE, CHECK_IP_ERROR);
}
