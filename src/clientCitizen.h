//
//  clientCitizen.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 07/01/22.
//

#ifndef clientCitizen_h
#define clientCitizen_h

#include "GreenPassUtility.h"



# define CLIENT_CITIZEN_ARGS_NO 2



const char * expectedUsageMessage = "<Numero Tessera Sanitaria>", * configFilePath = "../conf/clientCitizen.conf";

int setupClientCitizen      (int argc,                                  char * argv[],                  char ** healthCardNumber    );
void getVaccination         (int centroVaccinaleSocketFileDescriptor,   const void * healthCardNumber,  size_t nBytes               );

#endif /* clientCitizen_h */
