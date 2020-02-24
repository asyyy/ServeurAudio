#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

int main (int argc, char *argv[]){

    if (argc < 2){
		printf("Erreur, pas de fichier demandé \n");
		return(0);
    }
    int fd, err;
    char *msg = argv[1];
	char codeFound[1];
	socklen_t flen;
    printf("Requested song : %s \n",msg);


	//initialisation client
    struct sockaddr_in dest;
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0) {perror("erreur socket");exit(0);}

    dest.sin_family         = AF_INET;
    dest.sin_port           = htons(1234); //port arbitrairement mis @1234 mais possibilité de changer /!\ si changement le changer aussi dans /serveur/audioserveur.c
    dest.sin_addr.s_addr    = inet_addr("127.0.0.1");
	
	//envoi 1er message de requête au serveur
    err = sendto(fd,msg,strlen(msg)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    if(err<0){perror("Erreur sento ");exit(0);}
	flen = sizeof(struct sockaddr_in);


	//reception du code de retour du serveur (1 si son disponible 0 sinon)
    err = recvfrom(fd,codeFound,sizeof(codeFound),0, (struct sockaddr *) &dest, &flen); 
	if(err<0){perror("Erreur recv ");exit(0);}
	
	int code = strtol(codeFound, NULL, 10); //conversion en int du message de retour
	if (code < 1){
		printf("Musique demandée non disponible \n");
		return(0);	
	}
	printf("Musique disponible, envoi de la requête de lecture au serveur ...\n");
	
	//envoi 1er message de requête au serveur
    err = sendto(fd,"1",strlen(msg)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    if(err<0){perror("Erreur sento ");exit(0);}
    printf("Message envoyé, attente des infos du fichier ... \n");

	close(fd);
    return 1;   
}
