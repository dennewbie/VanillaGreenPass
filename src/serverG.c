//
//  serverG.c
//  VanillaGreenPass
//
//  Created by Denny Caruso on 10/01/22.
//

#include "serverG.h"

int main (int argc, char * argv[]) {
    int serverV_SocketFileDescriptor, listenFileDescriptor, connectionFileDescriptor, enable = TRUE;
    struct sockaddr_in client, serverG_Address;
    unsigned short int serverG_Port, requestIdentifier;
    pid_t childPid;
    
    // Si verifica che il ServerG sia stato avviato con i parametri che si aspetta di avere.
    checkUsage(argc, (const char **) argv, SERVER_G_ARGS_NO, expectedUsageMessage);
    // Si cerca di ricavare il numero di porta a partire dal valore passato come argomento via terminale all'avvio del ServerG.
    serverG_Port = (unsigned short int) strtoul((const char * restrict) argv[1], (char ** restrict) NULL, 10);
    if (serverG_Port == 0 && (errno == EINVAL || errno == ERANGE)) raiseError(STRTOUL_SCOPE, STRTOUL_ERROR);
    
    // Si imposta la comunicazione col clientS e clientT
    listenFileDescriptor = wsocket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(listenFileDescriptor, SOL_SOCKET, SO_REUSEADDR, & enable, (socklen_t) sizeof(enable)) == -1) raiseError(SET_SOCK_OPT_SCOPE, SET_SOCK_OPT_ERROR);
    memset((void *) & serverG_Address, 0, sizeof(serverG_Address));
    memset((void *) & client, 0, sizeof(client));
    
    serverG_Address.sin_family      = AF_INET;
    serverG_Address.sin_addr.s_addr = htonl(INADDR_ANY);
    serverG_Address.sin_port        = htons(serverG_Port);
    wbind(listenFileDescriptor, (struct sockaddr *) & serverG_Address, (socklen_t) sizeof(serverG_Address));
    wlisten(listenFileDescriptor, LISTEN_QUEUE_SIZE * LISTEN_QUEUE_SIZE);
    signal(SIGCHLD, SIG_IGN);
    
    while (TRUE) {
        ssize_t fullReadReturnValue;
        socklen_t clientAddressLength = (socklen_t) sizeof(client);
        while ((connectionFileDescriptor = waccept(listenFileDescriptor, (struct sockaddr *) & client, (socklen_t *) & clientAddressLength)) < 0 && (errno == EINTR));
        // Attesa tramite fullRead dell'identificativo del mittente col quale si è messo in collegamento.
        if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) & requestIdentifier, sizeof(requestIdentifier))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
        
        if ((childPid = fork()) == -1) {
            raiseError(FORK_SCOPE, FORK_ERROR);
        } else if (childPid == 0) {
            // Processo figlio che chiude il FD realtivo "all'ascolto" delle nuove connessioni in arrivo per il ServerG
            wclose(listenFileDescriptor);
            // Richiesta di instaurazione di una connessione con il ServerV.
            serverV_SocketFileDescriptor = createConnectionWithServerV(configFilePathServerG);
            
            // Controllo dell'ID del mittente (ClientS o ClientT).
            switch (requestIdentifier) {
                // ClientS
                case clientS_viaServerG_Sender:
                    clientS_RequestHandler(connectionFileDescriptor, serverV_SocketFileDescriptor);
                    break;
                // ClientT
                case clientT_viaServerG_Sender:
                    clientT_RequestHandler(connectionFileDescriptor, serverV_SocketFileDescriptor);
                    break;
                // ID sconosciuto
                default:
                    raiseError(INVALID_SENDER_ID_SCOPE, INVALID_SENDER_ID_ERROR);
                    break;
            }
            
            wclose(connectionFileDescriptor);
            wclose(serverV_SocketFileDescriptor);
            exit(0);
        }
        // Padre che chiude il socket file descriptor che realizza la connessione con il ClientCitizen collegatosi.
        wclose(connectionFileDescriptor);
    }
    
    // Codice mai eseguito
    wclose(listenFileDescriptor);
    exit(0);
}

