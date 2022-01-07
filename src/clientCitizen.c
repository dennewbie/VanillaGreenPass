//
//  clientCitizen.c
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 07/01/22.
//

#include "clientCitizen.h"

int main (int argc, char * argv[]) {
    int socketFileDescriptor;
    struct sockaddr_in serverAddress;
    const char * expectedUsageMessage = "<Numero Tessera Sanitaria> <IP Server Centro Vaccinale> <Porta Server Centro Vaccinale>";
    unsigned short int serverPort = (unsigned short int) strtoul(argv[3], (char **) NULL, 10);
    char * healthCardNumber = (char *) calloc(strlen(argv[1]), sizeof(char));
    
    
    checkUsage(argc, (const char **) argv, 4, expectedUsageMessage);
    checkIP(argv[2]);
    checkHealtCardNumber(argv[1]);
    if (!healthCardNumber) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strcpy(argv[1], healthCardNumber);

    socketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port   = htons(serverPort);
    if (inet_pton(AF_INET, argv[1], (void *) & serverAddress.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);
    
    wconnect(socketFileDescriptor, (struct sockaddr *) & serverAddress, (socklen_t) sizeof(serverAddress));
    
    // getVaccination(...);
    
    wclose(socketFileDescriptor);
    exit(0);
}

void getVaccination(void) {
    
}
