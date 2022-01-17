//
//  centroVaccinale.c
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 08/01/22.
//

// TODO: error handling su tutte le funzioni completo

#include "centroVaccinale.h"

int main (int argc, char * argv[]) {
    int serverV_SocketFileDescriptor, listenFileDescriptor, connectionFileDescriptor, enable = TRUE;
    struct sockaddr_in client, centroVaccinaleAddress;
    const char * expectedUsageMessage = "<Centro Vaccinale Port>";
    const char * configFilePathCentroVaccinale = "../conf/centroVaccinale.conf";
    unsigned short int centroVaccinalePort;
    pid_t childPid;
    
    checkUsage(argc, (const char **) argv, 2, expectedUsageMessage);
    centroVaccinalePort = (unsigned short int) strtoul(argv[1], (char **) NULL, 10);
    
    // si imposta la comunicazione col clientCitizen
    listenFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(listenFileDescriptor, SOL_SOCKET, SO_REUSEADDR, & enable, (socklen_t) sizeof(int)) < 0) raiseError(SET_SOCK_OPT_SCOPE, SET_SOCK_OPT_ERROR);
    memset((void *) & centroVaccinaleAddress, 0, sizeof(centroVaccinaleAddress));
    memset((void *) & client, 0, sizeof(client));
    
//    retrieveConfigurationData(configFilePathClient, & stringCentroVaccinale_AddressIP, & centroVaccinalePort);
    centroVaccinaleAddress.sin_family      = AF_INET;
    centroVaccinaleAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    centroVaccinaleAddress.sin_port        = htons(centroVaccinalePort);
    wbind(listenFileDescriptor, (struct sockaddr *) & centroVaccinaleAddress, (socklen_t) sizeof(centroVaccinaleAddress));
    wlisten(listenFileDescriptor, QUEUE_SIZE);
    
    while (TRUE) {
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        connectionFileDescriptor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength);
        if ((childPid = fork()) < 0) {
            raiseError(FORK_SCOPE, FORK_ERROR);
        } else if (childPid == 0) {
            wclose(listenFileDescriptor);
            serverV_SocketFileDescriptor = * createConnectionWithServerV(configFilePathCentroVaccinale);
            clientCitizenRequestHandler(connectionFileDescriptor, serverV_SocketFileDescriptor);
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

void clientCitizenRequestHandler (int connectionFileDescriptor, int serverV_SocketFileDescriptor) {
    centroVaccinaleReplyToClientCitizen * newCentroVaccinaleReply = (centroVaccinaleReplyToClientCitizen *) calloc(1, sizeof(centroVaccinaleReplyToClientCitizen));
    centroVaccinaleRequestToServerV * newCentroVaccinaleRequest = (centroVaccinaleRequestToServerV *) calloc(1, sizeof(centroVaccinaleRequestToServerV));
    serverV_ReplyToCentroVaccinale * newServerV_Reply = (serverV_ReplyToCentroVaccinale *) calloc(1, sizeof(serverV_ReplyToCentroVaccinale));
    if (!newCentroVaccinaleReply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newCentroVaccinaleRequest) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    char buffer[HEALTH_CARD_NUMBER_LENGTH];
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    unsigned short int centroVaccinaleSender = centroVaccinaleSender;
    
    if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) buffer, (size_t) HEALTH_CARD_NUMBER_LENGTH * sizeof(char))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    strncpy((char *) newCentroVaccinaleRequest->healthCardNumber, (const char *)  buffer, HEALTH_CARD_NUMBER_LENGTH);
    strncpy((char *) newCentroVaccinaleRequest->nowDate, (const char *) getNowDate(), DATE_LENGTH);
//    newCentroVaccinaleRequest->healthCardNumber[HEALTH_CARD_NUMBER_LENGTH - 1] = '\0';
//    newCentroVaccinaleRequest->nowDate[DATE_LENGTH - 1] = '\0';
    
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) & centroVaccinaleSender, (size_t) sizeof(unsigned short int))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) newCentroVaccinaleRequest, (size_t) sizeof(centroVaccinaleRequestToServerV))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullReadReturnValue = fullRead(serverV_SocketFileDescriptor, (void *) newServerV_Reply, (size_t) sizeof(serverV_ReplyToCentroVaccinale))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    strncpy(newCentroVaccinaleReply->healthCardNumber, (const char *) newServerV_Reply->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    strncpy(newCentroVaccinaleReply->vaccineExpirationDate, (const char *) newServerV_Reply->vaccineExpirationDate, DATE_LENGTH);
//    newCentroVaccinaleReply->healthCardNumber[HEALTH_CARD_NUMBER_LENGTH - 1] = '\0';
//    newCentroVaccinaleReply->vaccineExpirationDate[DATE_LENGTH - 1] = '\0';
    
    newCentroVaccinaleReply->requestResult = newServerV_Reply->requestResult == TRUE ? TRUE : FALSE;
    if ((fullWriteReturnValue = fullWrite(connectionFileDescriptor, (const void *) newCentroVaccinaleReply, (size_t) sizeof(centroVaccinaleReplyToClientCitizen))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    
    free(newCentroVaccinaleReply);
    free(newCentroVaccinaleRequest);
    free(newServerV_Reply);
}
