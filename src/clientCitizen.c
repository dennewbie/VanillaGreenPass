//
//  clientCitizen.c
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 07/01/22.
//

#include "clientCitizen.h"

int main (int argc, char * argv[]) {
    int centroVaccinaleSocketFileDescriptor;
    struct sockaddr_in centroVaccinaleAddress;
    const char * expectedUsageMessage = "<Numero Tessera Sanitaria>", * configFilePath = "../conf/clientCitizen.conf";
    char * stringCentroVaccinaleAddressIP = NULL, * healthCardNumber;
    unsigned short int centroVaccinalePort;
    
    checkUsage(argc, (const char **) argv, 2, expectedUsageMessage);
    checkHealtCardNumber(argv[1]);
    healthCardNumber = (char *) calloc(HEALTH_CARD_NUMBER_LENGTH, sizeof(char));
    if (!healthCardNumber) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strcpy(healthCardNumber, (const char *) argv[1]);
    healthCardNumber[HEALTH_CARD_NUMBER_LENGTH - 1] = '\0';
//    printf("\nSTRING: %s\nLEN: %d\n", healthCardNumber, (int) strlen(healthCardNumber));
    retrieveConfigurationData(configFilePath, & stringCentroVaccinaleAddressIP, & centroVaccinalePort);

    centroVaccinaleSocketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & centroVaccinaleAddress, 0, sizeof(centroVaccinaleAddress));
    centroVaccinaleAddress.sin_family = AF_INET;
    centroVaccinaleAddress.sin_port   = htons(centroVaccinalePort);
    if (inet_pton(AF_INET, stringCentroVaccinaleAddressIP, (void *) & centroVaccinaleAddress.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);

    wconnect(centroVaccinaleSocketFileDescriptor, (struct sockaddr *) & centroVaccinaleAddress, (socklen_t) sizeof(centroVaccinaleAddress));
    if (fprintf(stdout, "\nBenvenuti al Centro Vaccinale\nNumero tessera sanitaria: %s\n\n... A breve ti verra' inoculato il vaccino...\n", healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    getVaccination(centroVaccinaleSocketFileDescriptor, (const void *) healthCardNumber, (size_t) sizeof(char) * HEALTH_CARD_NUMBER_LENGTH);
    wclose(centroVaccinaleSocketFileDescriptor);

    free(stringCentroVaccinaleAddressIP);
    free(healthCardNumber);
    exit(0);
}

void getVaccination (int centroVaccinaleSocketFileDescriptor, const void * healthCardNumber, size_t nBytes) {
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    centroVaccinaleReplyToClientCitizen * newCentroVaccinaleReply = (centroVaccinaleReplyToClientCitizen *) calloc(1, sizeof(centroVaccinaleReplyToClientCitizen));
    if (!newCentroVaccinaleReply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    if (fprintf(stdout, "\n... Vaccinazione in corso ...\n") < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    if ((fullWriteReturnValue = fullWrite(centroVaccinaleSocketFileDescriptor, healthCardNumber, nBytes)) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullReadReturnValue = fullRead(centroVaccinaleSocketFileDescriptor, newCentroVaccinaleReply, (size_t) sizeof(centroVaccinaleReplyToClientCitizen))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    if (newCentroVaccinaleReply->requestResult == FALSE) {
        if (fprintf(stdout, "\nNon e' possibile effetuare un'altra dose di vaccino. Devono passare almeno %d mesi dall'ultima inoculazione.\nData a partire dalla quale e' possibile effettuare un'altra dose di vaccino: %s\nArrivederci.\n", MONTHS_TO_WAIT_FOR_NEXT_VACCINATION, newCentroVaccinaleReply->vaccineExpirationDate) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    } else {
        if (fprintf(stdout, "\nLa vaccinazione e' andata a buon fine.\nData a partire dalla quale e' possibile effettuare un'altra dose di vaccino: %s\nArrivederci.\n", newCentroVaccinaleReply->vaccineExpirationDate) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    }
    free(newCentroVaccinaleReply);
}
