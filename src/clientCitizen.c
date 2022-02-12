//
//  clientCitizen.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 07/01/22.
//

#include "clientCitizen.h"

int main (int argc, char * argv[]) {
    char * healthCardNumber;
    
    /*
    Si controlla, tramite la funzione seguente, il numero di argomenti passati via terminale e se il formato
    del codice ditessera sanitaria scelto dall'utente rispetta il formato previsto.
    Si crea un socket file descritpor, si effettua il collegamento col CentroVaccinale e si restituisce il socket
    file descriptor al chiamante.
    */
    int centroVaccinaleSocketFileDescriptor = setupClientCitizen(argc, argv, & healthCardNumber);
    healthCardNumber[HEALTH_CARD_NUMBER_LENGTH - 1] = '\0';
    
    
    /*
    Si effettua la richiesta di vaccinazione e di ottenimento del Vanilla Green Pass.
    "getVaccination(...)" avrà come parametri di ingresso: "centroVaccinaleFileDescriptor",
    il codice della tessera sanitaria e la dimensione di quest'ultima.
    */
    getVaccination(centroVaccinaleSocketFileDescriptor, (const void *) healthCardNumber, (size_t) sizeof(char) * HEALTH_CARD_NUMBER_LENGTH);
    wclose(centroVaccinaleSocketFileDescriptor);
    free(healthCardNumber);
    exit(0);
}

int setupClientCitizen (int argc, char * argv[], char ** healthCardNumber) {
    int centroVaccinaleSocketFileDescriptor;
    struct sockaddr_in centroVaccinaleAddress;
    char * stringCentroVaccinaleAddressIP = NULL;
    unsigned short int centroVaccinalePort;
    
    // Si verifica che il ClientCitizen sia stato avviato con i parametri che si aspetta di avere.
    checkUsage(argc, (const char **) argv, CLIENT_CITIZEN_ARGS_NO, expectedUsageMessage);
    // Si verifica che il codice di tessera sanitaria immesso sia del formato e della lunghezza giusta.
    checkHealtCardNumber(argv[1]);
    
    
    /*
    Si alloca la giusta quantità di memoria per "healthCardNumber" e si controlla che ciò sia stato fatto
    in maniera corretta.
    */
    * healthCardNumber = (char *) calloc(HEALTH_CARD_NUMBER_LENGTH, sizeof(char));
    if (! * healthCardNumber) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    
    // Si copia il valore passato in ingresso al ClientCitizen nella stringa definita e allocata "healthCardNumber"
    strncpy(* healthCardNumber, (const char *) argv[1], HEALTH_CARD_NUMBER_LENGTH - 1);
    
    /*
    Si ricavano, a partire dal file di configurazione definito in "configFilePath", i parametri fondamentali per contattare
    il CentroVaccinale.
    */
    retrieveConfigurationData(configFilePath, & stringCentroVaccinaleAddressIP, & centroVaccinalePort);
    
    centroVaccinaleSocketFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    memset((void *) & centroVaccinaleAddress, 0, sizeof(centroVaccinaleAddress));
    centroVaccinaleAddress.sin_family = AF_INET;
    centroVaccinaleAddress.sin_port   = htons(centroVaccinalePort);
    if (inet_pton(AF_INET, (const char * restrict) stringCentroVaccinaleAddressIP, (void *) & centroVaccinaleAddress.sin_addr) <= 0) raiseError(INET_PTON_SCOPE, INET_PTON_ERROR);
    
    wconnect(centroVaccinaleSocketFileDescriptor, (struct sockaddr *) & centroVaccinaleAddress, (socklen_t) sizeof(centroVaccinaleAddress));
    if (fprintf(stdout, "\nBenvenuti al Centro Vaccinale\nNumero tessera sanitaria: %s\n\n... A breve ti verra' inoculato il vaccino...\n", * healthCardNumber) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    free(stringCentroVaccinaleAddressIP);
    return centroVaccinaleSocketFileDescriptor;
}

void getVaccination (int centroVaccinaleSocketFileDescriptor, const void * healthCardNumber, size_t nBytes) {
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    // Si alloca la memoria per il pacchetto di risposta del Centro Vaccinale.
    centroVaccinaleReplyToClientCitizen * newCentroVaccinaleReply = (centroVaccinaleReplyToClientCitizen *) calloc(1, sizeof(* newCentroVaccinaleReply));
    if (!newCentroVaccinaleReply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    // fullWrite per la scrittura e invio del codice della tessera sanitaria del cittadino al Centro Vaccinale.
    if ((fullWriteReturnValue = fullWrite(centroVaccinaleSocketFileDescriptor, healthCardNumber, nBytes)) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    
    /*
    fullRead per ottenere e leggere la risposta da parte del CentroVaccinale. Tale risposta è caratterizzata da
    una serie di parametri: Codice Tessera Sanitaria, Data Scadenza Vanilla Green Pass ed esito della richiesta.
    La risposta verrà salvata in "newCentrovaccinaleReply".
    */
    if ((fullReadReturnValue = fullRead(centroVaccinaleSocketFileDescriptor, (void *) newCentroVaccinaleReply, sizeof(* newCentroVaccinaleReply))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    /*
    Si effettua un controllo sul terzo campo: se esso conterrà il valore FALSE, allora ciò vorrà dire che non
    è stato possibile inoculare una nuova dose di vaccino, in quanto non è passata la soglia temporale
    minima per effettuare una nuova vaccinazione. Se invece il terzo campo conterrà il valore TRUE, allora
    significa che la vaccinazione è andata a buon fine. Fatto ciò, il ClientCitizen libera la memoria occupata,
    rilascia le risorse e chiude il socket file descriptor richiesto in precedenza.
    */
    if (newCentroVaccinaleReply->requestResult == FALSE) {
        if (fprintf(stdout, "\nNon e' possibile effetuare un'altra dose di vaccino. Devono passare almeno %d mesi dall'ultima inoculazione.\nData a partire dalla quale e' possibile effettuare un'altra dose di vaccino: %s\nArrivederci.\n", MONTHS_TO_WAIT_FOR_NEXT_VACCINATION, newCentroVaccinaleReply->greenPassExpirationDate) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    } else {
        if (fprintf(stdout, "\n... Vaccinazione in corso ...\n\nLa vaccinazione e' andata a buon fine.\nData a partire dalla quale e' possibile effettuare un'altra dose di vaccino: %s\nArrivederci.\n", newCentroVaccinaleReply->greenPassExpirationDate) < 0) raiseError(FPRINTF_SCOPE, FPRINTF_ERROR);
    }
    free(newCentroVaccinaleReply);
}
