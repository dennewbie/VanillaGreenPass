//
//  UsageUtility.h
//  NetUilityWrapper
//
//  Created by Denny Caruso on 20/10/21.
//

#ifndef UsageUtility_h
#define UsageUtility_h

#include <stdio.h>
// fprintf, perror

#include <stdlib.h>
// exit

#include <unistd.h>
// read, write, close

#include <errno.h>

#include <string.h>
// strlen

#include <sys/types.h>
#include <sys/socket.h>
// socket, bind, listen, connect

#include <arpa/inet.h>
// struct sockaddr_in

#include <time.h>
// time

#include <signal.h>

#include <netdb.h>
// gethostbyname, gethostbyname2, gethostbyaddr

#include <ctype.h>
#include <pthread.h>


#define BUFFER_SIZE 1023
#define MAX_LINE 255

#define USAGE_ERROR 1
#define CHECK_USAGE_SCOPE "usage"

#define WRITE_ERROR 2
#define WRITE_SCOPE "write"

#define READ_ERROR 3
#define READ_SCOPE "read"

#define FPUTS_ERROR 4
#define FPUTS_SCOPE "fputs"

#define FORK_ERROR 5
#define FORK_SCOPE "fork"

#define SNPRINTF_ERROR 6
#define SNPRINTF_SCOPE "snprintf"

#define FPRINTF_ERROR 7
#define FPRINTF_SCOPE "fprintf"

#define FGETS_ERROR 8
#define FGETS_SCOPE "fgets"

#define MALLOC_SCOPE "malloc"
#define MALLOC_ERROR 9

#define CALLOC_SCOPE "calloc"
#define CALLOC_ERROR 10

#define SELECT_SCOPE "select"
#define SELECT_ERROR 11

#define FULL_WRITE_SCOPE "full_write"
#define FULL_WRITE_ERROR 12

#define FULL_READ_SCOPE "full_read"
#define FULL_READ_ERROR 13

#define FOPEN_SCOPE "fopen"
#define FOPEN_ERROR 14

#define GETLINE_SCOPE "getline"
#define GETLINE_ERROR 15

#define MKTIME_SCOPE "mktime"
#define MKTIME_ERROR 16

#define PTHREAD_CREATE_SCOPE "pthread_create"
#define PTHREAD_CREATE_ERROR 17

#define PTHREAD_MUTEX_INIT_SCOPE "pthread_mutex_init"
#define PTHREAD_MUTEX_INIT_ERROR 18

#define STRPTIME_SCOPE "strptime"
#define STRPTIME_ERROR 19

#define REMOVE_SCOPE "remove"
#define REMOVE_ERROR 20

#define RENAME_SCOPE "rename"
#define RENAME_ERROR 21

#define DUP_SCOPE "dup"
#define DUP_ERROR 22


#define max(x, y) ( { typeof (x) x_ = (x); typeof (y) y_ = (y); x_ > y_ ? x_ : y_; } )


enum boolean        { FALSE, TRUE };
enum isOpen         { CLOSE, OPEN };

void        checkUsage                      (int argc, const char * argv[], int expected_argc, const char * expectedUsageMessage);
void        raiseError                      (char * errorScope, int exitCode);
void        threadRaiseError                (char * errorScope, int exitCode);
void        raiseError_herror               (char * errorScope, int exitCode);
ssize_t     fullRead                        (int fileDescriptor, void * buffer, size_t count);
ssize_t     fullWrite                       (int fileDescriptor, const void * buffer, size_t nBytes);
//void        createConnectionUserMessage     (char * buffer, int nClients);

#endif /* UsageUtility_h */
