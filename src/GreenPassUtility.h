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
#define HEALTH_CARD_NUMBER_LENGTH 20
#define MONTHS_IN_A_YEAR 12



typedef struct {
    time_t vaccineExpirationDate;
    enum boolean requestResult;
} centroVaccinaleReply;

void checkHealtCardNumber           (char * healthCardNumber);
void retrieveConfigurationData      (const char * configFilePath, char ** configurationIP, unsigned short int * configurationPort);
time_t getVaccineExpirationDate     (void);

#endif /* GreenPassUtility_h */
