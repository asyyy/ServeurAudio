#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "audio.h"




int main (int argc, char *argv[]){
	 // Broken pipe => sleep un certain moment = random
	printf("coucou \n");
	if (argc == 2){ //si on a bien rentré un fichier en entrée
		char *filename = argv[1];
		
		int sample_rate;
		int sample_size;
		int channels;
		int opening = aud_readinit(filename,&sample_rate,&sample_size,&channels);
		if(opening < 0) {printf("erreur ouverture fichier");exit(0);}
		
		printf("ouverture ok \n");
		filename[strlen(filename) - 1] = '\0';
		int writing = aud_writeinit(sample_rate,sample_size,channels);
		
		if(writing < 0) {printf("erreur ecriture fichier");exit(0);}
		
		
		int buffer[sample_size];
		ssize_t read1 = read(opening,buffer,sample_size);
		ssize_t write1 = write(writing,buffer,sample_size);

		while((read1 == sample_size) && (write1 == sample_size)){
			read1 = read(opening,buffer,sample_size);
			write1 = write(writing,buffer,sample_size);
		}
		close(opening);
		close(writing);
		return 1;
	}	
}
