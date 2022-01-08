//
//  centroVaccinale.c
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 08/01/22.
//

#include "centroVaccinale.h"

int main (int argc, char * argv[]) {
    int serverV_SocketFileDescriptor, listenFileDescriptor, connectionFileDescritor, enable = TRUE;
    struct sockaddr_in serverV_Address, client, centroVaccinaleAddress;
    const char * configFilePathCentroVaccinale = "../conf/centroVaccinale.conf", * configFilePathClient = "../conf/clientCitizen.conf";
    char * stringServerV_AddressIP = NULL, * stringCentroVaccinale_AddressIP = NULL;
    unsigned short int serverV_Port, centroVaccinalePort;
    pid_t childPid;
    
    retrieveConfigurationData(configFilePathCentroVaccinale, & stringServerV_AddressIP, & serverV_Port);
    
    serverV_SocketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & serverV_Address, 0, sizeof(serverV_Address));
    serverV_Address.sin_family = AF_INET;
    serverV_Address.sin_port   = htons(serverV_Port);
    if (inet_pton(AF_INET, stringServerV_AddressIP, (void *) & serverV_Address.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);
    wconnect(serverV_SocketFileDescriptor, (struct sockaddr *) & serverV_Address, (socklen_t) sizeof(serverV_Address));
    
    listenFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(listenFileDescriptor, SOL_SOCKET, SO_REUSEADDR, & enable, (socklen_t) sizeof(int)) < 0) raiseError(SET_SOCK_OPT_SCOPE, SET_SOCK_OPT_ERROR);
    memset((void *) & centroVaccinaleAddress, 0, sizeof(centroVaccinaleAddress));
    memset((void *) & client, 0, sizeof(client));
    
    retrieveConfigurationData(configFilePathClient, & stringCentroVaccinale_AddressIP, & centroVaccinalePort);
    centroVaccinaleAddress.sin_family      = AF_INET;
    centroVaccinaleAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    centroVaccinaleAddress.sin_port        = htons(centroVaccinalePort);
    
    wbind(listenFileDescriptor, (struct sockaddr *) & centroVaccinaleAddress, (socklen_t) sizeof(centroVaccinaleAddress));
    wlisten(listenFileDescriptor, QUEUE_SIZE);
    
    while (TRUE) {
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        connectionFileDescritor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength);
        if ((childPid = fork()) < 0) {
            raiseError(FORK_SCOPE, FORK_ERROR);
        } else if (childPid == 0) {
            wclose(listenFileDescriptor);
            char buffer[HEALTH_CARD_NUMBER_LENGTH];
            ssize_t fullWriteReturnValue, fullReadReturnValue;
            centroVaccinaleReplyToClientCitizen * newCentroVaccinaleReply = (centroVaccinaleReplyToClientCitizen *) calloc(1, sizeof(centroVaccinaleReplyToClientCitizen));
            centroVaccinaleRequestToServerV * newCentroVaccinaleRequest = (centroVaccinaleRequestToServerV *) calloc(1, sizeof(centroVaccinaleRequestToServerV));
            serverV_ReplyToCentroVaccinale * newServerV_Reply = (serverV_ReplyToCentroVaccinale *) calloc(1, sizeof(serverV_ReplyToCentroVaccinale));
            if (!newCentroVaccinaleReply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
            if (!newCentroVaccinaleRequest) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
            if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
            
            if ((fullReadReturnValue = fullRead(connectionFileDescritor, (void *) buffer, (size_t) HEALTH_CARD_NUMBER_LENGTH)) < 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
            
            strcpy((char *) newCentroVaccinaleRequest->healthCardNumber, (const char *)  buffer);
            newCentroVaccinaleRequest->vaccineExpirationDate = getVaccineExpirationDate();
            if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) newCentroVaccinaleRequest, (size_t) sizeof(centroVaccinaleRequestToServerV))) < 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
            if ((fullReadReturnValue = fullRead(serverV_SocketFileDescriptor, (void *) newServerV_Reply, (size_t) sizeof(serverV_ReplyToCentroVaccinale))) < 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
            
            strcpy(newCentroVaccinaleReply->healthCardNumber, (const char *) newServerV_Reply->healthCardNumber);
            newCentroVaccinaleReply->vaccineExpirationDate = newServerV_Reply->vaccineExpirationDate;
            newCentroVaccinaleReply->requestResult = newServerV_Reply->requestResult == TRUE ? TRUE : FALSE;
            if ((fullWriteReturnValue = fullWrite(connectionFileDescritor, (const void *) newCentroVaccinaleReply, (size_t) sizeof(centroVaccinaleReplyToClientCitizen))) < 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
            wclose(connectionFileDescritor);
            free(newCentroVaccinaleReply);
            free(newCentroVaccinaleRequest);
            free(newServerV_Reply);
            exit(0);
        }
        wclose(connectionFileDescritor);
    }
    
    // codice mai eseguito
    free(stringServerV_AddressIP);
    wclose(listenFileDescriptor);
    wclose(serverV_SocketFileDescriptor);
    exit(0);
}