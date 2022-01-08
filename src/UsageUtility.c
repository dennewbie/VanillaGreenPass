//
//  UsageUtility.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 20/10/21.
//

#include "UsageUtility.h"

void checkUsage (int argc, const char * argv[], int expected_argc, const char * expectedUsageMessage) {
    if (argc != expected_argc) {
        if (fprintf(stderr, "Usage: %s %s\n", argv[0], expectedUsageMessage) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
        raiseError(CHECK_USAGE_SCOPE, USAGE_ERROR);
    }
}

void raiseError (char * errorScope, int exitCode) {
    if (fprintf(stderr, "Scope: %s - Error #%d\n", errorScope, exitCode) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    exit(exitCode);
}

void raiseError_herror (char * errorScope, int exitCode) {
    herror(errorScope);
    exit(exitCode);
}

ssize_t fullRead (int fileDescriptor, void * buffer, size_t nBytes) {
    size_t nBytesLeft = nBytes;
    ssize_t nBytesRead;
    
    while (nBytesLeft > 0) {
        if ((nBytesRead = read(fileDescriptor, buffer, nBytesLeft)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
//                raiseError("FULL_READ", (int) nBytesRead);
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
        if ((nBytesWrite = write(fileDescriptor, buffer, nBytesLeft)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
//                raiseError("FULL_WRITE", (int) nBytesWrite);
                 return nBytesWrite;
            }
        }
        
        nBytesLeft -= nBytesWrite;
        buffer += nBytesWrite;
    }
    
    return nBytesLeft;
}

//void createConnectionUserMessage(char * buffer, int nClients) {
//    time_t ticks = time(NULL);
//
//    size_t userMessageSize = snprintf(NULL, 0, "Sei il Client #%d. La data locale e':\n", nClients);
//    if (snprintf(buffer, userMessageSize + 1, "Sei il Client #%d. La data locale e':\n", nClients) < 0) raiseError(SNPRINTF_SCOPE, SNPRINTF_ERROR);
//    if (snprintf(buffer + userMessageSize, BUFFER_SIZE, "%.24s\r\n", ctime(& ticks)) < 0) raiseError(SNPRINTF_SCOPE, SNPRINTF_ERROR);
//}
