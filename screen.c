#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <math.h>


#define SCREEN_W 480
#define SCREEN_H 640
// #define INFO_H 64 
#define FPS 60
#define MARGIN 5

#define NUM_TYPES 4
#define N_LINHAS 9
#define N_COLUNAS 6

#define LATA 1
#define GARRAFA 2
#define PAPEL 3
#define PET 4

// Definição de peças do jogo
typedef struct Lixo {
	int tipo;
	int linha;
	int coluna;
	int active;
	ALLEGRO_COLOR cor;	
} Lixo;

// Definindo Matriz a partir da struct lixo
Lixo M[N_LINHAS][N_COLUNAS];
ALLEGRO_COLOR colors[NUM_TYPES+1];

// Padroninzando tamanho da matriz
const int CELL_W = (int)SCREEN_W/N_COLUNAS;
const int CELL_H = (int)SCREEN_H/N_LINHAS;



int generateRandomLixo(){
	return rand()%NUM_TYPES + 1;
}

// imprime a matriz na tela preenchida
void imprimeMatriz(){
	printf("\n");
	int i, j;
	for(i=0; i<N_LINHAS; i++){
		for(j=0; j<N_COLUNAS; j++){
			printf("%d (%d,%d) ",M[i][j].tipo, M[i][j].linha, M[i][j].active);
		}
		printf("\n");
	}
}

//completa a matriz com a struct Lixo
int completaMatriz(){
	int i, j;
	for(i=0; i<N_LINHAS; i++){
		for(j=0; j=N_COLUNAS; j++){
			if(M[i][j].tipo == 0){
				M[i][j].tipo = generateRandomLixo();
				M[i][j].coluna = 0;
				M[i][j].linha = 0;
				M[i][j].active = 1;
				M[i][j].cor = colors[M[i][j].tipo];
			}
		}
	}
}

// Inicializa o jogo
void initGame(){
	int i, j;
	for(i=0; i<N_LINHAS; i++){
		for(j=0; j<N_COLUNAS; j++){
			M[i][j].tipo = generateRandomLixo();
			M[i][j].coluna = 0;
			M[i][j].linha = 0;
			M[i][j].active = 1;
			M[i][j].cor = colors[M[i][j].tipo];
			printf("%d ", M[i][j].tipo);
		}
		printf("\n");
	}
}

// Coleta a coordenada ______________________

int getXCoord(int col){
	return col * CELL_W;
}
int getYCoord(int lin){
	return lin * CELL_H;
}

// __________________________________________

void draw_lixo(Lixo R, int lin, int col) {
	int cell_x = getXCoord(col);
	int cell_y = getYCoord(lin);

	if(R.tipo == LATA){
		al_draw_filled_triangle(cell_x, cell_y+CELL_H, cell_x+CELL_W, cell_y+CELL_H, cell_x+CELL_W/2, cell_y, R.cor);	
	}
	else if(R.tipo == GARRAFA) {
		al_draw_filled_rectangle(cell_x, cell_y, cell_x+CELL_W, cell_y-CELL_H, R.cor);

	} 
	else if(R.tipo == PAPEL) {
		al_draw_filled_rounded_rectangle(cell_x, cell_y, cell_x+CELL_W, cell_y+CELL_H, CELL_W/3, CELL_H/3, R.cor);

	} 	
	else if(R.tipo == PET) {
		al_draw_filled_ellipse(cell_x+CELL_W/2, cell_y+CELL_H/2, CELL_W/2, CELL_H/2, R.cor);
	}
}

// desenho do cenário
void draw_janela(ALLEGRO_DISPLAY *janela) { 	
// definição de cor do fundo
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(255,255,255); 
	al_set_target_bitmap(al_get_backbuffer(janela));
	al_clear_to_color(BKG_COLOR);

// definição das peças do jogo
	int i, j;
	for(i=0; i<N_LINHAS; i++){
		for(j=0; j<N_COLUNAS; j++){
			draw_lixo(M[i][j], i, j);
		}
	}   
}

void getCell(int x, int y, int *lin, int *col){
	*lin = y/CELL_H;
	*lin = x/CELL_W;
}

void swap(int lin1, int col1, int lin2, int col2){
	Lixo aux;
	aux = M[lin1][col1];
	M[lin1][col1] = M[lin2][col2];
	M[lin2][col2] = aux; 
}

int main(int argc, char **argv){
//representa a janela principal
	ALLEGRO_DISPLAY *janela = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

// _________________________________________________________

// Rotinas de alerta de erro e inicialização da biblioteca
	if(!al_init()){
		fprintf(stderr, "falha ao inicializar allegro!\n");
	}

	if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
		return -1;
	}


	janela = al_create_display(SCREEN_W, SCREEN_H);
	if(!janela){
		fprintf(stderr, "falha ao criar janela\n");
		//al_destroy_timer(timer);
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	if(!al_install_mouse())
		fprintf(stderr, "failed to initialize mouse!\n");
// Inicialização da fila de evnetos que serão criados a partir de imput do mouse e do teclado
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(janela);
		al_destroy_timer(timer);
		return -1;
	}

	al_install_keyboard();
   //registra na fila de eventos que eu quero identificar quando a tela foi alterada
	al_register_event_source(event_queue, al_get_display_event_source(janela));
   //registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra o teclado na fila de eventos:
	al_register_event_source(event_queue, al_get_keyboard_event_source());   
	//registra mouse na fila de eventos:
	al_register_event_source(event_queue, al_get_mouse_event_source());    
   //inicia o temporizador
	al_start_timer(timer);   

// _________________________________________________________

	colors[0] = al_map_rgb(255,255,255);
	colors[LATA] = al_map_rgb(255, 0, 250);
	colors[GARRAFA] = al_map_rgb(250, 250, 0);
	colors[PAPEL] = al_map_rgb(0,0,255);
	colors[PET] = al_map_rgb(0,255,0);

// _________________________________________________________

	//srand(4);
	initGame();


// Criação da fila de eventos

	int playing = 1;

	int lin_src, col_src, lin_dst, col_dst;


	ALLEGRO_EVENT ev;
	
	while(playing) {

	  //espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				playing = 0;
			}

		}
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			printf("\nclicou em (%d, %d)", ev.mouse.x, ev.mouse.y);
			getCell(ev.mouse.x, ev.mouse.y, &lin_src, &col_src);
		}
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			printf("\nsoltou em (%d, %d)", ev.mouse.x, ev.mouse.y);
			getCell(ev.mouse.x, ev.mouse.y, &lin_dst, &col_dst);
			swap(lin_src, col_src, lin_dst, col_dst);
		}		
	    //se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		else if(ev.type == ALLEGRO_EVENT_TIMER) {
		    draw_janela(janela);
			al_flip_display();		
		}
	    //se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}

	} 

// _________________________________________________________


	al_rest(1);

	al_destroy_timer(timer);
	//al_flip_display();
	al_destroy_event_queue(event_queue);
	al_destroy_display(janela);

	
	return 0;

}