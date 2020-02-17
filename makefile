#
CFLAGS = -Wall -g
#
lecteur: obj/audio.o obj/lecteur.o
# To be completed
	gcc $(CFLAGS) -o lecteur obj/lecteur.o obj/audio.o
#
# Objets of TP projet
#
obj/lecteur.o: src/lecteur.c 
	gcc $(CFLAGS) -c src/lecteur.c -I ./include -o obj/lecteur.o
# To be completed
obj/audio.o: src/audio.c  
# To be completed
	gcc $(CFLAGS) -c src/audio.c -I ./include -o obj/audio.o

