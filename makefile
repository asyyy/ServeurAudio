
CFLAGS = -Wall -g

lecteur: obj/audio.o obj/lecteur.o

	gcc $(CFLAGS) -o lecteur obj/lecteur.o obj/audio.o

obj/lecteur.o: src/lecteur.c 
	gcc $(CFLAGS) -c src/lecteur.c -I ./include -o obj/lecteur.o
obj/audio.o: src/audio.c  
	gcc $(CFLAGS) -c src/audio.c -I ./include -o obj/audio.o
clean:
	rm ./obj/audio.o ./obj/hash.o ./obj/lecteur.o lecteur
clean client: 
	rm ./client/audioclient
clean serveur: 
	rm ./serveur/audioserveur
client:
	gcc $(CFLAGS) -c client/audioclient.c -o client/audioclient
serveur:
	gcc $(CFLAGS) -c serveur/audioserveur.c -o serveur/audioserveur


