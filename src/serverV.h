//
//  serverV.h
//  VanillaGreenPass
//
//  Created by Denny Caruso on 08/01/22.
//

#ifndef serverV_h
#define serverV_h

#include "GreenPassUtility.h"



void * centroVaccinaleRequestHandler        (void * args                    );
void * clientS_viaServerG_RequestHandler    (void * args                    );
void * clientT_viaServerG_RequestHandler    (void * args                    );
void updateFile                             (FILE * original, FILE * temp   );

#endif /* serverV_h */
