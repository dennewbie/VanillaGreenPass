//
//  GreenPassUtility.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 07/01/22.
//

#ifndef GreenPassUtility_h
#define GreenPassUtility_h

#include "NetWrapper.h"

#define CHECK_HEALTH_CARD_NUMBER_SCOPE "checkHealthCardNumber"
#define CHECK_HEALTH_CARD_NUMBER_ERROR 200

#define INVALID_SENDER_ID_SCOPE "invalidSenderID_serverV"
#define INVALID_SENDER_ID_ERROR 201

#define INVALID_UPDATE_STATUS_SCOPE "invalidGreenPassUpdateStatus"
#define INVALID_UPDATE_STATUS_ERROR 202

#define MONTHS_TO_WAIT_FOR_NEXT_VACCINATION 5
#define HEALTH_CARD_NUMBER_LENGTH 21
#define DATE_LENGTH 11
#define MONTHS_IN_A_YEAR 12
#define SECONDS_BETWEEN_TWO_VACCINES 60*60*24*30*5



typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    char vaccineExpirationDate[DATE_LENGTH];
    unsigned short int requestResult;
} centroVaccinaleReplyToClientCitizen;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    char nowDate[DATE_LENGTH];
} centroVaccinaleRequestToServerV;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    char vaccineExpirationDate[DATE_LENGTH];
    unsigned short int requestResult;
} serverV_ReplyToCentroVaccinale;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    unsigned short int requestResult;
} serverV_ReplyToServerG_clientS;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    unsigned short int updateValue;
} serverG_RequestToServerV_onBehalfOfClientT;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    unsigned short int updateResult;
} serverV_ReplyToServerG_clientT;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    unsigned short int requestResult;
} serverG_ReplyToClientS;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    unsigned short int updateValue;
} clientT_RequestToServerG;

typedef struct {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    unsigned short int updateResult;
} serverG_ReplyToClientT;

enum sender {
    centroVaccinaleSender,
    clientS_viaServerG_Sender,
    clientT_viaServerG_Sender
};

void checkHealtCardNumber           (char * healthCardNumber);
void retrieveConfigurationData      (const char * configFilePath, char ** configurationIP, unsigned short int * configurationPort);
char * getVaccineExpirationDate     (void);
char * getNowDate                   (void);
int createConnectionWithServerV   (const char * configFilePath);

#endif /* GreenPassUtility_h */
