//
//  serverV.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 08/01/22.
//

#include "serverV.h"

pthread_mutex_t fileSystemAccessMutex;
pthread_mutex_t connectionFileDescriptorMutex;
FILE * filePointer;
int connectionFileDescriptor;
const char * dataPath = "../data/serverV.dat";

int main (int argc, char * argv[]) {
    int listenFileDescriptor, enable = TRUE;
    pthread_t * tids;
    unsigned int threadCounter = 0;
    unsigned short int serverV_Port;
    struct sockaddr_in serverV_Address, client;
//    const char * configFilePathServerV = "../conf/serverV.conf";
    const char * expectedUsageMessage = "<ServerV Port>";
    pid_t childPid;
    
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
    
    tids = (pthread_t *) calloc(BUFFER_SIZE, sizeof(pthread_t));
    if (!tids) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (pthread_mutex_init(& fileSystemAccessMutex, NULL) != 0) raiseError(PTHREAD_MUTEX_INIT_SCOPE, PTHREAD_MUTEX_INIT_ERROR);
    if (pthread_mutex_init(& connectionFileDescriptorMutex, NULL) != 0) raiseError(PTHREAD_MUTEX_INIT_SCOPE, PTHREAD_MUTEX_INIT_ERROR);
    
    while (TRUE) {
        unsigned short int requestIdentifier;
        int threadCreationReturnValue;
        ssize_t fullWriteReturnValue, fullReadReturnValue;
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        connectionFileDescriptor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength);
        
        if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) & requestIdentifier, (size_t) sizeof(requestIdentifier))) < 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
        switch (requestIdentifier) {
            case centroVaccinaleSender:
                if ((threadCreationReturnValue = pthread_create(& tids[threadCounter], NULL, & centroVaccinaleRequestHandler, NULL)) != 0) raiseError(PTHREAD_CREATE_SCOPE, PTHREAD_CREATE_ERROR);
                ++threadCounter;
                break;
            case clientS_viaServerG_Sender:
                break;
            case clientT_viaServerG_Sender:
                break;
            default:
                raiseError(INVALID_SENDER_ID_SCOPE, INVALID_SENDER_ID_ERROR);
                break;
        }
        // full READ identifier
//        if ((childPid = fork()) < 0) {
//            raiseError(FORK_SCOPE, FORK_ERROR);
//        } else if (childPid == 0) {
//            wclose(listenFileDescriptor);
//
            
            // TO DO serverV activities: mutex, thread, write on file system
//            wclose(connectionFileDescriptor);
//            exit(0);
//        }
        wclose(connectionFileDescriptor);
    }
    
    // codice mai eseguito
    wclose(listenFileDescriptor);
    free(tids);
    exit(0);
}

void * centroVaccinaleRequestHandler () {
    ssize_t fullWriteReturnValue, fullReadReturnValue, getLineBytes;
    char * singleLine = NULL;
    char * tempCopiedDate;
    size_t effectiveLineLength = 0;
    
    centroVaccinaleRequestToServerV * newCentroVaccinaleRequest = (centroVaccinaleRequestToServerV *) calloc(1, sizeof(centroVaccinaleRequestToServerV));
    serverV_ReplyToCentroVaccinale * newServerV_Reply = (serverV_ReplyToCentroVaccinale *) calloc(1, sizeof(serverV_ReplyToCentroVaccinale));
    tempCopiedDate = (char *) calloc(DATE_LENGTH, sizeof(char));
    
    if (!newCentroVaccinaleRequest) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!tempCopiedDate) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    pthread_mutex_lock(& fileSystemAccessMutex);
    pthread_mutex_lock(& connectionFileDescriptorMutex);
    
    filePointer = fopen(dataPath, "r+");
    if (!filePointer) raiseError(FOPEN_SCOPE, FOPEN_ERROR);
    if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) newCentroVaccinaleRequest, (size_t) sizeof(centroVaccinaleRequestToServerV))) < 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    while ((getLineBytes = getline(& singleLine, & effectiveLineLength, filePointer)) > 0) {
        if ((strncmp(newCentroVaccinaleRequest->healthCardNumber, singleLine, 20)) == 0) {
            unsigned int expirationDateLength = (unsigned int) strlen(singleLine) - 20;
            strncpy(tempCopiedDate, & singleLine[HEALTH_CARD_NUMBER_LENGTH + 2], expirationDateLength);
            
            
        }
    }
    
    
    
    fclose(filePointer);
    pthread_mutex_unlock(& connectionFileDescriptorMutex);
    pthread_mutex_unlock(& fileSystemAccessMutex);
    free(tempCopiedDate);
    free(newCentroVaccinaleRequest);
    free(newServerV_Reply);
    return NULL;
}
