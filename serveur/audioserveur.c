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

// 1 == vrai et 0 == false
/**
*Fonction qui prend 2 strings en entrée et qui dit si str commence par pre
*/
int startsWith(const char *pre, const char *str)
{
	return strncmp(pre, str, strlen(pre)) == 0;	
}
/*
* Fonction pour chercher le fichier dans le repertoire avec tous les fichiers, rend 1 si trouvé et 0 si non trouvé
*/
int findFile(char fileName[]) {
    DIR *d;
	struct dirent *dir;
    d = opendir("serveur/audio");
    if (!(d == NULL))  {
        while ((dir = readdir(d)) != NULL) //permet de looper sur le dossier avec les musiques et de chercher celle demandée
        {
			if((startsWith(fileName,dir->d_name)) == 1){ //permet de comparer les strings en virant le .wav de la fin
             //besoin de revoir le protocole de recherche car il rend true au premier fichier qui commence pareil trouvé
				return (1);
			}

        }
        closedir(d);
    }
	else {printf("Impossible d'ouvrir le dossier \n"); return (0);}
	printf("Fichier non trouvé, envoi du resultat au client \n");
    return(0);
	
}


int main (){
    int fd, err;
    char msg[64];
    socklen_t len, flen;
    struct sockaddr_in addr;

	//initialisation serveur
    fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0) {perror("erreur socket");exit(0);}	

    addr.sin_family         = AF_INET;
    addr.sin_port           = htons(1234); //port arbitrairement mis @1234 mais possibilité de changer /!\ si changement le changer aussi dans /client/audioclient.c
    addr.sin_addr.s_addr    = inet_addr("148.60.2.164");

    err = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    if(err<0) {perror("erreur bind");exit(0);}
    struct sockaddr_in dest = {0};
    flen = sizeof(struct sockaddr_in);
	//attente de la requête client
    len = recvfrom(fd,msg,sizeof(msg),0, (struct sockaddr *) &dest, &flen); 
    if(len<0){perror("erreur recvfrom");exit(0);}
    
    in_addr_t addrClient = dest.sin_addr.s_addr;


    printf("Received a request for a song : %s \n",msg);
	//recherche dans la bdd des sons de la requête
    int found = findFile(msg);
	char codeFound[2];
	sprintf(codeFound,"%d",found); //pour mettre le int found dans un char[] afin de l'envoyer au client

	//renvoi du message pour savoir si oui ou non on a trouvé la musique
	err = sendto(fd,codeFound,strlen(codeFound)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in))	;
	if(err<0){perror("Erreur sento ");exit(0);}
	if(found < 1){printf("Fichier demandé introuvable, arrêt ...\n"); return(0);}
	
	//attente de l'ACK du client sur la requete réussie
	char ackClient[2];
	len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen); 
    if(len<0){perror("erreur recvfrom");exit(0);}
    int codeRetour = strtol(ackClient, NULL, 10); //passage de "1" à 1(int)
	while(dest.sin_addr.s_addr != addrClient){
            err = sendto(fd,"3",sizeof(int)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
            dest.sin_addr.s_addr = addrClient;
            len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen);
            
        }
	if(codeRetour<1){
		printf("Retour négatif du client, arrêt ... \n"); return(0);
	}
	
	printf("Client ok pour la transmission, envoi des infos du fichier \n");
	char filePath[128] = {}; //chemin entier du fichier a envoyer
	printf("%s\n",filePath);
	char *dir = "serveur/audio/";
	char *ext = ".wav";
	strcat(filePath,dir); // construction 
	strcat(filePath,msg); // du 
	strcat(filePath,ext); // chemin
	int sample_rate;
	int sample_size;
	int channels;
	
	printf("Le chemin : %s \n", filePath);
	int opening = aud_readinit(filePath,&sample_rate,&sample_size,&channels);
	if(opening < 0) {printf("erreur ouverture fichier \n");exit(0);}
	
    //envoi des infos fichier au client (1 par 1);
	char infos[16];
	sprintf(infos, "%d", sample_rate); //début par le sample_rate
	err = sendto(fd,infos,strlen(infos)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
	if(err<0){perror("Erreur sento ");exit(0);}

	len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen); 
    if(len<0){perror("erreur recvfrom");exit(0);}
    codeRetour = strtol(ackClient, NULL, 10); //passage de "1" à 1(int)
	while(dest.sin_addr.s_addr != addrClient){
            err = sendto(fd,"3",sizeof(int)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
            dest.sin_addr.s_addr = addrClient;
            len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen);
            
        }
	if(codeRetour<1){
		printf("Retour négatif du client, arrêt ... \n"); return(0); // TODO changer ça pour qu'il réenvoi le parametre
	}

	//envoi du sample_size
	sprintf(infos, "%d", sample_size); 
	err = sendto(fd,infos,strlen(infos)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in))	;
	if(err<0){perror("Erreur sento ");exit(0);}

	len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen); 
    if(len<0){perror("erreur recvfrom");exit(0);}
    codeRetour = strtol(ackClient, NULL, 10); //passage de "1" à 1(int)
	while(dest.sin_addr.s_addr != addrClient){
            err = sendto(fd,"3",sizeof(int)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
            dest.sin_addr.s_addr = addrClient;
            len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen);
            
        }
	if(codeRetour<1){
		printf("Retour négatif du client, arrêt 1... \n"); return(0); // TODO changer ça pour qu'il réenvoi le parametre
	}

	//envoi du nb de channels
	sprintf(infos, "%d", channels); 
	err = sendto(fd,infos,strlen(infos)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in))	;
	if(err<0){perror("Erreur sento ");exit(0);}

	len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen); 
    if(len<0){perror("erreur recvfrom");exit(0);}
    codeRetour = strtol(ackClient, NULL, 10); //passage de "1" à 1(int)
	while(dest.sin_addr.s_addr != addrClient){
            err = sendto(fd,"3",sizeof(int)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
            dest.sin_addr.s_addr = addrClient;
            len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen);
            
        }
	if(codeRetour<1){
		printf("Retour négatif du client, arrêt 2... \n"); return(0); // TODO changer ça pour qu'il réenvoi le parametre
	}
	
	//si on arrive ici il faut transmettre les paquets de sorte que le lecteur puisse les lire
	//load une portion du fichier, l'envoyer, le réenvoyer si retour négatif du client
	//TODO calculer le temps de wait du client en fonction des paramètres fichiers
	
	
	int bufferInt[sample_size];

	//char bufferChar[sample_size];
	ssize_t reading = read(opening,bufferInt,sample_size);

	
	while(reading == sample_size){

		err = sendto(fd,bufferInt,(sizeof(bufferInt)/sizeof(int))+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
    	if(err<0){perror("Erreur sento ");exit(0);}//TODO réenvoyer l'information au client

		len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen); 
    	if(len<0){perror("erreur recvfrom");exit(0);}
        while(dest.sin_addr.s_addr != addrClient){
                err = sendto(fd,"3",sizeof(int)+1,0,(struct sockaddr *) &dest,sizeof(struct sockaddr_in));
                dest.sin_addr.s_addr = addrClient;
                len = recvfrom(fd,ackClient,sizeof(ackClient),0, (struct sockaddr *) &dest, &flen);
            
        }
    	codeRetour = strtol(ackClient, NULL, 10); //passage de "1" à 1(int)
	
		if(codeRetour<1){
			printf("Retour négatif du client, arrêt ... \n"); return(0); // TODO changer ça pour qu'il réenvoi le parametre
		}

		
		reading = read(opening,bufferInt,sample_size);
        
	}

	close(opening);
	close(fd);
    return 1;
}
