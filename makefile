
CFLAGS = -Wall -g

lecteur: obj/lecteur.o obj/audio.o
	gcc $(CFLAGS) -o lecteur obj/lecteur.o obj/audio.o
client: obj/client.o obj/audio.o
	gcc $(CFLAGS) -o ./client/audioclient obj/client.o obj/audio.o
serveur: obj/serveur.o obj/audio.o
	gcc $(CFLAGS) -o ./serveur/audioserveur obj/serveur.o obj/audio.o
serveur2: obj/serveur2.o obj/audio.o
	gcc $(CFLAGS) -o ./serveur/audioserveur2 obj/serveur2.o obj/audio.o


obj/lecteur.o: src/lecteur.c
	gcc $(CFLAGS) -c src/lecteur.c -I include -o obj/lecteur.o
obj/audio.o: src/audio.c
	gcc $(CFLAGS) -c src/audio.c -I include -o obj/audio.o
obj/client.o: client/audioclient.c
	gcc $(CFLAGS) -c client/audioclient.c -I include -o obj/client.o
obj/serveur.o: serveur/audioserveur.c
	gcc $(CFLAGS) -c serveur/audioserveur.c -I include -o obj/serveur.o
obj/serveur2.o: serveur/audioserveur2.c
	gcc $(CFLAGS) -c serveur/audioserveur2.c -I include -o obj/serveur2.o

clean:
	rm ./obj/audio.o obj/hash.o obj/lecteur.o lecteur
