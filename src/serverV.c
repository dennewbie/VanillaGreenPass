//
//  serverV.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 08/01/22.
//

#include "serverV.h"

int main (int argc, char * argv[]) {
    int listenFileDescriptor, connectionFileDescriptor, enable = TRUE;
    unsigned short int serverV_Port;
    struct sockaddr_in serverV_Address, client;
//    const char * configFilePathServerV = "../conf/serverV.conf";
    const char * expectedUsageMessage = "<ServerV Port>";
    pid_t childPid;
    
    checkUsage(argc, (const char **) argv, 2, expectedUsageMessage);
    serverV_Port = (unsigned short int) strtoul(argv[1], (char **) NULL, 10);
    
    listenFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(listenFileDescriptor, SOL_SOCKET, SO_REUSEADDR, & enable, (socklen_t) sizeof(int)) < 0) raiseError(SET_SOCK_OPT_SCOPE, SET_SOCK_OPT_ERROR);
    memset((void *) & serverV_Address, 0, sizeof(serverV_Address));
    memset((void *) & client, 0, sizeof(client));
    serverV_Address.sin_family      = AF_INET;
    serverV_Address.sin_addr.s_addr = htonl(INADDR_ANY);
    serverV_Address.sin_port        = htons(serverV_Port);
    wbind(listenFileDescriptor, (struct sockaddr *) & serverV_Address, (socklen_t) sizeof(serverV_Address));
    wlisten(listenFileDescriptor, QUEUE_SIZE);
    
    while (TRUE) {
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        connectionFileDescriptor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength);
        // full READ identifier
//        if ((childPid = fork()) < 0) {
//            raiseError(FORK_SCOPE, FORK_ERROR);
//        } else if (childPid == 0) {
//            wclose(listenFileDescriptor);
//
            
            // TO DO serverV activities: mutex, thread, write on file system
//            wclose(connectionFileDescriptor);
//            exit(0);
//        }
        wclose(connectionFileDescriptor);
    }
    
    // codice mai eseguito
    wclose(listenFileDescriptor);
    exit(0);
}
