//
//  serverV.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 08/01/22.
//

#include "serverV.h"

pthread_mutex_t fileSystemAccessMutex;
pthread_mutex_t connectionFileDescriptorMutex;
const char * dataPath = "../data/serverV.dat";
const char * tempDataPath = "../data/tempServerV.dat";

int main (int argc, char * argv[]) {
    int listenFileDescriptor, connectionFileDescriptor, enable = TRUE;
    pthread_t singleTID;
    pthread_attr_t attr;
    unsigned short int serverV_Port;
    struct sockaddr_in serverV_Address, client;
    //    const char * configFilePathServerV = "../conf/serverV.conf";
    const char * expectedUsageMessage = "<ServerV Port>";
    
    checkUsage(argc, (const char **) argv, 2, expectedUsageMessage);
    serverV_Port = (unsigned short int) strtoul(argv[1], (char **) NULL, 10);
    
    listenFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(listenFileDescriptor, SOL_SOCKET, SO_REUSEADDR, & enable, (socklen_t) sizeof(int)) < 0) raiseError(SET_SOCK_OPT_SCOPE, SET_SOCK_OPT_ERROR);
    memset((void *) & serverV_Address, 0, sizeof(serverV_Address));
    memset((void *) & client, 0, sizeof(client));
    serverV_Address.sin_family      = AF_INET;
    serverV_Address.sin_addr.s_addr = htonl(INADDR_ANY);
    serverV_Address.sin_port        = htons(serverV_Port);
    wbind(listenFileDescriptor, (struct sockaddr *) & serverV_Address, (socklen_t) sizeof(serverV_Address));
    wlisten(listenFileDescriptor, QUEUE_SIZE);
    
    if (pthread_mutex_init(& fileSystemAccessMutex, NULL) != 0) raiseError(PTHREAD_MUTEX_INIT_SCOPE, PTHREAD_MUTEX_INIT_ERROR);
    if (pthread_mutex_init(& connectionFileDescriptorMutex, NULL) != 0) raiseError(PTHREAD_MUTEX_INIT_SCOPE, PTHREAD_MUTEX_INIT_ERROR);
    pthread_attr_init(& attr);
    pthread_attr_setdetachstate(& attr, PTHREAD_CREATE_DETACHED);
    
    while (TRUE) {
        unsigned short int requestIdentifier;
        int threadCreationReturnValue;
        ssize_t fullReadReturnValue;
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        connectionFileDescriptor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength);
        centroVaccinaleRequestToServerV * newCentroVaccinaleRequest = (centroVaccinaleRequestToServerV *) calloc(1, sizeof(centroVaccinaleRequestToServerV));
        if (!newCentroVaccinaleRequest) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
        if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) & requestIdentifier, (size_t) sizeof(unsigned short int))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
        
        int * threadConnectionFileDescriptor = (int *) calloc(1, sizeof(int));
        if (!threadConnectionFileDescriptor) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
        pthread_mutex_lock(& connectionFileDescriptorMutex);
        if ((*threadConnectionFileDescriptor = dup(connectionFileDescriptor)) < 0) raiseError(DUP_SCOPE, DUP_ERROR);
        pthread_mutex_unlock(& connectionFileDescriptorMutex);
        switch (requestIdentifier) {
            case centroVaccinaleSender:
                // passo sempre lo stesso TID. Non mi interessa fare join
                if ((threadCreationReturnValue = pthread_create(& singleTID, & attr, & centroVaccinaleRequestHandler, threadConnectionFileDescriptor)) != 0) raiseError(PTHREAD_CREATE_SCOPE, PTHREAD_CREATE_ERROR);
                break;
            case clientS_viaServerG_Sender:
                if ((threadCreationReturnValue = pthread_create(& singleTID, & attr, & clientS_viaServerG_RequestHandler, threadConnectionFileDescriptor)) != 0) raiseError(PTHREAD_CREATE_SCOPE, PTHREAD_CREATE_ERROR);
                break;
            case clientT_viaServerG_Sender:
                if ((threadCreationReturnValue = pthread_create(& singleTID, & attr, & clientT_viaServerG_RequestHandler, threadConnectionFileDescriptor)) != 0) raiseError(PTHREAD_CREATE_SCOPE, PTHREAD_CREATE_ERROR);
                break;
            default:
                raiseError(INVALID_SENDER_ID_SCOPE, INVALID_SENDER_ID_ERROR);
                break;
        }
        wclose(connectionFileDescriptor);
    }
    // codice mai eseguito
    pthread_attr_destroy(& attr);
    wclose(listenFileDescriptor);
    exit(0);
}

