//
//  clientT.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

#ifndef clientT_h
#define clientT_h

#include "GreenPassUtility.h"

void updateGreenPass     (int serverG_SocketFileDescriptor, const void * healthCardNumber, const unsigned short int newGreenPassStatus);

#endif /* clientT_h */
