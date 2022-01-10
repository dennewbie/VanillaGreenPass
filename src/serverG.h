//
//  serverG.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

#ifndef serverG_h
#define serverG_h

#include "GreenPassUtility.h"



void clientS_RequestHandler     (int connectionFileDescriptor, int serverV_SocketFileDescriptor);
void clientT_RequestHandler     (int connectionFileDescriptor, int serverV_SocketFileDescriptor);

#endif /* serverG_h */