void * centroVaccinaleRequestHandler (void * args) {
    int * threadConnectionFileDescriptor = (int *) args;
    ssize_t fullWriteReturnValue, fullReadReturnValue, getLineBytes;
    size_t effectiveLineLength = 0;
    char * singleLine = NULL, * tempCopiedDate;
    struct tm firstTime, secondTime;
    time_t scheduledVaccinationDate, requestVaccinationDate;
    double seconds;
    enum boolean isVaccineBlocked = FALSE, healthCardNumberWasFound = FALSE;
    FILE * originalFilePointer;
    FILE * tempFilePointer;
    
    centroVaccinaleRequestToServerV * newCentroVaccinaleRequest = (centroVaccinaleRequestToServerV *) calloc(1, sizeof(centroVaccinaleRequestToServerV));
    serverV_ReplyToCentroVaccinale * newServerV_Reply = (serverV_ReplyToCentroVaccinale *) calloc(1, sizeof(serverV_ReplyToCentroVaccinale));
    tempCopiedDate = (char *) calloc(DATE_LENGTH, sizeof(char));
    
    if (!newCentroVaccinaleRequest) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!tempCopiedDate) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    if ((fullReadReturnValue = fullRead(* threadConnectionFileDescriptor, (void *) newCentroVaccinaleRequest, (size_t) sizeof(centroVaccinaleRequestToServerV))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    strncpy(newServerV_Reply->healthCardNumber, newCentroVaccinaleRequest->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
//    newServerV_Reply->healthCardNumber[HEALTH_CARD_NUMBER_LENGTH - 1] = '\0';
    newServerV_Reply->requestResult = FALSE;
    
    pthread_mutex_lock(& fileSystemAccessMutex);
    originalFilePointer = fopen(dataPath, "r");
    if (!originalFilePointer) raiseError(FOPEN_SCOPE, FOPEN_ERROR);
    tempFilePointer = fopen(tempDataPath, "w");
    if (!tempFilePointer) raiseError(FOPEN_SCOPE, FOPEN_ERROR);

    // controllo 5 mesi non passati
    while ((getLineBytes = getline(& singleLine, & effectiveLineLength, originalFilePointer)) > 0) {
        if ((strncmp(newServerV_Reply->healthCardNumber, singleLine, HEALTH_CARD_NUMBER_LENGTH - 1)) == 0) {
            healthCardNumberWasFound = TRUE;
            strncpy(tempCopiedDate, singleLine + HEALTH_CARD_NUMBER_LENGTH, DATE_LENGTH);
            memset(& firstTime, 0, sizeof(struct tm));
            memset(& secondTime, 0, sizeof(struct tm));
                       
            firstTime.tm_mday = ((int) strtol(& tempCopiedDate[0], (char **) NULL, 10));
            firstTime.tm_mon = ((int) strtol(& tempCopiedDate[3], (char **) NULL, 10) - 1);
            firstTime.tm_year = ((int) strtol(& tempCopiedDate[6], (char **) NULL, 10) - 1900);
            scheduledVaccinationDate = mktime(& firstTime);
            
            secondTime.tm_mday = ((int) strtol(& newCentroVaccinaleRequest->nowDate[0], (char **) NULL, 10));
            secondTime.tm_mon = ((int) strtol(& newCentroVaccinaleRequest->nowDate[3], (char **) NULL, 10) - 1);
            secondTime.tm_year = ((int) strtol(& newCentroVaccinaleRequest->nowDate[6], (char **) NULL, 10) - 1900);
            requestVaccinationDate = mktime(& secondTime);
            
            seconds = difftime(scheduledVaccinationDate, requestVaccinationDate);
            if (seconds <= SECONDS_BETWEEN_TWO_VACCINES) {
                strncpy(newServerV_Reply->vaccineExpirationDate, tempCopiedDate, DATE_LENGTH);
                isVaccineBlocked = TRUE;
            }
            break;
        }
    }
    
    if ((!isVaccineBlocked && healthCardNumberWasFound) || (!isVaccineBlocked && !healthCardNumberWasFound)) {
        strncpy(newServerV_Reply->vaccineExpirationDate, getVaccineExpirationDate(), DATE_LENGTH);
        newServerV_Reply->requestResult = TRUE;
        fclose(originalFilePointer);
        originalFilePointer = fopen(dataPath, "r");
        if (!originalFilePointer) raiseError(FOPEN_SCOPE, FOPEN_ERROR);
        
        // salvo nel file serverV.dat la nuova data di scadenza della vaccinazione
        while ((getLineBytes = getline(& singleLine, & effectiveLineLength, originalFilePointer)) > 0) {
            if ((strncmp(newServerV_Reply->healthCardNumber, singleLine, HEALTH_CARD_NUMBER_LENGTH - 1)) != 0) {
                if (fprintf(tempFilePointer, "%s", singleLine) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
            }
        }
        
        if (fprintf(tempFilePointer, "%s:%s:%s\n", newServerV_Reply->healthCardNumber, newServerV_Reply->vaccineExpirationDate, "1") < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
        updateFile(originalFilePointer, tempFilePointer);
    }
    
    if ((fullWriteReturnValue = fullWrite(* threadConnectionFileDescriptor, (const void *) newServerV_Reply, (size_t) sizeof(serverV_ReplyToCentroVaccinale))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    
    fclose(originalFilePointer);
    pthread_mutex_unlock(& fileSystemAccessMutex);
    fclose(tempFilePointer);
    free(tempCopiedDate);
    free(newCentroVaccinaleRequest);
    free(newServerV_Reply);
    free(threadConnectionFileDescriptor);
    pthread_exit(NULL);
}

void updateFile(FILE * originalFilePointer, FILE * tempFilePointer) {
    fclose(originalFilePointer);
    fclose(tempFilePointer);
    if (remove(dataPath) != 0) raiseError(REMOVE_SCOPE, REMOVE_ERROR);
    if (rename(tempDataPath, dataPath) != 0) raiseError(RENAME_SCOPE, RENAME_ERROR);
    tempFilePointer = fopen(tempDataPath, "w+");
    if (!tempFilePointer) raiseError(FOPEN_SCOPE, FOPEN_ERROR);
    fclose(tempFilePointer);
}

void * clientS_viaServerG_RequestHandler(void * args) {
    int * threadConnectionFileDescriptor = (int *) args;
    ssize_t fullWriteReturnValue, fullReadReturnValue, getLineBytes;
    size_t effectiveLineLength = 0;
    char * singleLine = NULL, * vaccineExpirationDateString, * nowDateString;
    struct tm firstTime, secondTime;
    time_t vaccineExpirationDate, nowDate;
    double seconds;
    char greenPassStatusString[2];
    unsigned short int greenPassStatus;
    enum boolean isGreenPassExpired = TRUE, healthCardNumberWasFound = FALSE, isGreenPassValid = FALSE;
    FILE * originalFilePointer;
    
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    serverV_ReplyToServerG_clientS * newServerV_Reply = (serverV_ReplyToServerG_clientS *) calloc(1, sizeof(serverV_ReplyToServerG_clientS));
    vaccineExpirationDateString = (char *) calloc(DATE_LENGTH, sizeof(char));
    if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!vaccineExpirationDateString) raiseError(CALLOC_SCOPE, CALLOC_ERROR);

    if ((fullReadReturnValue = fullRead(* threadConnectionFileDescriptor, (void *) healthCardNumber, (size_t) HEALTH_CARD_NUMBER_LENGTH * sizeof(char))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    strncpy(newServerV_Reply->healthCardNumber, healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerV_Reply->requestResult = FALSE;
    pthread_mutex_lock(& fileSystemAccessMutex);
    originalFilePointer = fopen(dataPath, "r");
    if (!originalFilePointer) raiseError(FOPEN_SCOPE, FOPEN_ERROR);
    
    // controllo 5 mesi non passati
    while ((getLineBytes = getline(& singleLine, & effectiveLineLength, originalFilePointer)) > 0) {
        if ((strncmp(newServerV_Reply->healthCardNumber, singleLine, HEALTH_CARD_NUMBER_LENGTH - 1)) == 0) {
            healthCardNumberWasFound = TRUE;
            strncpy(vaccineExpirationDateString, singleLine + HEALTH_CARD_NUMBER_LENGTH, DATE_LENGTH);
            memset(& firstTime, 0, sizeof(struct tm));
            memset(& secondTime, 0, sizeof(struct tm));
                       
            firstTime.tm_mday = ((int) strtol(& vaccineExpirationDateString[0], (char **) NULL, 10));
            firstTime.tm_mon = ((int) strtol(& vaccineExpirationDateString[3], (char **) NULL, 10) - 1);
            firstTime.tm_year = ((int) strtol(& vaccineExpirationDateString[6], (char **) NULL, 10) - 1900);
            vaccineExpirationDate = mktime(& firstTime);
            
            nowDateString = getNowDate();
            secondTime.tm_mday = ((int) strtol(& nowDateString[0], (char **) NULL, 10));
            secondTime.tm_mon = ((int) strtol(& nowDateString[3], (char **) NULL, 10) - 1);
            secondTime.tm_year = ((int) strtol(& nowDateString[6], (char **) NULL, 10) - 1900);
            nowDate = mktime(& secondTime);
            
            seconds = difftime(vaccineExpirationDate, nowDate);
            if (seconds <= SECONDS_BETWEEN_TWO_VACCINES && seconds > 0) isGreenPassExpired = FALSE;
            
            strncpy(greenPassStatusString, singleLine + HEALTH_CARD_NUMBER_LENGTH + DATE_LENGTH, 1);
            greenPassStatus = (unsigned short int) strtoul(greenPassStatusString, (char **) NULL, 10);
            if (greenPassStatus) isGreenPassValid = TRUE;
            break;
        }
    }

    if (healthCardNumberWasFound) {
        if (isGreenPassValid && !isGreenPassExpired) newServerV_Reply->requestResult = TRUE;
    }
    
    if ((fullWriteReturnValue = fullWrite(* threadConnectionFileDescriptor, (const void *) newServerV_Reply, (size_t) sizeof(serverV_ReplyToCentroVaccinale))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    
    fclose(originalFilePointer);
    pthread_mutex_unlock(& fileSystemAccessMutex);
    free(vaccineExpirationDateString);
    free(newServerV_Reply);
    free(threadConnectionFileDescriptor);
    pthread_exit(NULL);
}

void * clientT_viaServerG_RequestHandler(void * args) {
    pthread_exit(NULL);
}
