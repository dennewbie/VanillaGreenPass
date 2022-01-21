//
//  clientS.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

// CHECKED
#include "clientS.h"

int main (int argc, char * argv[]) {
    int serverG_SocketFileDescriptor;
    struct sockaddr_in serverG_Address;
    const char * expectedUsageMessage = "<Numero Tessera Sanitaria da Controllare>", * configFilePath = "../conf/clientS.conf";
    char * stringServerG_IP = NULL, * healthCardNumber;
    unsigned short int serverG_Port;
    
    checkUsage(argc, (const char **) argv, CLIENT_S_ARGS_NO, expectedUsageMessage);
    checkHealtCardNumber(argv[1]);
    healthCardNumber = (char *) calloc(HEALTH_CARD_NUMBER_LENGTH, sizeof(char));
    if (!healthCardNumber) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strcpy(healthCardNumber, (const char *) argv[1]);
    retrieveConfigurationData(configFilePath, & stringServerG_IP, & serverG_Port);

    serverG_SocketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & serverG_Address, 0, sizeof(serverG_Address));
    serverG_Address.sin_family = AF_INET;
    serverG_Address.sin_port   = htons(serverG_Port);
    if (inet_pton(AF_INET, (const char * restrict) stringServerG_IP, (void *) & serverG_Address.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);

    wconnect(serverG_SocketFileDescriptor, (struct sockaddr *) & serverG_Address, (socklen_t) sizeof(serverG_Address));
    if (fprintf(stdout, "\nVerifica GreenPass\nNumero tessera sanitaria: %s\n\n... A breve verra' mostrato se il GreenPass inserito risulta essere valido...\n", healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    checkGreenPass(serverG_SocketFileDescriptor, (const void *) healthCardNumber, (size_t) sizeof(char) * HEALTH_CARD_NUMBER_LENGTH);
    wclose(serverG_SocketFileDescriptor);

    free(stringServerG_IP);
    free(healthCardNumber);
    exit(0);
}

void checkGreenPass (int serverG_SocketFileDescriptor, const void * healthCardNumber, size_t nBytes) {
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    unsigned short int clientS_SenderID = clientS_viaServerG_Sender;
    serverG_ReplyToClientS * newServerG_Reply = (serverG_ReplyToClientS *) calloc(1, sizeof(serverG_ReplyToClientS));
    if (!newServerG_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    if (fprintf(stdout, "\n... Verifica in corso ...\n") < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    if ((fullWriteReturnValue = fullWrite(serverG_SocketFileDescriptor, (const void *) & clientS_SenderID, sizeof(clientS_SenderID))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullWriteReturnValue = fullWrite(serverG_SocketFileDescriptor, healthCardNumber, nBytes)) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullReadReturnValue = fullRead(serverG_SocketFileDescriptor, (void *) newServerG_Reply, sizeof(* newServerG_Reply))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    if (newServerG_Reply->requestResult == FALSE) {
        if (fprintf(stdout, "\nLa tessera sanitaria immessa non risulta essere associata a un GreenPass attualmente valido.\nArrivederci.\n") < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    } else {
        if (fprintf(stdout, "\nLa tessera sanitaria immessa risulta essere associata a un GreenPass attualmente valido.\nArrivederci.\n") < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    }
    free(newServerG_Reply);
}
