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
    addr.sin_addr.s_addr    = inet_addr("127.0.0.1");

    err = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    if(err<0) {perror("erreur bind");exit(0);}
    
    flen = sizeof(struct sockaddr_in);
	//attente de la requête client
    len = recvfrom(fd,msg,sizeof(msg),0, (struct sockaddr *) &addr, &flen); 
    if(len<0){perror("erreur recvfrom");exit(0);}
    


    printf("Received a request for a song : %s \n",msg);
	//recherche dans la bdd des sons de la requête
    int found = findFile(msg);
	char codeFound[2];
	sprintf(codeFound,"%d",found); //pour mettre le int found dans un char[] afin de l'envoyer au client

	//renvoi du message pour savoir si oui ou non on a trouvé la musique
	err = sendto(fd,codeFound,strlen(codeFound)+1,0,(struct sockaddr *) &addr,sizeof(struct sockaddr_in));
	if(err<0){perror("Erreur sento ");exit(0);}
	if(found < 1){printf("Fichier demandé introuvable, arrêt ...\n"); return(0);}
	
	//attente de l'ACK du client sur la requete réussie
	len = recvfrom(fd,msg,sizeof(msg),0, (struct sockaddr *) &addr, &flen); 
    if(len<0){perror("erreur recvfrom");exit(0);}
    int code = strtol(msg, NULL, 10); 
	if(code<1){
		printf("Retour négatif du client, arrêt ... \n"); return(0);
	}
	
	printf("Client ok pour la transmission, envoi des infos du fichier \n");
	
	
	close(fd);
    return 1;
}
