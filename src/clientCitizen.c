//
//  clientCitizen.c
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 07/01/22.
//

#include "clientCitizen.h"

int main (int argc, char * argv[]) {
//    time_t tempo = getVaccineExpirationDate();
//    char * time_str = ctime(& tempo);
//   time_str[strlen(time_str)-1] = '\0';
//   printf("Current Time : %s\n", time_str);
//    sleep(10);
//    char * time_str2 = ctime(& tempo);
//   time_str[strlen(time_str2)-1] = '\0';
//   printf("Current Time : %s\n", time_str2);
    
    
    int centroVaccinaleSocketFileDescriptor;
    struct sockaddr_in centroVaccinaleAddress;
    const char * expectedUsageMessage = "<Numero Tessera Sanitaria>";
    const char * configFilePath = "../conf/clientCitizen.conf";
    char * stringCentroVaccinaleAddressIP = NULL;
    unsigned short int centroVaccinalePort;
    char * healthCardNumber;
    
    checkUsage(argc, (const char **) argv, 2, expectedUsageMessage);
    healthCardNumber = (char *) calloc(strlen(argv[1]), sizeof(char));
    checkHealtCardNumber(argv[1]);
    if (!healthCardNumber) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strcpy(healthCardNumber, (const char *) argv[1]);
    
    retrieveConfigurationData(configFilePath, & stringCentroVaccinaleAddressIP, & centroVaccinalePort);

    centroVaccinaleSocketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & centroVaccinaleAddress, 0, sizeof(centroVaccinaleAddress));
    centroVaccinaleAddress.sin_family = AF_INET;
    centroVaccinaleAddress.sin_port   = htons(centroVaccinalePort);
    if (inet_pton(AF_INET, stringCentroVaccinaleAddressIP, (void *) & centroVaccinaleAddress.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);

    wconnect(centroVaccinaleSocketFileDescriptor, (struct sockaddr *) & centroVaccinaleAddress, (socklen_t) sizeof(centroVaccinaleAddress));
    if (fprintf(stdout, "\nBenvenuti al Centro Vaccinale\nNumero tessera sanitaria: %s\n\n... A breve ti verra' inoculato il vaccino...\n", healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    getVaccination(centroVaccinaleSocketFileDescriptor, (const void *) healthCardNumber, (size_t) sizeof(char) * strlen(healthCardNumber));
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
        if (fprintf(stdout, "\nNon e' possibile effetuare un'altra dose di vaccino. Devono passare almeno %d mesi dall'ultima inoculazione.\nData a partire dalla quale e' possibile effettuare un'altra dose di vaccino: %s\nArrivederci.", MONTHS_TO_WAIT_FOR_NEXT_VACCINATION, newCentroVaccinaleReply->vaccineExpirationDate) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    } else {
        if (fprintf(stdout, "\nLa vaccinazione e' andata a buon fine.\nData a partire dalla quale e' possibile effettuare un'altra dose di vaccino: %s\nArrivederci.", newCentroVaccinaleReply->vaccineExpirationDate) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    }
    free(newCentroVaccinaleReply);
}
