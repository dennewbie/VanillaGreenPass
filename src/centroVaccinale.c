//
//  centroVaccinale.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 08/01/22.
//

#include "centroVaccinale.h"

int main (int argc, char * argv[]) {
    int serverV_SocketFileDescriptor, listenFileDescriptor, connectionFileDescriptor, enable = TRUE;
    struct sockaddr_in client, centroVaccinaleAddress;
    unsigned short int centroVaccinalePort;
    pid_t childPid;
    
    // Si verifica che il centroVaccinale sia stato avviato con i parametri che si aspetta di avere.
    checkUsage(argc, (const char **) argv, CENTRO_VACCINALE_ARGS_NO, expectedUsageMessage);
    /*
    Si cerca di ricavare il numero di porta a partire dal valore passato come argomento via terminale all'avvio di
    CentroVaccinale.
    */
    centroVaccinalePort = (unsigned short int) strtoul((const char * restrict) argv[1], (char ** restrict) NULL, 10);
    if (centroVaccinalePort == 0 && (errno == EINVAL || errno == ERANGE)) raiseError(STRTOUL_SCOPE, STRTOUL_ERROR);
    
    // Si imposta la comunicazione col clientCitizen
    listenFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    // Si imposta l'opzione di riutilizzo degli indirizzi durante l'applicazione del meccanismo di IPC via socket.
    if (setsockopt(listenFileDescriptor, SOL_SOCKET, SO_REUSEADDR, & enable, (socklen_t) sizeof(enable))  == -1) raiseError(SET_SOCK_OPT_SCOPE, SET_SOCK_OPT_ERROR);
    memset((void *) & centroVaccinaleAddress, 0, sizeof(centroVaccinaleAddress));
    memset((void *) & client, 0, sizeof(client));
    
    centroVaccinaleAddress.sin_family      = AF_INET;
    centroVaccinaleAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    centroVaccinaleAddress.sin_port        = htons(centroVaccinalePort);
    wbind(listenFileDescriptor, (struct sockaddr *) & centroVaccinaleAddress, (socklen_t) sizeof(centroVaccinaleAddress));
    wlisten(listenFileDescriptor, LISTEN_QUEUE_SIZE);
    signal(SIGCHLD, SIG_IGN);
    
    while (TRUE) {
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        while ((connectionFileDescriptor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength)) < 0 && (errno == EINTR));
        if ((childPid = fork()) == -1) {
            raiseError(FORK_SCOPE, FORK_ERROR);
        } else if (childPid == 0) {
            // Processo figlio che chiude il FD realtivo "all'ascolto" delle nuove connessioni in arrivo per il CentroVaccinale
            wclose(listenFileDescriptor);
            // Realizzazione di un collegamento con il ServerV all'interno del processo figlio generato
            serverV_SocketFileDescriptor = createConnectionWithServerV(configFilePathCentroVaccinale);
            // Invocazione della routine per la gestione della richiesta del ClientCitizen collegatosi.
            clientCitizenRequestHandler(connectionFileDescriptor, serverV_SocketFileDescriptor);
            exit(0);
        }
        // Processo padre che chiude il socket file descriptor che realizza la connessione con il ClientCitizen collegatosi.
        wclose(connectionFileDescriptor);
    }
    
    // Codice mai eseguito
    wclose(listenFileDescriptor);
    exit(0);
}

void clientCitizenRequestHandler (int connectionFileDescriptor, int serverV_SocketFileDescriptor) {
    char * vaccineExpirationDate;
    // Allocazione dinamica della memoria per il pacchetto da inviare al ClientCitizen e quello da inviare al ServerV.
    centroVaccinaleReplyToClientCitizen * newCentroVaccinaleReply = (centroVaccinaleReplyToClientCitizen *) calloc(1, sizeof(centroVaccinaleReplyToClientCitizen));
    centroVaccinaleRequestToServerV * newCentroVaccinaleRequest = (centroVaccinaleRequestToServerV *) calloc(1, sizeof(centroVaccinaleRequestToServerV));
    serverV_ReplyToCentroVaccinale * newServerV_Reply = (serverV_ReplyToCentroVaccinale *) calloc(1, sizeof(serverV_ReplyToCentroVaccinale));
    if (!newCentroVaccinaleReply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newCentroVaccinaleRequest) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    char buffer[HEALTH_CARD_NUMBER_LENGTH];
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    unsigned short int centroVaccinaleSenderID = centroVaccinaleSender;
    
    // fullRead per la lettura del codice della Tessera Sanitaria
    if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) buffer, (size_t) HEALTH_CARD_NUMBER_LENGTH * sizeof(char))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    // Copia del codice della tessera sanitaria nel primo campoo del pacchetto di invio al ServerV.
    strncpy((char *) newCentroVaccinaleRequest->healthCardNumber, (const char *)  buffer, HEALTH_CARD_NUMBER_LENGTH);
    // Calcolo del periodo di validità del Vanilla Green Pass
    vaccineExpirationDate = getVaccineExpirationDate();
    // Copia del valore di tale periodo nel pacchetto di invio al ServerV.
    strncpy((char *) newCentroVaccinaleRequest->greenPassExpirationDate, (const char *) vaccineExpirationDate, DATE_LENGTH);
    
    // fullWrite per scrivere e inviare il pacchetto al ServerV
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) & centroVaccinaleSenderID, sizeof(centroVaccinaleSenderID))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) newCentroVaccinaleRequest, sizeof(* newCentroVaccinaleRequest))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    // fullRead per la rettura e la ricezione del pacchetto di risposta dal ServerV
    if ((fullReadReturnValue = fullRead(serverV_SocketFileDescriptor, (void *) newServerV_Reply, sizeof(* newServerV_Reply))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    // Copia dei valori di risposta del ServerV nel pacchetto di risposta da parte del centroVaccinale al ClientCitizen
    strncpy((char *) newCentroVaccinaleReply->healthCardNumber, (const char *) newServerV_Reply->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    strncpy((char *) newCentroVaccinaleReply->greenPassExpirationDate, (const char *) newServerV_Reply->greenPassExpirationDate, DATE_LENGTH);
    newCentroVaccinaleReply->requestResult = newServerV_Reply->requestResult == TRUE ? TRUE : FALSE;
    
    // Invio del pacchetto al clientCitizen tramite FullWrite sul descrittore connesso
    if ((fullWriteReturnValue = fullWrite(connectionFileDescriptor, (const void *) newCentroVaccinaleReply, (size_t) sizeof(* newCentroVaccinaleReply))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    
    free(newCentroVaccinaleReply);
    free(newCentroVaccinaleRequest);
    free(newServerV_Reply);
    free(vaccineExpirationDate);
    wclose(connectionFileDescriptor);
    wclose(serverV_SocketFileDescriptor);
}
