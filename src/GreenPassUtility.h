//
//  GreenPassUtility.h
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 07/01/22.
//

#ifndef GreenPassUtility_h
#define GreenPassUtility_h

#include "NetWrapper.h"

#define CHECK_HEALTH_CARD_NUMBER_SCOPE "checkHealthCardNumber"
#define CHECK_HEALTH_CARD_NUMBER_ERROR 200

#define MONTHS_TO_WAIT_FOR_NEXT_VACCINATION 5



void checkHealtCardNumber(char * healthCardNumber);

#endif /* GreenPassUtility_h */
