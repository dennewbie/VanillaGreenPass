//
//  UsageUtility.h
//  NetUilityWrapper
//
//  Created by Denny Caruso on 20/10/21.
//

// CHECKED
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

#include <ctype.h>
#include <pthread.h>



#define CHECK_USAGE_ERROR 1
#define CHECK_USAGE_SCOPE "usage"

#define WRITE_ERROR 2
#define WRITE_SCOPE "write"

#define READ_ERROR 3
#define READ_SCOPE "read"

#define FORK_ERROR 4
#define FORK_SCOPE "fork"

#define FPRINTF_ERROR 5
#define FPRINTF_SCOPE "fprintf"

#define CALLOC_ERROR 6
#define CALLOC_SCOPE "calloc"

#define FULL_WRITE_ERROR 7
#define FULL_WRITE_SCOPE "full_write"

#define FULL_READ_ERROR 8
#define FULL_READ_SCOPE "full_read"

#define FOPEN_ERROR 9
#define FOPEN_SCOPE "fopen"

#define GETLINE_ERROR 10
#define GETLINE_SCOPE "getline"

#define MKTIME_ERROR 11
#define MKTIME_SCOPE "mktime"

#define PTHREAD_CREATE_ERROR 12
#define PTHREAD_CREATE_SCOPE "pthread_create"

#define PTHREAD_MUTEX_INIT_ERROR 13
#define PTHREAD_MUTEX_INIT_SCOPE "pthread_mutex_init"

#define PTHREAD_MUTEX_ATTR_INIT_ERROR 14
#define PTHREAD_MUTEX_ATTR_INIT_SCOPE "pthread_mutex_attr_init"

#define PTHREAD_ATTR_DETACH_STATE_ERROR 15
#define PTHREAD_ATTR_DETACH_STATE_SCOPE "pthread_attr_setdetachstate"

#define REMOVE_ERROR 16
#define REMOVE_SCOPE "remove"

#define RENAME_ERROR 17
#define RENAME_SCOPE "rename"

#define DUP_ERROR 18
#define DUP_SCOPE "dup"

#define STRTOUL_ERROR 19
#define STRTOUL_SCOPE "strtoul"

#define max(x, y) ( { typeof (x) x_ = (x); typeof (y) y_ = (y); x_ > y_ ? x_ : y_; } )



enum boolean        { FALSE, TRUE };

void        checkUsage          (int argc,              const char * argv[],    int expected_argc, const char * expectedUsageMessage);
void        raiseError          (char * errorScope,     int exitCode                                                                );
void        threadRaiseError    (char * errorScope,     int exitCode                                                                );
ssize_t     fullRead            (int fileDescriptor,    void * buffer,          size_t nBytes                                       );
ssize_t     fullWrite           (int fileDescriptor,    const void * buffer,    size_t nBytes                                       );

#endif /* UsageUtility_h */
