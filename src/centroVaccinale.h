//
//  centroVaccinale.h
//  VanillaGreenPass
//
//  Created by Denny Caruso and Francesco Calcopietro on 08/01/22.
//

#ifndef centroVaccinale_h
#define centroVaccinale_h

#include "GreenPassUtility.h"



void clientCitizenRequestHandler (int connectionFileDescriptor, int serverV_SocketFileDescriptor, centroVaccinaleReplyToClientCitizen * newCentroVaccinaleReply, centroVaccinaleRequestToServerV * newCentroVaccinaleRequest, serverV_ReplyToCentroVaccinale * newServerV_Reply);

#endif /* centroVaccinale_h */
