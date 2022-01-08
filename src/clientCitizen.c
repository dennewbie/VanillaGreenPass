//
//  clientCitizen.c
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 07/01/22.
//

#include "clientCitizen.h"

int main (int argc, char * argv[]) {
    FILE * filePointer;
    size_t IPlength = 0, portLength = 0;
    ssize_t getLineBytes;
    int serverSocketFileDescriptor;
    struct sockaddr_in serverAddress;
    const char * expectedUsageMessage = "<Numero Tessera Sanitaria>";
    unsigned short int serverPort;
    char * stringServerAddressIP = NULL, * tempStringServerAddressIP = NULL, * stringServerAddressPort = NULL;
    
    checkUsage(argc, (const char **) argv, 2, expectedUsageMessage);
    char * healthCardNumber = (char *) calloc(strlen(argv[1]), sizeof(char));
    checkHealtCardNumber(argv[1]);
    if (!healthCardNumber) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strcpy(healthCardNumber, argv[1]);
    
    filePointer = fopen("../conf/clientCitizen.conf", "r");
    if (filePointer == NULL) raiseError(FOPEN_SCOPE, FOPEN_ERROR);
    if ((getLineBytes = getline(& tempStringServerAddressIP, & IPlength, filePointer)) < 0) raiseError(GETLINE_SCOPE, GETLINE_ERROR);
    stringServerAddressIP = (char *) calloc(strlen(tempStringServerAddressIP) - 1, sizeof(char));
    if (!stringServerAddressIP) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strncpy(stringServerAddressIP, tempStringServerAddressIP, sizeof(char) * (strlen(tempStringServerAddressIP) - 1));
    checkIP(stringServerAddressIP);
    if ((getLineBytes = getline(& stringServerAddressPort, & portLength, filePointer)) < 0) raiseError(GETLINE_SCOPE, GETLINE_ERROR);
    serverPort = (unsigned short int) strtoul(stringServerAddressPort, (char **) NULL, 10);

    serverSocketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port   = htons(serverPort);
    if (inet_pton(AF_INET, stringServerAddressIP, (void *) & serverAddress.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);

    wconnect(serverSocketFileDescriptor, (struct sockaddr *) & serverAddress, (socklen_t) sizeof(serverAddress));
    if (fprintf(stdout, "\nBenvenuti al Centro Vaccinale\nNumero tessera sanitaria: %s\nA breve ti verra' inoculato il vaccino...\n", healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    getVaccination(serverSocketFileDescriptor, (const void *) healthCardNumber, (size_t) sizeof(char) * strlen(healthCardNumber));
    wclose(serverSocketFileDescriptor);

    free(tempStringServerAddressIP);
    free(stringServerAddressIP);
    free(stringServerAddressPort);
    free(healthCardNumber);
    exit(0);
}

void getVaccination(int serverSocketFileDescriptor, const void * healthCardNumber, size_t nBytes) {
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    char buffer[MAX_LINE];
    centroVaccinaleReply * newCentroVaccinaleReply = (centroVaccinaleReply *) calloc(1, sizeof(centroVaccinaleReply));
    if (!newCentroVaccinaleReply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    if (fprintf(stdout, "\n... Vaccinazione in corso ...\n") < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    if ((fullWriteReturnValue = fullWrite(serverSocketFileDescriptor, healthCardNumber, nBytes)) < 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullReadReturnValue = fullRead(serverSocketFileDescriptor, newCentroVaccinaleReply, sizeof(centroVaccinaleReply))) < 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    if (snprintf(buffer, sizeof(buffer), "%.24s\r\n", ctime(& newCentroVaccinaleReply->vaccineExpirationDate)) < 0) raiseError(SNPRINTF_SCOPE, SNPRINTF_ERROR);
    if (newCentroVaccinaleReply->requestResult == FALSE) {
        if (fprintf(stdout, "\nNon e' possibile effetuare un'altra dose di vaccino. Devono passare almeno %d mesi dall'ultima inoculazione.\nData a partire dalla quale e' possibile effettuare un'altra dose di vaccino: %s\n", MONTHS_TO_WAIT_FOR_NEXT_VACCINATION, buffer) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    } else {
        if (fprintf(stdout, "\nLa vaccinazione e' andata a buon fine.\nData a partire dalla quale e' possibile effettuare un'altra dose di vaccino: %s\n", buffer) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    }
    
    free(newCentroVaccinaleReply);
}
