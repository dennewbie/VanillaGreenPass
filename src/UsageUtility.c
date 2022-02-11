//
//  UsageUtility.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 20/10/21.
//

#include "UsageUtility.h"

// Controllo dei parametri di ogni entità software invocata
void checkUsage (int argc, const char * argv[], int expected_argc, const char * expectedUsageMessage) {
    if (argc != expected_argc) {
        if (fprintf(stderr, (const char * restrict) "Usage: %s %s\n", argv[0], expectedUsageMessage) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
        raiseError(CHECK_USAGE_SCOPE, CHECK_USAGE_ERROR);
    }
}

/*
 Procedura per la gestione degli errori più comuni con la visualizzazione dello scope e conseguente
 terminazione del processo.
 */
void raiseError (char * errorScope, int exitCode) {
    if (fprintf(stderr, (const char * restrict) "Scope: %s - Error #%d\n", errorScope, exitCode) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    exit(exitCode);
}

/*
 Procedura per la gestione degli errori più comuni con la visualizzazione dello scope e conseguente
 terminazione del thread.
 */
void threadRaiseError (char * errorScope, int exitCode) {
    if (fprintf(stderr, (const char * restrict)  "Scope: %s - Error #%d\n", errorScope, exitCode) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    pthread_exit(NULL);
}

ssize_t fullRead (int fileDescriptor, void * buffer, size_t nBytes) {
    size_t nBytesLeft = nBytes;
    ssize_t nBytesRead;
    
    while (nBytesLeft > 0) {
        // == -1
        if ((nBytesRead = read(fileDescriptor, buffer, nBytesLeft)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                return nBytesRead;
            }
        } else if (nBytesRead == 0) {
            break;
        }
        
        nBytesLeft -= nBytesRead;
        buffer += nBytesRead;
    }
    
    return nBytesLeft;
}

ssize_t fullWrite (int fileDescriptor, const void * buffer, size_t nBytes) {
    size_t nBytesLeft = nBytes;
    ssize_t nBytesWrite;
    
    while (nBytesLeft > 0) {
        // == -1
        if ((nBytesWrite = write(fileDescriptor, buffer, nBytesLeft)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                return nBytesWrite;
            }
        }
        
        nBytesLeft -= nBytesWrite;
        buffer += nBytesWrite;
    }
    
    return nBytesLeft;
}
