//
//  GreenPassUtility.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 07/01/22.
//

#include "GreenPassUtility.h"

void checkHealtCardNumber(char * healthCardNumber) {
    size_t healthCardNumberLength = strlen(healthCardNumber);
    if (healthCardNumberLength > 20 || healthCardNumberLength <= 0) raiseError(CHECK_HEALTH_CARD_NUMBER_SCOPE, CHECK_HEALTH_CARD_NUMBER_ERROR);
}
