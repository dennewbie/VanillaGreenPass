//
//  serverG.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

#ifndef serverG_h
#define serverG_h

#include "GreenPassUtility.h"



#define SERVER_G_ARGS_NO 2



const char * expectedUsageMessage = "<ServerG Port>", * configFilePathServerG = "../conf/serverG.conf";

void clientS_RequestHandler     (int connectionFileDescriptor, int serverV_SocketFileDescriptor);
void clientT_RequestHandler     (int connectionFileDescriptor, int serverV_SocketFileDescriptor);

#endif /* serverG_h */
