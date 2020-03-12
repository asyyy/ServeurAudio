#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "audio.h"

int main (int argc, char *argv[]){

    if (argc != 3){
		printf("Erreur, nombre d'arguments incorrect, requis : 1) ip 2) filename \n");
		return(0);
    }
    int fd, err;
    char *msg = argv[2];pas de fichier demandé 
	char codeFound[1];
	char bufferInfos[16];
	socklen_t flen, len;
    printf("Requested song : %s \n",msg);


	//initialisation client
    struct sockaddr_in dest;
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0) {perror("erreur socket");exit(0);}

    dest.sin_family         = AF_INET;
    dest.sin_port           = htons(1234); //port arbitrairement mis @1234 mais possibilité de changer /!\ si changement le changer aussi dans /serveur/audioserveur.c
    dest.sin_addr.s_addr    = inet_addr(argv[1]);

	//envoi 1er message de requête au serveur
    err = sendto(fd,msg,strlen(msg)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    if(err<0){perror("Erreur sento ");exit(0);}
	flen = sizeof(struct sockaddr_in);


	//reception du code de retour du serveur (1 si son disponible 0 sinon)
    len = recvfrom(fd,codeFound,sizeof(codeFound),0, (struct sockaddr *) &dest, &flen);
	if(len<0){perror("Erreur recv ");exit(0);}

	int code = strtol(codeFound, NULL, 10); //conversion en int du message de retour
    printf(" code recu %d \n",code);
	if (code == 0){
		printf("Musique demandée non disponible \n");
		return(0);
	}
    else if (code == 3){
         printf("Serveur occupé \n");
		return(0);
    }
	printf("Musique disponible, envoi de la requête de lecture au serveur ...\n");

	//envoi 1er message de requête au serveur
    err = sendto(fd,"1",strlen(msg)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    if(err<0){perror("Erreur sento ");exit(0);}
    printf("Message envoyé, attente des infos du fichier ... \n");

	int sample_rate;
	int sample_size;
	int channels;
	//reception du sample_rate
	len = recvfrom(fd,bufferInfos,sizeof(bufferInfos),0, (struct sockaddr *) &dest, &flen);
    if(len<0){perror("erreur recvfrom");exit(0);}// TODO attendre le réenvoi de l'information par le serveur
    sample_rate = strtol(bufferInfos, NULL, 10); //passage de "char" à int

	//envoi du ok
	err = sendto(fd,"1",strlen(msg)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    if(err<0){perror("Erreur sento ");exit(0);} //TODO réenvoyer l'information au serveur

	printf("Reçu sample rate : %d \n",sample_rate);

	//reception du sample_size
	len = recvfrom(fd,bufferInfos,sizeof(bufferInfos),0, (struct sockaddr *) &dest, &flen);
    if(len<0){perror("erreur recvfrom");exit(0);}// TODO attendre le réenvoi de l'information par le serveur
    sample_size = strtol(bufferInfos, NULL, 10); //passage de "char" à int

	//envoi du ok
	err = sendto(fd,"1",strlen(msg)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    if(err<0){perror("Erreur sento ");exit(0);} //TODO réenvoyer l'information au serveur

	printf("Reçu sample size : %d \n",sample_size);

	//reception du nb channels
	len = recvfrom(fd,bufferInfos,sizeof(bufferInfos),0, (struct sockaddr *) &dest, &flen);
    if(len<0){perror("erreur recvfrom");exit(0);} // TODO attendre le réenvoi de l'information par le serveur
    channels = strtol(bufferInfos, NULL, 10); //passage de "char" à int

	//envoi du ok
	err = sendto(fd,"1",strlen(msg)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    if(err<0){perror("Erreur sento ");exit(0);}//TODO réenvoyer l'information au serveur

	printf("Reçu channels : %d \n Fin de la reception des informations, début de la reception du fichier \n",channels);

	int writing = aud_writeinit(sample_rate,sample_size,channels);
	int bufferInt[sample_size];
	ssize_t write1 = write(writing,bufferInt,sample_size);
	int iteration = 0;
	while(write1 == sample_size){

		len = recvfrom(fd,bufferInt,(sizeof(bufferInt)/sizeof(int))+1,0, (struct sockaddr *) &dest, &flen);
    	if(len<0){perror("erreur recvfrom");exit(0);} // TODO attendre le réenvoi de l'information par le serveur


        write1 = write(writing,bufferInt,sample_size);
		//envoi du ok
		err = sendto(fd,"1",strlen(msg)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    	if(err<0){perror("Erreur sento ");exit(0);}//TODO réenvoyer l'information au serveur

		iteration = iteration + 1;

	}


	close(writing);

	close(fd);
    return 1;
}
