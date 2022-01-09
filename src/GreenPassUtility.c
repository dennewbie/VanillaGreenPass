//
//  GreenPassUtility.c
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 07/01/22.
//

#include "GreenPassUtility.h"

void checkHealtCardNumber (char * healthCardNumber) {
    size_t healthCardNumberLength = strlen(healthCardNumber);
    if (healthCardNumberLength != HEALTH_CARD_NUMBER_LENGTH) raiseError(CHECK_HEALTH_CARD_NUMBER_SCOPE, CHECK_HEALTH_CARD_NUMBER_ERROR);
}

void retrieveConfigurationData (const char * configFilePath, char ** configurationIP, unsigned short int * configurationPort) {
    FILE * filePointer;
    size_t IPlength = 0, portLength = 0;
    ssize_t getLineBytes;
    char * tempStringConfigurationIP = NULL, * stringServerAddressPort = NULL;
    
    filePointer = fopen(configFilePath, "r");
    if (!filePointer) raiseError(FOPEN_SCOPE, FOPEN_ERROR);
    if ((getLineBytes = getline(& tempStringConfigurationIP, & IPlength, filePointer)) < 0) raiseError(GETLINE_SCOPE, GETLINE_ERROR);
    *configurationIP = (char *) calloc(strlen(tempStringConfigurationIP) - 1, sizeof(char));
    if (! *configurationIP) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    strncpy(*configurationIP, tempStringConfigurationIP, sizeof(char) * (strlen(tempStringConfigurationIP) - 1));
    checkIP(*configurationIP);
    if ((getLineBytes = getline(& stringServerAddressPort, & portLength, filePointer)) < 0) raiseError(GETLINE_SCOPE, GETLINE_ERROR);
    *configurationPort = (unsigned short int) strtoul(stringServerAddressPort, (char **) NULL, 10);
    fclose(filePointer);
}

char * getVaccineExpirationDate () {
    struct tm * timeInfo;
    time_t systemTime;
    time(& systemTime);
    timeInfo = localtime(& systemTime);
    timeInfo->tm_mday = 1;
    if (timeInfo->tm_mon + MONTHS_TO_WAIT_FOR_NEXT_VACCINATION + 1 > 11) {
        timeInfo->tm_year += 1;
        timeInfo->tm_mon = (timeInfo->tm_mon + MONTHS_TO_WAIT_FOR_NEXT_VACCINATION + 1) % MONTHS_IN_A_YEAR;
    } else {
        timeInfo->tm_mon = (timeInfo->tm_mon + MONTHS_TO_WAIT_FOR_NEXT_VACCINATION + 1);
    }
    
    char * vaccineExpirationDate = (char *) calloc(DATE_LENGTH, sizeof(char));
    if (!vaccineExpirationDate) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    sprintf(vaccineExpirationDate, "%02d-%02d-%d\n", timeInfo->tm_mday, timeInfo->tm_mon, timeInfo->tm_year + 1900);
    return vaccineExpirationDate;
}

char * getNowDate(void) {
    time_t tempTime = time(NULL);
    char * nowDate = (char *) calloc(DATE_LENGTH, sizeof(char));
    if (!nowDate) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    struct tm * timeInfo = localtime(& tempTime);
    sprintf(nowDate, "%02d-%02d-%d\n", timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900);
    return nowDate;
}
