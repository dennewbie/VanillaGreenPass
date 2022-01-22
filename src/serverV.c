//
//  serverV.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 08/01/22.
//

#include "serverV.h"

int main (int argc, char * argv[]) {
    int listenFileDescriptor, connectionFileDescriptor, enable = TRUE, threadCreationReturnValue;
    pthread_t singleTID;
    pthread_attr_t attr;
    unsigned short int serverV_Port, requestIdentifier;
    struct sockaddr_in serverV_Address, client;

    checkUsage(argc, (const char **) argv, SERVER_V_ARGS_NO, expectedUsageMessage);
    serverV_Port = (unsigned short int) strtoul((const char * restrict) argv[1], (char ** restrict) NULL, 10);
    if (serverV_Port == 0 && (errno == EINVAL || errno == ERANGE)) raiseError(STRTOUL_SCOPE, STRTOUL_ERROR);
    
    listenFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(listenFileDescriptor, SOL_SOCKET, SO_REUSEADDR, & enable, (socklen_t) sizeof(int))  == -1) raiseError(SET_SOCK_OPT_SCOPE, SET_SOCK_OPT_ERROR);
    memset((void *) & serverV_Address, 0, sizeof(serverV_Address));
    memset((void *) & client, 0, sizeof(client));
    serverV_Address.sin_family      = AF_INET;
    serverV_Address.sin_addr.s_addr = htonl(INADDR_ANY);
    serverV_Address.sin_port        = htons(serverV_Port);
    wbind(listenFileDescriptor, (struct sockaddr *) & serverV_Address, (socklen_t) sizeof(serverV_Address));
    wlisten(listenFileDescriptor, LISTEN_QUEUE_SIZE * LISTEN_QUEUE_SIZE);
    
    if (pthread_mutex_init(& fileSystemAccessMutex, (const pthread_mutexattr_t *) NULL) != 0) raiseError(PTHREAD_MUTEX_INIT_SCOPE, PTHREAD_MUTEX_INIT_ERROR);
    if (pthread_mutex_init(& connectionFileDescriptorMutex, (const pthread_mutexattr_t *) NULL) != 0) raiseError(PTHREAD_MUTEX_INIT_SCOPE, PTHREAD_MUTEX_INIT_ERROR);
    if (pthread_attr_init(& attr) != 0) raiseError(PTHREAD_MUTEX_ATTR_INIT_SCOPE, PTHREAD_MUTEX_ATTR_INIT_ERROR);
    if (pthread_attr_setdetachstate(& attr, PTHREAD_CREATE_DETACHED) != 0) raiseError(PTHREAD_ATTR_DETACH_STATE_SCOPE, PTHREAD_ATTR_DETACH_STATE_ERROR);
    
    while (TRUE) {
        ssize_t fullReadReturnValue;
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        connectionFileDescriptor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength);
        if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) & requestIdentifier, sizeof(requestIdentifier))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
        
        if (pthread_mutex_lock(& connectionFileDescriptorMutex) != 0) raiseError(PTHREAD_MUTEX_LOCK_SCOPE, PTHREAD_MUTEX_LOCK_ERROR);
        int * threadConnectionFileDescriptor = (int *) calloc(1, sizeof(int));
        if (!threadConnectionFileDescriptor) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
        if ((* threadConnectionFileDescriptor = dup(connectionFileDescriptor)) < 0) raiseError(DUP_SCOPE, DUP_ERROR);
        if (pthread_mutex_unlock(& connectionFileDescriptorMutex) != 0)  raiseError(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR);
        switch (requestIdentifier) {
            case centroVaccinaleSender:
                // passo sempre lo stesso TID. Non mi interessa fare join. I thread sono detached.
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
    if (pthread_attr_destroy(& attr) != 0) raiseError(PTHREAD_MUTEX_ATTR_DESTROY_SCOPE, PTHREAD_MUTEX_ATTR_DESTROY_ERROR);
    wclose(listenFileDescriptor);
    exit(0);
}

