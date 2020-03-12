#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include "audio.h"

typedef struct client {

    in_addr_t addrClient;
    struct client * next;
} client_t;

int insertClient(client_t *currentClient,client_t *new){
    while(currentClient->next != NULL){
        if(currentClient->addrClient == new->addrClient){
            return 0;
        }
        else
        {
        currentClient = currentClient->next;
        }
    }
    currentClient->next = new;
    return 1;

}

void servirClient(){

}


int main(){

    int fd, err;
    char msg[64];
    socklen_t len, flen;
    struct sockaddr_in addr;


    //initialisation serveur
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0) {perror("erreur socket");exit(0);}

    addr.sin_family         = AF_INET;
    addr.sin_port           = htons(1234); //port arbitrairement mis @1234 mais possibilité de changer /!\ si changement le changer aussi dans /client/audioclient.c
    addr.sin_addr.s_addr    = inet_addr("127.0.0.1");

    err = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    if(err<0) {perror("erreur bind");exit(0);}
    client_t *firstClient= (client_t *)malloc(sizeof(client_t));
    firstClient->addrClient = inet_addr("127.0.0.1");
    firstClient->next = NULL;

    while(1){
        struct sockaddr_in dest = {0};
        flen = sizeof(struct sockaddr_in);
        //attente de la requête d'un client
        len = recvfrom(fd,msg,sizeof(msg),0, (struct sockaddr *) &dest, &flen);
        if(len<0){perror("erreur recvfrom");exit(0);}
        client_t *newClient = (client_t *)malloc(sizeof(client_t));
        newClient->addrClient = dest.sin_addr.s_addr;
        newClient->next = NULL;

        if(insertClient(firstClient,newClient)){
            printf("Nouveau client detecté : fork pour le servir");
            pid_t pid = fork();
            if (pid == 0) {//on va servir le nouveau client
                servirClient();
            }
        }



    }


}