/*
Procedura per la gestione del servizio con un ClientS. I paraetri di ingresso sono: il socket file
descriptor col quale è possibile comunicare con un ClientS o un ClientT e il socket file descriptor col
quale è possibile comunicare con il ServerV.
*/
void clientS_RequestHandler (int connectionFileDescriptor, int serverV_SocketFileDescriptor) {
    char healthCardNumber[HEALTH_CARD_NUMBER_LENGTH];
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    unsigned short int clientS_viaServerG_SenderID = clientS_viaServerG_Sender;
    
    // Allocazione dinamica della memoria per il pacchetto da inviare al ClientS e quello da ricevere dal ServerV.
    serverG_ReplyToClientS * newServerG_Reply = (serverG_ReplyToClientS *) calloc(1, sizeof(* newServerG_Reply));
    serverV_ReplyToServerG_clientS * newServerV_Reply = (serverV_ReplyToServerG_clientS *) calloc(1, sizeof(* newServerV_Reply));
    if (!newServerG_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    // fullRead per leggere il codice della tessera sanitaria.
    if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) healthCardNumber, sizeof(char) * HEALTH_CARD_NUMBER_LENGTH)) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    // fullWrite per scrivere e inviare l'ID del Client e il suo codice di tessera sanitaria nel pacchetto da inviare al ServerV.
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) & clientS_viaServerG_SenderID, sizeof(clientS_viaServerG_SenderID))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) healthCardNumber, sizeof(char) * HEALTH_CARD_NUMBER_LENGTH)) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    // fullRead per attendere in lettura il pacchetto di risposta da parte del ServerV.
    if ((fullReadReturnValue = fullRead(serverV_SocketFileDescriptor, (void *) newServerV_Reply, sizeof(* newServerV_Reply))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    
    // Copia dei parametri del pacchetto di risposta del ServerV nel pacchetto da inviare al ClientS
    strncpy((char *) newServerG_Reply->healthCardNumber, (const char *) newServerV_Reply->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerG_Reply->requestResult = newServerV_Reply->requestResult;
    // fullWrite per inviare il pacchetto al ClientS.
    if ((fullWriteReturnValue = fullWrite(connectionFileDescriptor, (const void *) newServerG_Reply, sizeof(* newServerG_Reply))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    free(newServerV_Reply);
    free(newServerG_Reply);
}

void clientT_RequestHandler (int connectionFileDescriptor, int serverV_SocketFileDescriptor) {
    unsigned short int clientT_viaServerG_SenderID = clientT_viaServerG_Sender;
    ssize_t fullWriteReturnValue, fullReadReturnValue;
    
    /*
     Allocazione dinamica della memoria per il pacchetto da ricevere dal ClientT, quello da inviare al ClientT,
     quello da inviare al ServerV e quello da ricevere dal ServerV.
    */
    clientT_RequestToServerG * newClientT_Request = (clientT_RequestToServerG *) calloc(1, sizeof(* newClientT_Request));
    serverG_ReplyToClientT * newServerG_Reply = (serverG_ReplyToClientT *) calloc(1, sizeof(* newServerG_Reply));
    serverG_RequestToServerV_onBehalfOfClientT * newServerG_Request = (serverG_RequestToServerV_onBehalfOfClientT *) calloc(1, sizeof(* newServerG_Request));
    serverV_ReplyToServerG_clientT * newServerV_Reply = (serverV_ReplyToServerG_clientT *) calloc(1, sizeof(* newServerV_Reply));
    if (!newServerG_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerV_Reply) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newClientT_Request) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    if (!newServerG_Request) raiseError(CALLOC_SCOPE, CALLOC_ERROR);
    
    // fullRead per leggere la richiesta del ClientT
    if ((fullReadReturnValue = fullRead(connectionFileDescriptor, (void *) newClientT_Request, (size_t) sizeof(* newClientT_Request))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    strncpy((char *) newServerG_Request->healthCardNumber, (const char *) newClientT_Request->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerG_Request->updateValue = newClientT_Request->updateValue;
    // fullWrite per scrivere e inviare al ServerV ID e richiesta al ServerV
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) & clientT_viaServerG_SenderID, sizeof(clientT_viaServerG_SenderID))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    if ((fullWriteReturnValue = fullWrite(serverV_SocketFileDescriptor, (const void *) newServerG_Request, sizeof(* newServerG_Request))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    // fullRead per attendere e leggere la risposta dal ServerV
    if ((fullReadReturnValue = fullRead(serverV_SocketFileDescriptor, (void *) newServerV_Reply, sizeof(* newServerV_Reply))) != 0) raiseError(FULL_READ_SCOPE, (int) fullReadReturnValue);
    
    strncpy((char *) newServerG_Reply->healthCardNumber, (const char *) newServerV_Reply->healthCardNumber, HEALTH_CARD_NUMBER_LENGTH);
    newServerG_Reply->updateResult = newServerV_Reply->updateResult;
    // fullWrite per scrivere e invoare la risposta precedente del ServerV al ClientT
    if ((fullWriteReturnValue = fullWrite(connectionFileDescriptor, (const void *) newServerG_Reply, sizeof(* newServerG_Reply))) != 0) raiseError(FULL_WRITE_SCOPE, (int) fullWriteReturnValue);
    free(newClientT_Request);
    free(newServerG_Reply);
    free(newServerG_Request);
    free(newServerV_Reply);
}
