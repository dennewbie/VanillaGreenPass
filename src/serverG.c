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
    unsigned short int serverG_Port, requestIdentifier;
    pid_t childPid;
    
    checkUsage(argc, (const char **) argv, SERVER_G_ARGS_NO, expectedUsageMessage);
    serverG_Port = (unsigned short int) strtoul((const char * restrict) argv[1], (char ** restrict) NULL, 10);
    if (serverG_Port == 0 && (errno == EINVAL || errno == ERANGE)) raiseError(STRTOUL_SCOPE, STRTOUL_ERROR);
    
    // si imposta la comunicazione col clientS e clientT
    listenFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(listenFileDescriptor, SOL_SOCKET, SO_REUSEADDR, & enable, (socklen_t) sizeof(int)) == -1) raiseError(SET_SOCK_OPT_SCOPE, SET_SOCK_OPT_ERROR);
    memset((void *) & serverG_Address, 0, sizeof(serverG_Address));
    memset((void *) & client, 0, sizeof(client));
    
    serverG_Address.sin_family      = AF_INET;
    serverG_Address.sin_addr.s_addr = htonl(INADDR_ANY);
    serverG_Address.sin_port        = htons(serverG_Port);
    wbind(listenFileDescriptor, (struct sockaddr *) & serverG_Address, (socklen_t) sizeof(serverG_Address));
    wlisten(listenFileDescriptor, LISTEN_QUEUE_SIZE * LISTEN_QUEUE_SIZE);
    
    while (TRUE) {
        ssize_t fullReadReturnValue;
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        connectionFileDescriptor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength);
        if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) & requestIdentifier, sizeof(requestIdentifier))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
        
        if ((childPid = fork()) == -1) {
            raiseError(FORK_SCOPE, FORK_ERROR);
        } else if (childPid == 0) {
            wclose(listenFileDescriptor);
            serverV_SocketFileDescriptor = createConnectionWithServerV(configFilePathServerG);
            
            switch (requestIdentifier) {
                case clientS_viaServerG_Sender:
                    clientS_RequestHandler(connectionFileDescriptor, serverV_SocketFileDescriptor);
                    break;
                case clientT_viaServerG_Sender:
                    clientT_RequestHandler(connectionFileDescriptor, serverV_SocketFileDescriptor);
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
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    unsigned short int clientS_viaServerG_SenderID = clientS_viaServerG_Sender;
    serverG_ReplyToClientS * newServerG_Reply = (serverG_ReplyToClientS *) calloc(1, sizeof(serverG_ReplyToClientS));
    serverV_ReplyToServerG_clientS * newServerV_Reply = (serverV_ReplyToServerG_clientS *) calloc(1, sizeof(serverV_ReplyToServerG_clientS));
    if (!newServerG_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) healthCardNumber, sizeof(char) * HEALTH_CARD_NUMBER_LENGTH)) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) & clientS_viaServerG_SenderID, sizeof(clientS_viaServerG_SenderID))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) healthCardNumber, sizeof(char) * HEALTH_CARD_NUMBER_LENGTH)) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullReadReturnValue = fullRead(serverV_SocketFileDescriptor, (void *) newServerV_Reply, sizeof(* newServerV_Reply))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    strncpy((char *) newServerG_Reply->healthCardNumber, (const char *) newServerV_Reply->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerG_Reply->requestResult = newServerV_Reply->requestResult;
    if ((fullWriteReturnValue = fullWrite(connectionFileDescriptor, (const void *) newServerG_Reply, sizeof(serverG_ReplyToClientS))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    free(newServerV_Reply);
    free(newServerG_Reply);
}

void clientT_RequestHandler (int connectionFileDescriptor, int serverV_SocketFileDescriptor) {
    unsigned short int clientT_viaServerG_SenderID = clientT_viaServerG_Sender;
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    clientT_RequestToServerG * newClientT_Request = (clientT_RequestToServerG *) calloc(1, sizeof(clientT_RequestToServerG));
    serverG_ReplyToClientT * newServerG_Reply = (serverG_ReplyToClientT *) calloc(1, sizeof(serverG_ReplyToClientT));
    serverG_RequestToServerV_onBehalfOfClientT * newServerG_Request = (serverG_RequestToServerV_onBehalfOfClientT *) calloc(1, sizeof(serverG_RequestToServerV_onBehalfOfClientT));
    serverV_ReplyToServerG_clientT * newServerV_Reply = (serverV_ReplyToServerG_clientT *) calloc(1, sizeof(serverV_ReplyToServerG_clientT));
    if (!newServerG_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newClientT_Request) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerG_Request) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) newClientT_Request, (size_t) sizeof(* newClientT_Request))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    strncpy((char *) newServerG_Request->healthCardNumber, (const char *) newClientT_Request->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerG_Request->updateValue = newClientT_Request->updateValue;
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) & clientT_viaServerG_SenderID, sizeof(clientT_viaServerG_SenderID))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) newServerG_Request, sizeof(* newServerG_Request))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullReadReturnValue = fullRead(serverV_SocketFileDescriptor, (void *) newServerV_Reply, sizeof(* newServerV_Reply))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    strncpy((char *) newServerG_Reply->healthCardNumber, (const char *) newServerV_Reply->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerG_Reply->updateResult = newServerV_Reply->updateResult;
    if ((fullWriteReturnValue = fullWrite(connectionFileDescriptor, (const void *) newServerG_Reply, sizeof(* newServerG_Reply))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    free(newClientT_Request);
    free(newServerG_Reply);
    free(newServerG_Request);
    free(newServerV_Reply);
}
