
CFLAGS = -Wall -g

lecteur: obj/lecteur.o obj/audio.o 
	gcc $(CFLAGS) -o lecteur obj/lecteur.o obj/audio.o
client: obj/client.o
	gcc $(CFLAGS) -o ./client/audioclient obj/client.o
serveur: obj/serveur.o
	gcc $(CFLAGS) -o ./serveur/audioserveur obj/serveur.o


obj/lecteur.o: src/lecteur.c 
	gcc $(CFLAGS) -c src/lecteur.c -I include -o obj/lecteur.o
obj/audio.o: src/audio.c  
	gcc $(CFLAGS) -c src/audio.c -I include -o obj/audio.o
obj/client.o: client/audioclient.c
	gcc $(CFLAGS) -c client/audioclient.c -o obj/client.o
obj/serveur.o: serveur/audioserveur.c
	gcc $(CFLAGS) -c serveur/audioserveur.c -o obj/serveur.o

clean:
	rm ./obj/audio.o obj/hash.o obj/lecteur.o lecteur

