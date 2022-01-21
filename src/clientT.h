//
//  clientT.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

// CHECKED
#ifndef clientT_h
#define clientT_h

#include "GreenPassUtility.h"

# define CLIENT_T_ARGS_NO 3



const char * expectedUsageMessage = "<Numero Tessera Sanitaria> <Nuovo Stato Green Pass (0 = NON VALIDO / 1 = VALIDO)>", * configFilePath = "../conf/clientT.conf";

int setupClientT            (int argc,                          char * argv[],                  char ** healthCaardNumber,                  int * newGreenPassStatus);
void updateGreenPass        (int serverG_SocketFileDescriptor,  const void * healthCardNumber,  const unsigned short int newGreenPassStatus                         );

#endif /* clientT_h */
