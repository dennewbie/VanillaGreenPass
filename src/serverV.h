//
//  serverV.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 08/01/22.
//

#ifndef serverV_h
#define serverV_h

#include "GreenPassUtility.h"

#define SERVER_V_ARGS_NO 2



pthread_mutex_t fileSystemAccessMutex;
pthread_mutex_t connectionFileDescriptorMutex;
const char * dataPath =     "../data/serverV.dat";
const char * tempDataPath = "../data/tempServerV.dat";
const char * expectedUsageMessage = "<ServerV Port>";

void * centroVaccinaleRequestHandler        (void * args                    );
void * clientS_viaServerG_RequestHandler    (void * args                    );
void * clientT_viaServerG_RequestHandler    (void * args                    );
void updateFile                             (FILE * original, FILE * temp   );

#endif /* serverV_h */
