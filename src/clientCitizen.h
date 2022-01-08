//
//  clientCitizen.h
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 07/01/22.
//

#ifndef clientCitizen_h
#define clientCitizen_h

#include "GreenPassUtility.h"



typedef struct {
    time_t vaccineExpirationDate;
    enum boolean requestResult;
} centroVaccinaleReply;

void getVaccination(int serverSocketFileDescriptor, const void * healthCardNumber, size_t nBytes);

#endif /* clientCitizen_h */
