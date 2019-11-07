#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <math.h>


#define SCREEN_W 640
#define SCREEN_H 480
#define INFO_H 64
#define FPS 60
#define MARGIN 5


// desenho do cenário
void draw_janela(ALLEGRO_DISPLAY *janela) {
 	
// definição de cor do fundo
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(255,255,255); 
	al_set_target_bitmap(al_get_backbuffer(janela));
	al_clear_to_color(BKG_COLOR);   
	
}


int main(int argc, char **argv){
//representa a janela principal
	ALLEGRO_DISPLAY *janela = NULL;

//Inicializa a biblioteca
	if(!al_init()){
		fprintf(stderr, "falha ao inicializar allegro!\n");
	}

	janela = al_create_display(SCREEN_W, SCREEN_H);
	if(!janela){
		fprintf(stderr, "falha ao criar janela\n");
		//al_destroy_timer(timer);
		return -1;
	}

	draw_janela(janela);

	al_flip_display();
	al_rest(10.0);
	al_destroy_display(janela);

	
	return 0;

}