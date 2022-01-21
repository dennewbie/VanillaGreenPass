//
//  clientT.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

// CHECKED
#include "clientT.h"

int main (int argc, char * argv[]) {
    char * healthCardNumber;
    int newGreenPassStatus, serverG_SocketFileDescriptor = setupClientT(argc, argv, & healthCardNumber, & newGreenPassStatus);
    healthCardNumber[HEALTH_CARD_NUMBER_LENGTH - 1] = '\0';
    updateGreenPass(serverG_SocketFileDescriptor, (const void *) healthCardNumber, (const unsigned short int) newGreenPassStatus);
    wclose(serverG_SocketFileDescriptor);
    free(healthCardNumber);
    exit(0);
}

int setupClientT (int argc, char * argv[], char ** healthCardNumber, int * newGreenPassStatus) {
    struct sockaddr_in serverG_Address;
    
    char * stringServerG_IP = NULL;
    unsigned short int serverG_Port;
    int serverG_SocketFileDescriptor;
    
    checkUsage(argc, (const char **) argv, CLIENT_T_ARGS_NO, expectedUsageMessage);
    checkHealtCardNumber(argv[1]);
    * newGreenPassStatus = (unsigned short int) strtoul((const char * restrict) argv[2], (char ** restrict) NULL, 10);
    if (* newGreenPassStatus == 0 && (errno == EINVAL || errno == ERANGE)) raiseError(STRTOUL_SCOPE, STRTOUL_ERROR);
    if (* newGreenPassStatus != TRUE && * newGreenPassStatus != FALSE) raiseError(INVALID_UPDATE_STATUS_SCOPE, INVALID_UPDATE_STATUS_ERROR);
    
    * healthCardNumber = (char *) calloc(HEALTH_CARD_NUMBER_LENGTH, sizeof(char));
    if (! * healthCardNumber) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strncpy(* healthCardNumber, (const char *) argv[1], HEALTH_CARD_NUMBER_LENGTH - 1);
    retrieveConfigurationData(configFilePath, & stringServerG_IP, & serverG_Port);
    
    serverG_SocketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & serverG_Address, 0, sizeof(serverG_Address));
    serverG_Address.sin_family = AF_INET;
    serverG_Address.sin_port   = htons(serverG_Port);
    if (inet_pton(AF_INET, (const char * restrict) stringServerG_IP, (void *) & serverG_Address.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);
    
    wconnect(serverG_SocketFileDescriptor, (struct sockaddr *) & serverG_Address, (socklen_t) sizeof(serverG_Address));
    if (fprintf(stdout, "\nAggiornamento Validita' GreenPass\nNumero tessera sanitaria: %s\n\n... A breve verra' inviato il nuovo stato di validita' del Green Pass...\n", * healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    free(stringServerG_IP);
    return serverG_SocketFileDescriptor;
}

void updateGreenPass (int serverG_SocketFileDescriptor, const void * healthCardNumber, const unsigned short int newGreenPassStatus) {
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    unsigned short int clientT_SenderID = clientT_viaServerG_Sender;
    serverG_ReplyToClientT * newServerG_Reply = (serverG_ReplyToClientT *) calloc(1, sizeof(serverG_ReplyToClientT));
    clientT_RequestToServerG * newClientT_Request = (clientT_RequestToServerG *) calloc(1, sizeof(clientT_RequestToServerG));
    if (!newServerG_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newClientT_Request) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    strncpy(newClientT_Request->healthCardNumber, healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newClientT_Request->updateValue = newGreenPassStatus;
    
    if (fprintf(stdout, "\n... Aggiornamento in corso ...\n") < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    if ((fullWriteReturnValue = fullWrite(serverG_SocketFileDescriptor, (const void *) & clientT_SenderID, sizeof(clientT_SenderID))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullWriteReturnValue = fullWrite(serverG_SocketFileDescriptor, (const void *) newClientT_Request, sizeof(* newClientT_Request))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullReadReturnValue = fullRead(serverG_SocketFileDescriptor, (void *) newServerG_Reply, sizeof(* newServerG_Reply))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    if (newServerG_Reply->updateResult == FALSE) {
        if (fprintf(stdout, "\nL'aggiornamento del Green Pass associato alla tessera sanitaria %s, non e' andato a buon fine.\nArrivederci.\n", newServerG_Reply->healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    } else {
        if (fprintf(stdout, "\nL'aggiornamento del Green Pass associato alla tessera sanitaria %s, e' andato a buon fine.\nArrivederci.\n", newServerG_Reply->healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    }
    free(newServerG_Reply);
    free(newClientT_Request);
}
