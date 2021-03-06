//
//  GreenPassUtility.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 07/01/22.
//

#include "GreenPassUtility.h"

// Controllo del formato e lunghezza del codice della tessera Sanitaria
void checkHealtCardNumber (char * healthCardNumber) {
    size_t healthCardNumberLength = strlen(healthCardNumber);
    if (healthCardNumberLength + 1 != HEALTH_CARD_NUMBER_LENGTH) raiseError(CHECK_HEALTH_CARD_NUMBER_SCOPE, CHECK_HEALTH_CARD_NUMBER_ERROR);
}


// Procedura per acquisire la configurazione riportata nel file di configurazione passato come parametro.
void retrieveConfigurationData (const char * configFilePath, char ** configurationIP, unsigned short int * configurationPort) {
    FILE * filePointer;
    size_t IPlength = 0, portLength = 0;
    ssize_t getLineBytes;
    char * tempStringConfigurationIP = NULL, * stringServerAddressPort = NULL;
    
    filePointer = fopen(configFilePath, "r");
    if (!filePointer) raiseError(FOPEN_SCOPE, FOPEN_ERROR);
    if ((getLineBytes = getline((char ** restrict) & tempStringConfigurationIP, (size_t * restrict) & IPlength, (FILE * restrict) filePointer)) == -1) raiseError(GETLINE_SCOPE, GETLINE_ERROR);
    * configurationIP = (char *) calloc(strlen(tempStringConfigurationIP), sizeof(char));
    if (! *configurationIP) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strncpy(* configurationIP, (const char *) tempStringConfigurationIP, strlen(tempStringConfigurationIP) - 1);
    checkIP(* configurationIP);
    if ((getLineBytes = getline((char ** restrict) & stringServerAddressPort, (size_t * restrict) & portLength, (FILE * restrict) filePointer)) == -1) raiseError(GETLINE_SCOPE, GETLINE_ERROR);
    * configurationPort = (unsigned short int) strtoul((const char * restrict) stringServerAddressPort, (char ** restrict) NULL, 10);
    if (configurationPort == 0 && (errno == EINVAL || errno == ERANGE)) raiseError(STRTOUL_SCOPE, STRTOUL_ERROR);
    free(tempStringConfigurationIP);
    free(stringServerAddressPort);
    fclose(filePointer);
}

// Calcolo del periodo di validità di un nuovo Vanilla Green Pass
char * getVaccineExpirationDate (void) {
    time_t systemTimeSeconds = time(NULL);
    // Si considera la data di sistema attuale
    struct tm * expirationDateTimeInfo = localtime((const time_t *) & systemTimeSeconds);
    
    // Si tronca tale data al primo giorno del mese
    expirationDateTimeInfo->tm_mday = 1;
    
    // Si aggiungono almeno 5 mesi e si verifica se è cambiato l'anno
    if (expirationDateTimeInfo->tm_mon + MONTHS_TO_WAIT_FOR_NEXT_VACCINATION + 1 > 11) {
        expirationDateTimeInfo->tm_year += 1;
        expirationDateTimeInfo->tm_mon = (expirationDateTimeInfo->tm_mon + MONTHS_TO_WAIT_FOR_NEXT_VACCINATION + 1) % MONTHS_IN_A_YEAR;
    } else {
        expirationDateTimeInfo->tm_mon = (expirationDateTimeInfo->tm_mon + MONTHS_TO_WAIT_FOR_NEXT_VACCINATION + 1);
    }
    
    // Conversione in stringa della data e ritorno al chiamante.
    char * vaccineExpirationDate = (char *) calloc(DATE_LENGTH, sizeof(char));
    if (!vaccineExpirationDate) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    sprintf(vaccineExpirationDate, "%02d-%02d-%d", expirationDateTimeInfo->tm_mday, expirationDateTimeInfo->tm_mon, expirationDateTimeInfo->tm_year + 1900);
    vaccineExpirationDate[DATE_LENGTH - 1] = '\0';
    return vaccineExpirationDate;
}

// Procedura per ottenere la data attuale
char * getNowDate (void) {
    time_t tempTimeSeconds = time(NULL);
    struct tm * nowDateTimeInfo = localtime((const time_t *) & tempTimeSeconds);
    char * nowDate = (char *) calloc(DATE_LENGTH, sizeof(char));
    if (!nowDate) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    sprintf(nowDate, "%02d-%02d-%d", nowDateTimeInfo->tm_mday, nowDateTimeInfo->tm_mon + 1, nowDateTimeInfo->tm_year + 1900);
    nowDate[DATE_LENGTH - 1] = '\0';
    return nowDate;
}

/*
La seguente funzione ritorna il socket file descriptor di una connessione impostata con il ServerV.
Si passa in input il file di configurazione che è stati fornito al CentroVaccinale o al ServerG per mettersi
in contatto con il ServerV.
*/
int createConnectionWithServerV (const char * configFilePath) {
    struct sockaddr_in serverV_Address;
    char * stringServerV_AddressIP = NULL;
    unsigned short int serverV_Port;
    int serverV_SocketFileDescriptor;
    
    // Effettivo "retrieve" del file di configurazione.
    retrieveConfigurationData(configFilePath, & stringServerV_AddressIP, & serverV_Port);
    // Si imposta la comunicazione col serverV
    serverV_SocketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & serverV_Address, 0, sizeof(serverV_Address));
    serverV_Address.sin_family = AF_INET;
    serverV_Address.sin_port   = htons(serverV_Port);
    if (inet_pton(AF_INET, (const char * restrict) stringServerV_AddressIP, (void *) & serverV_Address.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);
    wconnect(serverV_SocketFileDescriptor, (struct sockaddr *) & serverV_Address, (socklen_t) sizeof(serverV_Address));
    free(stringServerV_AddressIP);
    return serverV_SocketFileDescriptor;
}