void * centroVaccinaleRequestHandler (void * args) {
    if (pthread_mutex_lock(& connectionFileDescriptorMutex) != 0) threadRaiseError(PTHREAD_MUTEX_LOCK_SCOPE, PTHREAD_MUTEX_LOCK_ERROR);
    int threadConnectionFileDescriptor = * ((int *) args);
    if (pthread_mutex_unlock(& connectionFileDescriptorMutex) != 0) threadRaiseError(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR);
    ssize_t fullWriteReturnValue, fullReadReturnValue, getLineBytes;
    size_t effectiveLineLength = 0;
    char * singleLine = NULL;
    char dateCopiedFromFile[DATE_LENGTH];
    struct tm firstTime, secondTime;
    time_t scheduledVaccinationDate, requestVaccinationDate = 0;
    double elapsedMonths;
    enum boolean isVaccineBlocked = FALSE, healthCardNumberWasFound = FALSE;
    FILE * originalFilePointer, * tempFilePointer;
    
    centroVaccinaleRequestToServerV * newCentroVaccinaleRequest = (centroVaccinaleRequestToServerV *) calloc(1, sizeof(centroVaccinaleRequestToServerV));
    if (!newCentroVaccinaleRequest) threadAbort(CALLOC_SCOPE, CALLOC_ERROR, threadConnectionFileDescriptor, args);
    
    serverV_ReplyToCentroVaccinale * newServerV_Reply = (serverV_ReplyToCentroVaccinale *) calloc(1, sizeof(serverV_ReplyToCentroVaccinale));
    if (!newServerV_Reply) threadAbort(CALLOC_SCOPE, CALLOC_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest);
    
    if ((fullReadReturnValue = fullRead(threadConnectionFileDescriptor, (void *) newCentroVaccinaleRequest, (size_t) sizeof(* newCentroVaccinaleRequest))) != 0) threadAbort(FULL_READ_SCOPE, (int) fullReadReturnValue, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
    strncpy((char *) newServerV_Reply->healthCardNumber, (const char *) newCentroVaccinaleRequest->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerV_Reply->requestResult = FALSE;
    
    if (pthread_mutex_lock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_LOCK_SCOPE, PTHREAD_MUTEX_LOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
    originalFilePointer = fopen(dataPath, "r");
    tempFilePointer = fopen(tempDataPath, "w");
    if (!originalFilePointer || !tempFilePointer) {
        fclose(originalFilePointer);
        fclose(tempFilePointer);
        if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
        threadAbort(FOPEN_SCOPE, FOPEN_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
    }
    
    // controllo 5 mesi non ancora passati
    while ((getLineBytes = getline((char ** restrict) & singleLine, (size_t * restrict) & effectiveLineLength, (FILE * restrict) originalFilePointer)) != -1) {
        if ((strncmp((const char *) newServerV_Reply->healthCardNumber, (const char *) singleLine, HEALTH_CARD_NUMBER_LENGTH - 1)) == 0) {
            healthCardNumberWasFound = TRUE;
            strncpy((char *) dateCopiedFromFile, (const char *) singleLine + HEALTH_CARD_NUMBER_LENGTH, DATE_LENGTH - 1);
            dateCopiedFromFile[DATE_LENGTH - 1] = '\0';
            memset(& firstTime, 0, sizeof(struct tm));
            memset(& secondTime, 0, sizeof(struct tm));
            
            firstTime.tm_mday = (int) strtol((const char * restrict) & dateCopiedFromFile[0], (char ** restrict) NULL, 10);
            firstTime.tm_mon = ((int) strtol((const char * restrict) & dateCopiedFromFile[3], (char ** restrict) NULL, 10) - 1);
            firstTime.tm_year = ((int) strtol((const char * restrict) & dateCopiedFromFile[6], (char ** restrict) NULL, 10) - 1900);
            secondTime.tm_mday = ((int) strtol((const char * restrict) & newCentroVaccinaleRequest->nowDate[0], (char ** restrict) NULL, 10));
            secondTime.tm_mon = ((int) strtol((const char * restrict) & newCentroVaccinaleRequest->nowDate[3], (char ** restrict) NULL, 10) - 1);
            secondTime.tm_year = ((int) strtol((const char * restrict) & newCentroVaccinaleRequest->nowDate[6], (char ** restrict) NULL, 10) - 1900);
            
            if ((firstTime.tm_mday == 0 || firstTime.tm_mon == 0 || firstTime.tm_year == 0 || secondTime.tm_mday == 0 || secondTime.tm_mon == 0 || secondTime.tm_year == 0) && (errno == EINVAL || errno == ERANGE)) {
                fclose(originalFilePointer);
                fclose(tempFilePointer);
                if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
                threadAbort(STRTOL_SCOPE, STRTOL_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
            }
            
            if (((scheduledVaccinationDate = mktime(& firstTime)) == -1) || ((requestVaccinationDate = mktime(& secondTime)) == -1)) {
                    fclose(originalFilePointer);
                    fclose(tempFilePointer);
                    if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
                    threadAbort(MKTIME_SCOPE, MKTIME_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
            }
            
            elapsedMonths = ((((difftime(scheduledVaccinationDate, requestVaccinationDate) / 60) / 60) / 24) / 31);
            if (elapsedMonths <= MONTHS_TO_WAIT_FOR_NEXT_VACCINATION && elapsedMonths > 0) {
                strncpy((char *) newServerV_Reply->vaccineExpirationDate, (const char *) dateCopiedFromFile, DATE_LENGTH);
                isVaccineBlocked = TRUE;
            }
            break;
        }
    }
    
    if (!isVaccineBlocked) {
        strncpy((char *) newServerV_Reply->vaccineExpirationDate, (const char *) getVaccineExpirationDate(), DATE_LENGTH);
        newServerV_Reply->requestResult = TRUE;
        fclose(originalFilePointer);
        originalFilePointer = fopen(dataPath, "r");
        if (!originalFilePointer) {
            fclose(tempFilePointer);
            if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
            threadAbort(FOPEN_SCOPE, FOPEN_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
        }
        
        // salvo nel file serverV.dat la nuova data di scadenza della vaccinazione
        while ((getLineBytes = getline(& singleLine, & effectiveLineLength, originalFilePointer)) != -1) {
            if ((strncmp((const char *) newServerV_Reply->healthCardNumber, (const char *) singleLine, HEALTH_CARD_NUMBER_LENGTH - 1)) != 0) {
                if (fprintf(tempFilePointer, "%s", singleLine) < 0) {
                    fclose(originalFilePointer);
                    fclose(tempFilePointer);
                    if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
                    threadAbort(FPRINTF_SCOPE, FPRINTF_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
                }
            }
        }
        
        if (fprintf(tempFilePointer, "%s:%s:%s\n", newServerV_Reply->healthCardNumber, newServerV_Reply->vaccineExpirationDate, "1") < 0) {
            fclose(originalFilePointer);
            fclose(tempFilePointer);
            if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
            threadAbort(FPRINTF_SCOPE, FPRINTF_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
        }
        
        // updateFile
        fclose(originalFilePointer);
        fclose(tempFilePointer);
        if (remove(dataPath) != 0) {
            if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
            threadAbort(REMOVE_SCOPE, REMOVE_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
        }
        
        if (rename(tempDataPath, dataPath) != 0) {
            if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
            threadAbort(RENAME_SCOPE, RENAME_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
        }
        
        tempFilePointer = fopen(tempDataPath, "w+");
        if (!tempFilePointer) {
            if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
            threadAbort(FOPEN_SCOPE, FOPEN_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
        }
        fclose(tempFilePointer);
    }
    
    if ((fullWriteReturnValue = fullWrite(threadConnectionFileDescriptor, (const void *) newServerV_Reply, (size_t) sizeof(* newServerV_Reply))) != 0) {
        fclose(originalFilePointer);
        fclose(tempFilePointer);
        if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
        threadAbort(FULL_WRITE_SCOPE, (int) fullWriteReturnValue, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
    }
    
    fclose(originalFilePointer);
    fclose(tempFilePointer);
    if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newCentroVaccinaleRequest, newServerV_Reply);
    wclose(threadConnectionFileDescriptor);
    free(args);
    free(newCentroVaccinaleRequest);
    free(newServerV_Reply);
    pthread_exit(NULL);
}

void * clientS_viaServerG_RequestHandler(void * args) {
    if (pthread_mutex_lock(& connectionFileDescriptorMutex) != 0) threadRaiseError(PTHREAD_MUTEX_LOCK_SCOPE, PTHREAD_MUTEX_LOCK_ERROR);
    int threadConnectionFileDescriptor = * ((int *) args);
    if (pthread_mutex_unlock(& connectionFileDescriptorMutex) != 0) threadRaiseError(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR);
    ssize_t fullWriteReturnValue, fullReadReturnValue, getLineBytes;
    size_t effectiveLineLength = 0;
    char * singleLine = NULL;
    struct tm firstTime, secondTime;
    time_t scheduledVaccinationDate = 0, requestVaccinationDate = 0;
    double elapsedMonths;
    unsigned short int greenPassStatus;
    enum boolean isGreenPassExpired = TRUE, healthCardNumberWasFound = FALSE, isGreenPassValid = FALSE;
    FILE * originalFilePointer;
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH], dateCopiedFromFile[DATE_LENGTH], nowDateString[DATE_LENGTH], greenPassStatusString[2];
    
    serverV_ReplyToServerG_clientS * newServerV_Reply = (serverV_ReplyToServerG_clientS *) calloc(1, sizeof(serverV_ReplyToServerG_clientS));
    if (!newServerV_Reply) threadAbort(CALLOC_SCOPE, CALLOC_ERROR, threadConnectionFileDescriptor, args);
    
    if ((fullReadReturnValue = fullRead(threadConnectionFileDescriptor, (void *) healthCardNumber, (size_t) HEALTH_CARD_NUMBER_LENGTH * sizeof(char))) != 0) threadAbort(FULL_READ_SCOPE, (int) fullReadReturnValue, threadConnectionFileDescriptor, args, newServerV_Reply);
    
    strncpy((char *) newServerV_Reply->healthCardNumber, (const char *) healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerV_Reply->requestResult = FALSE;
    
    if (pthread_mutex_lock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_LOCK_SCOPE, PTHREAD_MUTEX_LOCK_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
    originalFilePointer = fopen(dataPath, "r");
    if (!originalFilePointer) {
        if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
        threadAbort(FOPEN_SCOPE, FOPEN_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
    }
    
    while ((getLineBytes = getline(& singleLine, & effectiveLineLength, originalFilePointer)) != -1) {
        if ((strncmp((const char *) newServerV_Reply->healthCardNumber, (const char *) singleLine, HEALTH_CARD_NUMBER_LENGTH - 1)) == 0) {
            healthCardNumberWasFound = TRUE;
            strncpy((char *) dateCopiedFromFile, (const char *) singleLine + HEALTH_CARD_NUMBER_LENGTH, DATE_LENGTH - 1);
            dateCopiedFromFile[DATE_LENGTH - 1] = '\0';
            memset(& firstTime, 0, sizeof(struct tm));
            memset(& secondTime, 0, sizeof(struct tm));
            
            firstTime.tm_mday = (int) strtol((const char * restrict) & dateCopiedFromFile[0], (char ** restrict) NULL, 10);
            firstTime.tm_mon = ((int) strtol((const char * restrict) & dateCopiedFromFile[3], (char ** restrict) NULL, 10) - 1);
            firstTime.tm_year = ((int) strtol((const char * restrict) & dateCopiedFromFile[6], (char ** restrict) NULL, 10) - 1900);
            strncpy((char *) nowDateString, (const char *) getNowDate(), DATE_LENGTH);
            secondTime.tm_mday = ((int) strtol((const char * restrict) & nowDateString[0], (char ** restrict) NULL, 10));
            secondTime.tm_mon = ((int) strtol((const char * restrict) & nowDateString[3], (char ** restrict) NULL, 10) - 1);
            secondTime.tm_year = ((int) strtol((const char * restrict) & nowDateString[6], (char ** restrict) NULL, 10) - 1900);
            
            if ((firstTime.tm_mday == 0 || firstTime.tm_mon == 0 || firstTime.tm_year == 0 || secondTime.tm_mday == 0 || secondTime.tm_mon == 0 || secondTime.tm_year == 0) && (errno == EINVAL || errno == ERANGE)) {
                fclose(originalFilePointer);
                if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
                threadAbort(STRTOL_SCOPE, STRTOL_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
            }
            
            if (((scheduledVaccinationDate = mktime(& firstTime)) == -1) || ((requestVaccinationDate = mktime(& secondTime)) == -1)) {
                    fclose(originalFilePointer);
                    if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
                    threadAbort(MKTIME_SCOPE, MKTIME_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
            }
            
            elapsedMonths = ((((difftime(scheduledVaccinationDate, requestVaccinationDate) / 60) / 60) / 24) / 31);
            if (elapsedMonths <= MONTHS_TO_WAIT_FOR_NEXT_VACCINATION && elapsedMonths > 0) isGreenPassExpired = FALSE;
            strncpy((char *) greenPassStatusString, (const char *) singleLine + HEALTH_CARD_NUMBER_LENGTH + DATE_LENGTH, 1);
            greenPassStatusString[1] = '\0';
            greenPassStatus = (unsigned short int) strtoul((const char * restrict) greenPassStatusString, (char ** restrict) NULL, 10);
            if ((greenPassStatus == FALSE) && (errno == EINVAL || errno == ERANGE)) {
                fclose(originalFilePointer);
                if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
                threadAbort(STRTOUL_SCOPE, STRTOUL_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
            }
            if (greenPassStatus == TRUE) isGreenPassValid = TRUE;
            break;
        }
    }
    
    fclose(originalFilePointer);
    if (pthread_mutex_unlock(& fileSystemAccessMutex) != 0) threadAbort(PTHREAD_MUTEX_UNLOCK_SCOPE, PTHREAD_MUTEX_UNLOCK_ERROR, threadConnectionFileDescriptor, args, newServerV_Reply);
    
    if (healthCardNumberWasFound) if (isGreenPassValid && !isGreenPassExpired) newServerV_Reply->requestResult = TRUE;
    if ((fullWriteReturnValue = fullWrite(threadConnectionFileDescriptor, (const void *) newServerV_Reply, (size_t) sizeof(serverV_ReplyToServerG_clientS))) != 0) threadAbort(FULL_WRITE_SCOPE, (int) fullWriteReturnValue, threadConnectionFileDescriptor, args, newServerV_Reply);
    
    free(newServerV_Reply);
    free(args);
    wclose(threadConnectionFileDescriptor);
    pthread_exit(NULL);
}

void * clientT_viaServerG_RequestHandler(void * args) {
    pthread_mutex_lock(& connectionFileDescriptorMutex);
    int threadConnectionFileDescriptor = * ((int *) args);
    pthread_mutex_unlock(& connectionFileDescriptorMutex);
    ssize_t fullWriteReturnValue, fullReadReturnValue, getLineBytes;
    size_t effectiveLineLength = 0;
    char * singleLine = NULL, * vaccineExpirationDateString;
    enum boolean healthCardNumberWasFound = FALSE;
    FILE * originalFilePointer, * tempFilePointer = NULL;
    
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    serverG_RequestToServerV_onBehalfOfClientT * newServerG_Request = (serverG_RequestToServerV_onBehalfOfClientT *) calloc(1, sizeof(serverG_RequestToServerV_onBehalfOfClientT));
    if (!newServerG_Request) {
        free(args);
        threadRaiseError(CALLOC_SCOPE, CALLOC_ERROR);
    }
    
    serverV_ReplyToServerG_clientT * newServerV_Reply = (serverV_ReplyToServerG_clientT *) calloc(1, sizeof(serverV_ReplyToServerG_clientT));
    if (!newServerV_Reply) {
        free(newServerG_Request);
        free(args);
        threadRaiseError(CALLOC_SCOPE, CALLOC_ERROR);
    }
    
    vaccineExpirationDateString = (char *) calloc(DATE_LENGTH, sizeof(char));
    if (!vaccineExpirationDateString) {
        free(newServerG_Request);
        free(newServerV_Reply);
        free(args);
        threadRaiseError(CALLOC_SCOPE, CALLOC_ERROR);
    }
    
    if ((fullReadReturnValue = fullRead(threadConnectionFileDescriptor, (void *) newServerG_Request, (size_t) sizeof(serverG_RequestToServerV_onBehalfOfClientT))) != 0) {
        free(vaccineExpirationDateString);
        free(newServerV_Reply);
        free(newServerG_Request);
        free(args);
        threadRaiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    }
    
    strncpy((char *) healthCardNumber, (const char *) newServerG_Request->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    strncpy((char *) newServerV_Reply->healthCardNumber, (const char *) healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerV_Reply->updateResult = FALSE;
    
    pthread_mutex_lock(& fileSystemAccessMutex);
    originalFilePointer = fopen(dataPath, "r");
    if (!originalFilePointer) {
        free(vaccineExpirationDateString);
        free(newServerV_Reply);
        free(newServerG_Request);
        free(args);
        pthread_mutex_unlock(& fileSystemAccessMutex);
        threadRaiseError(FOPEN_SCOPE, FOPEN_ERROR);
    }
    
    while ((getLineBytes = getline(& singleLine, & effectiveLineLength, originalFilePointer)) > 0) {
        if ((strncmp(newServerV_Reply->healthCardNumber, singleLine, HEALTH_CARD_NUMBER_LENGTH - 1)) == 0) {
            healthCardNumberWasFound = TRUE;
            // fix here strncpy
            strncpy((char *) vaccineExpirationDateString, (const char *) singleLine + HEALTH_CARD_NUMBER_LENGTH, DATE_LENGTH - 1);
            break;
        }
    }
    
    if (healthCardNumberWasFound) {
        fclose(originalFilePointer);
        originalFilePointer = fopen(dataPath, "r");
        tempFilePointer = fopen(tempDataPath, "w");
        if (!originalFilePointer || !tempFilePointer) {
            fclose(originalFilePointer);
            fclose(tempFilePointer);
            free(vaccineExpirationDateString);
            free(args);
            free(newServerV_Reply);
            free(newServerG_Request);
            pthread_mutex_unlock(& fileSystemAccessMutex);
            threadRaiseError(FOPEN_SCOPE, FOPEN_ERROR);
        }
        
        // salvo nel file serverV.dat il nuovo stato del green pass
        while ((getLineBytes = getline(& singleLine, & effectiveLineLength, originalFilePointer)) > 0) {
            if ((strncmp(newServerV_Reply->healthCardNumber, singleLine, HEALTH_CARD_NUMBER_LENGTH - 1)) != 0) {
                if (fprintf(tempFilePointer, "%s", singleLine) < 0) {
                    fclose(originalFilePointer);
                    fclose(tempFilePointer);
                    free(vaccineExpirationDateString);
                    free(args);
                    free(newServerV_Reply);
                    free(newServerG_Request);
                    pthread_mutex_unlock(& fileSystemAccessMutex);
                    threadRaiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
                }
            }
        }
        
        if (fprintf(tempFilePointer, "%s:%s:%hu\n", newServerV_Reply->healthCardNumber, vaccineExpirationDateString, newServerG_Request->updateValue) < 0) {
            fclose(originalFilePointer);
            fclose(tempFilePointer);
            free(vaccineExpirationDateString);
            free(newServerV_Reply);
            free(newServerG_Request);
            free(args);
            pthread_mutex_unlock(& fileSystemAccessMutex);
            threadRaiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
        }
        
        // updateFile
        fclose(originalFilePointer);
        fclose(tempFilePointer);
        if (remove(dataPath) != 0) {
            fclose(originalFilePointer);
            fclose(tempFilePointer);
            free(vaccineExpirationDateString);
            free(newServerG_Request);
            free(newServerV_Reply);
            free(args);
            pthread_mutex_unlock(& fileSystemAccessMutex);
            threadRaiseError(REMOVE_SCOPE, REMOVE_ERROR);
        }
        
        if (rename(tempDataPath, dataPath) != 0) {
            fclose(originalFilePointer);
            fclose(tempFilePointer);
            free(vaccineExpirationDateString);
            free(newServerG_Request);
            free(newServerV_Reply);
            free(args);
            pthread_mutex_unlock(& fileSystemAccessMutex);
            threadRaiseError(RENAME_SCOPE, RENAME_ERROR);
        }
        
        tempFilePointer = fopen(tempDataPath, "w+");
        if (!tempFilePointer) {
            fclose(originalFilePointer);
            fclose(tempFilePointer);
            free(vaccineExpirationDateString);
            free(newServerG_Request);
            free(newServerV_Reply);
            free(args);
            pthread_mutex_unlock(& fileSystemAccessMutex);
            threadRaiseError(FOPEN_SCOPE, FOPEN_ERROR);
        }
        newServerV_Reply->updateResult = TRUE;
    }
    
    if ((fullWriteReturnValue = fullWrite(threadConnectionFileDescriptor, (const void *) newServerV_Reply, (size_t) sizeof(serverV_ReplyToServerG_clientT))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    
    fclose(originalFilePointer);
    fclose(tempFilePointer);
    pthread_mutex_unlock(& fileSystemAccessMutex);
    free(vaccineExpirationDateString);
    free(newServerV_Reply);
    free(newServerG_Request);
    free(args);
    pthread_exit(NULL);
}

void threadAbort (char * errorScope, int exitCode, int threadConnectionFileDescriptor, void * arg1, ...) {
    wclose(threadConnectionFileDescriptor);
    va_list argumentsList;
    void * currentElement;
    free(arg1);
    va_start(argumentsList, arg1);
    while ((currentElement = va_arg(argumentsList, void *)) != 0) free(currentElement);
    va_end(argumentsList);
    threadRaiseError(errorScope, exitCode);
}
