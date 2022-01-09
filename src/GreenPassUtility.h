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

#define INVALID_SENDER_ID_SCOPE "invalidSenderID_serverV"
#define INVALID_SENDER_ID_ERROR 201

#define MONTHS_TO_WAIT_FOR_NEXT_VACCINATION 5
#define HEALTH_CARD_NUMBER_LENGTH 20
#define DATE_LENGTH 10
#define MONTHS_IN_A_YEAR 12
#define SECONDS_BETWEEN_TWO_VACCINES 60*60*24*30*5



typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    char vaccineExpirationDate[DATE_LENGTH];
    enum boolean requestResult;
} centroVaccinaleReplyToClientCitizen;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    char vaccineExpirationDate[DATE_LENGTH];
} centroVaccinaleRequestToServerV;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    char vaccineExpirationDate[DATE_LENGTH];
    enum boolean requestResult;
} serverV_ReplyToCentroVaccinale;

enum sender {
    centroVaccinaleSender,
    clientS_viaServerG_Sender,
    clientT_viaServerG_Sender
};

void checkHealtCardNumber           (char * healthCardNumber);
void retrieveConfigurationData      (const char * configFilePath, char ** configurationIP, unsigned short int * configurationPort);
time_t getVaccineExpirationDate     (void);
char * getNowDate                   (void);

#endif /* GreenPassUtility_h */
