LIBS=`pkg-config --cflags --libs allegro-5 allegro_acodec-5 allegro_audio-5 allegro_color-5 allegro_font-5 allegro_image-5 allegro_main-5 allegro_memfile-5 allegro_physfs-5 allegro_primitives-5 allegro_ttf-5` -lm

INCLUDES=-I/home/pi/allegro5/

all: candy candyc ccrush louco

candy: candy.o
	gcc -o candy candy.o $(LIBS)

candy.o: candy.c
	gcc -c candy.c $(LIBS)

candyc: candyc.o
	gcc -o candyc candyc.o $(LIBS)

candyc.o: candyc.c
	gcc -c candyc.c $(LIBS)

ccrush: ccrush.o
	gcc -o ccrush ccrush.o $(LIBS)

ccrush.o: ccrush.c
	gcc -c ccrush.c $(LIBS)

louco: louco.o
	gcc -o louco louco.o $(LIBS)

louco.o: louco.c
	gcc -c louco.c $(LIBS)

clean:
	rm -f candy.o
	rm -f candy
	rm -f candyc.o
	rm -f candyc
	rm -f ccrush.o
	rm -f ccrush
	rm -f louco.o
	rm -f louco


