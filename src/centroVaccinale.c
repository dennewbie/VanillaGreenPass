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
//    time_t systemTime;
    
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
            ssize_t fullWriteReturnValue, fullReadReturnValue;
            centroVaccinaleReply * newCentroVaccinaleReply = (centroVaccinaleReply *) calloc(1, sizeof(centroVaccinaleReply));
            if (!newCentroVaccinaleReply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
            char buffer[HEALTH_CARD_NUMBER_LENGTH];
            
            if ((fullReadReturnValue = fullRead(connectionFileDescritor, (void *) buffer, (size_t) HEALTH_CARD_NUMBER_LENGTH)) < 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
            newCentroVaccinaleReply->vaccineExpirationDate = getVaccineExpirationDate();
            
            
            
            
            
            if ((fullWriteReturnValue = fullWrite(connectionFileDescritor, (const void *) newCentroVaccinaleReply, (size_t) sizeof(centroVaccinaleReply))) < 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
            
            
//            if (snprintf(buffer, sizeof(buffer), "%.24s\r\n", ctime(& ticks)) < 0) raiseError(SNPRINTF_SCOPE, SNPRINTF_ERROR);
//            if (fullWrite(connectionFileDescritor, (const void *) buffer, strlen(buffer) * sizeof(char)) != 0) raiseError(WRITE_SCOPE, WRITE_ERROR);
//            if (logging) clientLog(AF_INET, (struct sockaddr_in *) & client, LOG_IP);
            
            wclose(connectionFileDescritor);
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
