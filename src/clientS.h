//
//  clientS.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

// CHECKED 
#ifndef clientS_h
#define clientS_h

#include "GreenPassUtility.h"

# define CLIENT_S_ARGS_NO 2



int setupClientS        (int argc,                          char * argv[],                  char ** healthCardNumber);
void checkGreenPass     (int serverG_SocketFileDescriptor,  const void * healthCardNumber,  size_t nBytes           );

#endif /* clientS_h */
