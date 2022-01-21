//
//  clientCitizen.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 07/01/22.
//

// CHECKED
#ifndef clientCitizen_h
#define clientCitizen_h

#include "GreenPassUtility.h"

# define CLIENT_CITIZEN_ARGS_NO 2



int setupClientCitizen      (int argc,                                  char * argv[],                  char ** healthCardNumber    );
void getVaccination         (int centroVaccinaleSocketFileDescriptor,   const void * healthCardNumber,  size_t nBytes               );

#endif /* clientCitizen_h */
