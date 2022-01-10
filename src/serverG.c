//
//  serverG.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

#include "serverG.h"

int main (int argc, char * argv[]) {
    int serverV_SocketFileDescriptor, listenFileDescriptor, connectionFileDescriptor, enable = TRUE;
    struct sockaddr_in client, serverG_Address;
    const char * expectedUsageMessage = "<ServerG Port>";
    unsigned short int serverG_Port;
    pid_t childPid;
    
    checkUsage(argc, (const char **) argv, 2, expectedUsageMessage);
    serverG_Port = (unsigned short int) strtoul(argv[1], (char **) NULL, 10);
    
    // si imposta la comunicazione col clientS e clientT
    listenFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(listenFileDescriptor, SOL_SOCKET, SO_REUSEADDR, & enable, (socklen_t) sizeof(int)) < 0) raiseError(SET_SOCK_OPT_SCOPE, SET_SOCK_OPT_ERROR);
    memset((void *) & serverG_Address, 0, sizeof(serverG_Address));
    memset((void *) & client, 0, sizeof(client));
    
    serverG_Address.sin_family      = AF_INET;
    serverG_Address.sin_addr.s_addr = htonl(INADDR_ANY);
    serverG_Address.sin_port        = htons(serverG_Port);
    wbind(listenFileDescriptor, (struct sockaddr *) & serverG_Address, (socklen_t) sizeof(serverG_Address));
    wlisten(listenFileDescriptor, QUEUE_SIZE);
    
    while (TRUE) {
        unsigned short int requestIdentifier;
        ssize_t fullReadReturnValue;
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        connectionFileDescriptor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength);
        
        if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) & requestIdentifier, (size_t) sizeof(unsigned short int))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
        
        if ((childPid = fork()) < 0) {
            raiseError(FORK_SCOPE, FORK_ERROR);
        } else if (childPid == 0) {
            wclose(listenFileDescriptor);
            serverV_SocketFileDescriptor = * createConnectionWithServerV();
            
            switch (requestIdentifier) {
                case clientS_viaServerG_Sender:
                    // stuff
                    break;
                case clientT_viaServerG_Sender:
                    // stuff
                    break;
                default:
                    raiseError(INVALID_SENDER_ID_SCOPE, INVALID_SENDER_ID_ERROR);
                    break;
            }
            
            wclose(connectionFileDescriptor);
            wclose(serverV_SocketFileDescriptor);
            exit(0);
        }
        wclose(connectionFileDescriptor);
    }
    
    // codice mai eseguito
    wclose(listenFileDescriptor);
    exit(0);
}

void clientS_RequestHandler (int connectionFileDescriptor, int serverV_SocketFileDescriptor) {
    
}

void clientT_RequestHandler (int connectionFileDescriptor, int serverV_SocketFileDescriptor) {
    
}
