//
//  clientT.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

#include "clientT.h"

int main (int argc, char * argv[]) {
    int serverG_SocketFileDescriptor;
    struct sockaddr_in serverG_Address;
    const char * expectedUsageMessage = "<Numero Tessera Sanitaria> <Nuovo Stato Green Pass (0 = NON VALIDO / 1 = VALIDO)>", * configFilePath = "../conf/clientT.conf";
    char * stringServerG_IP = NULL, * healthCardNumber;
    unsigned short int serverG_Port;
    
    checkUsage(argc, (const char **) argv, 3, expectedUsageMessage);
    checkHealtCardNumber(argv[1]);
    unsigned short int newGreenPassStatus = (unsigned short int) strtoul(argv[2], NULL, 10);
    if (newGreenPassStatus != TRUE && newGreenPassStatus != FALSE) raiseError(INVALID_UPDATE_STATUS_SCOPE, INVALID_UPDATE_STATUS_ERROR);
    
    healthCardNumber = (char *) calloc(HEALTH_CARD_NUMBER_LENGTH, sizeof(char));
    if (!healthCardNumber) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strcpy(healthCardNumber, (const char *) argv[1]);
    healthCardNumber[HEALTH_CARD_NUMBER_LENGTH - 1] = '\0';
    retrieveConfigurationData(configFilePath, & stringServerG_IP, & serverG_Port);

    serverG_SocketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & serverG_Address, 0, sizeof(serverG_Address));
    serverG_Address.sin_family = AF_INET;
    serverG_Address.sin_port   = htons(serverG_Port);
    if (inet_pton(AF_INET, stringServerG_IP, (void *) & serverG_Address.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);

    wconnect(serverG_SocketFileDescriptor, (struct sockaddr *) & serverG_Address, (socklen_t) sizeof(serverG_Address));
    if (fprintf(stdout, "\nAggiornamento Validita' GreenPass\nNumero tessera sanitaria: %s\n\n... A breve verra' inviato il nuovo stato di validita' del Green Pass...\n", healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    updateGreenPass(serverG_SocketFileDescriptor, (const void *) healthCardNumber, (const unsigned short int) newGreenPassStatus);
    wclose(serverG_SocketFileDescriptor);

    free(stringServerG_IP);
    free(healthCardNumber);
    exit(0);
}

void updateGreenPass (int serverG_SocketFileDescriptor, const void * healthCardNumber, const unsigned short int newGreenPassStatus) {
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    serverG_ReplyToClientT * newServerG_Reply = (serverG_ReplyToClientT *) calloc(1, sizeof(serverG_ReplyToClientT));
    clientT_RequestToServerG * newClientT_Request = (clientT_RequestToServerG *) calloc(1, sizeof(clientT_RequestToServerG));
    if (!newServerG_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newClientT_Request) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    strncpy(newClientT_Request->healthCardNumber, healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newClientT_Request->updateValue = newGreenPassStatus;
    
    if (fprintf(stdout, "\n... Aggiornamento in corso ...\n") < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    if ((fullWriteReturnValue = fullWrite(serverG_SocketFileDescriptor, newClientT_Request, sizeof(clientT_RequestToServerG))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullReadReturnValue = fullRead(serverG_SocketFileDescriptor, newServerG_Reply, (size_t) sizeof(serverG_ReplyToClientT))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    if (newServerG_Reply->requestResult == FALSE) {
        if (fprintf(stdout, "\nL'aggiornamento del Green Pass associato alla tessera sanitaria %s, non e' andato a buon fine.\nArrivederci.\n", newServerG_Reply->healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    } else {
        if (fprintf(stdout, "\nL'aggiornamento del Green Pass associato alla tessera sanitaria %s, e' andato a buon fine.\nArrivederci.\n", newServerG_Reply->healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    }
    free(newServerG_Reply);
    free(newClientT_Request);
}
