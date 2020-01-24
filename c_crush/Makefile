LIBS=`pkg-config --cflags --libs allegro-5 allegro_acodec-5 allegro_audio-5 allegro_color-5 allegro_font-5 allegro_image-5 allegro_main-5 allegro_memfile-5 allegro_physfs-5 allegro_primitives-5 allegro_ttf-5` -lm

INCLUDES=-I/home/pi/allegro5/

all: ccrush

ccrush: ccrush.o
	gcc -o ccrush ccrush.o $(LIBS)

ccrush.o: ccrush.c
	gcc -c ccrush.c $(LIBS)

clean:
	rm -f ccrush.o
	rm -f ccrush
