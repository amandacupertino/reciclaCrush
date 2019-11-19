#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdlib.h>
#include <math.h>


#define N_LINHAS 9
#define N_COLS 6

#define NUM_TYPES 4
#define LATA 1
#define GARRAFA 2
#define PAPEL 3
#define PET 4

#define LARGURA_TELA 480
#define ALTURA_TELA 640
#define LARGURA_JOGO 320
#define ALTURA_JOGO 480
#define LARGURA_COL 64
#define LARGURA_LIN 60
#define FPS 1
#define INFO_H 64
#define MARGIN 6

typedef struct Lixo {
	int type;
	int offset_lin;
	int offset_col;
	int active;
	ALLEGRO_BITMAP desenho;
} Lixo;

Lixo M[N_LINHAS][N_COLS];
ALLEGRO_BITMAP desenhos[NUM_TYPES+1];

const int COL_W = (int)LARGURA_TELA/N_COLS;
const int LIN_H = (int)(ALTURA_TELA-INFO_H)/N_LINHAS;

int score=0, plays=10;
char my_score[100], my_plays[100];

ALLEGRO_FONT *size_f;   



int Random() {
	return rand()%NUM_TYPES + 1;
}


void pausa(ALLEGRO_TIMER *timer) {
	al_stop_timer(timer);
	al_rest(3);
	al_start_timer(timer);
}

void completaMatriz() {
	int i, j;
	for(i=0; i<N_LINHAS; i++) {
		for(j=0; j<N_COLS; j++) {
			if(M[i][j].type == 0) {
				M[i][j].type = Random();
				M[i][j].offset_col = 0;
				M[i][j].offset_lin = 0;
				M[i][j].active = 1;		
				M[i][j].desenho = desenhos[M[i][j].type];	
			}
		}
	}
}

void iniciarJogo() {
	int i, j;
	for(i=0; i<N_LINHAS; i++) {
		for(j=0; j<N_COLS; j++) {
			M[i][j].type = Random();
			M[i][j].offset_col = 0;
			M[i][j].offset_lin = 0;
			M[i][j].active = 1;
			M[i][j].desenho = desenhos[M[i][j].type];
			printf("%d ", M[i][j].type);
		}
		printf("\n");
	}
}


int getXCoord(int col){
	return col * LARGURA_COL + 80;
}
int getYCoord(int lin){
	return lin * LARGURA_LIN + 80;
}


void draw_candy(int lin, int col) {

	int cell_x = getXCoord(col);
	int cell_y = getYCoord(lin);


	if(M[lin][col].type == LATA) {
		al_draw_filled_triangle(cell_x+MARGIN, cell_y + LIN_H - MARGIN,
		                        cell_x + COL_W - MARGIN, cell_y + LIN_H - MARGIN,
		                        cell_x + COL_W/2, cell_y+MARGIN,
		                        M[lin][col].desenho);
	} 
	else if(M[lin][col].type == GARRAFA) {
		al_draw_filled_rectangle(cell_x+2*MARGIN, cell_y+2*MARGIN,
		                         cell_x-2*MARGIN+COL_W, cell_y-2*MARGIN+LIN_H,
		                         M[lin][col].desenho);

	} 
	else if(M[lin][col].type == PAPEL) {
		al_draw_filled_rounded_rectangle(cell_x+MARGIN, cell_y+MARGIN,
		                                 cell_x-MARGIN+COL_W, cell_y-MARGIN+LIN_H,
		                                 COL_W/3, LIN_H/3, M[lin][col].desenho);

	} 	
	else if(M[lin][col].type == PET) {
		al_draw_filled_ellipse(cell_x+COL_W/2, cell_y+LIN_H/2,
		                       COL_W/2-MARGIN, LIN_H/2-MARGIN,
		                       M[lin][col].desenho);
	}

}


int newRecord(int score, int *record) {
	FILE *arq = fopen("recorde.txt", "r");
	*record = -1;
	if(arq != NULL) {
		fscanf(arq, "%d", record);
		fclose(arq);
	}
	if(*record < score ) {
		arq = fopen("recorde.txt", "w");
		fprintf(arq, "%d", score);
		fclose(arq);
		return 1;
	}
	return 0;
	
}


void draw_scenario(ALLEGRO_DISPLAY *display) {

	
	ALLEGRO_BITMAP *image61 = NULL;
	image61 = al_load_bitmap("Sel_061.png");
	al_draw_bitmap(image61, 0, 0, 0);
	al_set_target_bitmap(al_get_backbuffer(display)); 
	
	//SCORE
	sprintf(my_score, "score: %d", score);
	al_draw_text(size_f, al_map_rgb(255, 255, 255), LARGURA_TELA - 200, INFO_H/4, 0, my_score); 
	//PLAYS
	sprintf(my_plays, "jogadas: %d", plays);
	al_draw_text(size_f, al_map_rgb(255, 255, 255), 10, INFO_H/4, 0, my_plays);   

	int i, j;
	for(i=0; i<N_LINHAS; i++) {
		for(j=0; j<N_COLS; j++) {
			draw_candy(i, j);
		}
	}       

}


int clearSequence(int li, int lf, int ci, int cf) {
	int i, j, count=0;
	for(i=li; i<=lf; i++) {
		for(j=ci; j<=cf; j++) {
			count++;
			M[i][j].active = 0;
			M[i][j].desenho = desenhos[0];
		}
	}
	return count;
}

int processaMatriz() {
	//retorna a quantidade de pontos feitos
	int i, j, k, count = 0;
	int current, seq, ultimo;

	//procura na horizontal:
	for(i=0; i<N_LINHAS; i++) {
		current = M[i][0].type;
		seq = 1;
		for(j=1; j<N_COLS; j++) {
			if(current == M[i][j].type && current > 0) {
				seq++;
				if(j == N_COLS-1 && seq >=3)
					count += clearSequence(i, i, j-seq+1, N_COLS-1);
			}
			else {
				if(seq >= 3) {
					count += clearSequence(i, i, j-seq, j-1);
				}
				seq = 1;
				current = M[i][j].type;					
			}
		}
	}


	//procura na vertical:
	for(j=0; j<N_COLS; j++) {
		current = M[0][j].type;
		seq = 1;
		for(i=1; i<N_LINHAS; i++) {
			if(current == M[i][j].type && current > 0) {
				seq++;
				if(i == N_LINHAS-1 && seq >=3) 
					count += clearSequence(i-seq+1, N_LINHAS-1, j, j);
			}
			else {
				if(seq >= 3) {
					count += clearSequence(i-seq, i-1, j, j);
				}

				seq = 1;
				current = M[i][j].type;	

			}
		}
	}


	return count;


}

void atualizaOffset() {
	int i, j, offset;

	for(j=0; j<N_COLS; j++) {
		offset = 0;
		for(i=N_LINHAS-1; i>=0; i--) {
			M[i][j].offset_lin = offset;
			if(M[i][j].active == 0) {
				M[i][j].type = 0;
				offset++;
			}
		}
	}
}

void atualizaMatriz() {
	int i, j, offset;

	for(j=0; j<N_COLS; j++) {
		for(i=N_LINHAS-1; i>=0; i--) {
			offset = M[i][j].offset_lin;
			if(offset > 0) {
				M[i+offset][j].type = M[i][j].type;
				M[i+offset][j].active = M[i][j].active;
				M[i+offset][j].desenho = M[i][j].desenho;
				M[i][j].type = 0;
				M[i][j].active = 0;
				M[i][j].offset_lin = 0;
			}
		}
	}
}


void getCell(int x, int y, int *lin, int *col){
	x -= 80;
	y -= 80;
	*lin = y / LARGURA_LIN;
	*col = x / LARGURA_COL;
}

int distancia(int lin1, int col1, int lin2, int col2) {
	return sqrt(pow(lin1-lin2, 2) + pow(col1-col2, 2));
}


void swap(int lin1, int col1, int lin2, int col2){
	Lixo aux;
	aux = M[lin1][col1];
	M[lin1][col1] = M[lin2][col2];
	M[lin2][col2] = aux;
	plays--;
}




int main(int argc, char **argv){

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	//samples que guardam os efeitos sonoros
	ALLEGRO_SAMPLE *som_swap=NULL;
	ALLEGRO_SAMPLE *som_preenche=NULL;
	ALLEGRO_SAMPLE *som_telatroca=NULL;
	//musica de fundo
	ALLEGRO_AUDIO_STREAM *musica = NULL;
	//plano de fundo
	ALLEGRO_BITMAP *image61 = NULL;
	//imagem das peças
	ALLEGRO_BITMAP *Lata = NULL;
	ALLEGRO_BITMAP *pet = NULL;
	ALLEGRO_BITMAP *garrafa = NULL;
	ALLEGRO_BITMAP *papel = NULL;
	ALLEGRO_BITMAP *vazio = NULL;

	//----------------------- rotinas de inicializacao ---------------------------------------
	
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	//----------------------- rotinas de audio -----------------------------------------------
	
	if(!al_install_audio()){
        error_msg("Falha ao inicializar o audio");
        return 0;
    }

    if(!al_init_acodec_addon()){
        error_msg("Falha ao inicializar o codec de audio");
        return 0;
    }

    if (!al_reserve_samples(5)){
        error_msg("Falha ao reservar amostrar de audio");
        return 0;
    }

    //carrega os samples
    som_swap = al_load_sample("swap.mp3");
    if (!som_swap){
        error_msg( "Audio nao carregado" );
        return 0;
    }
    som_preenche = al_load_sample("complete.mp3");
    if (!som_preenche){
        error_msg( "Audio nao carregado" );
        al_destroy_sample(som_preenche);
        return 0;
    }

    /*som_telatroca = al_load_sample("nome_arquivo.extensão");
    if (!som_telatroca){
        error_msg("Audio nao carregado");
        al_destroy_sample(som_telatroca);
        return 0;
    }*/
 
    //carrega o stream
    musica = al_load_audio_stream("musicatabuleiro.mp3", 4, 1024);
    if (!musica)
    {
        error_msg( "Audio nao carregado" );
        al_destroy_sample(som_swap);
        al_destroy_sample(som_preenche);
        return 0;
    }
    //liga o stream no mixer
    al_attach_audio_stream_to_mixer(musica, al_get_default_mixer());
    //define que o stream vai tocar no modo repeat
    al_set_audio_stream_playmode(musica, ALLEGRO_PLAYMODE_LOOP)


	//-------------------------------------------------------------------------------------

	if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	display = al_create_display(LARGURA_TELA, ALTURA_TELA);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		al_destroy_sample(som_swap);
        al_destroy_sample(som_preenche);
		al_destroy_audio_stream(musica);
		return -1;
	}

	if(!al_install_mouse())
		fprintf(stderr, "failed to initialize mouse!\n");   


	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();
	//inicializa o modulo allegro que entende arquivos tff de fontes
	al_init_ttf_addon();

	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
	size_f = al_load_font("arial.ttf", 32, 1);   	

	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
	al_install_keyboard();
   //registra na fila de eventos que eu quero identificar quando a tela foi alterada
	al_register_event_source(event_queue, al_get_display_event_source(display));
   //registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra o teclado na fila de eventos:
	al_register_event_source(event_queue, al_get_keyboard_event_source());   
	//registra mouse na fila de eventos:
	al_register_event_source(event_queue, al_get_mouse_event_source());    
   //inicia o temporizador
	al_start_timer(timer);

	al_init_image_addon();

	Lata = al_load_bitmap("Lata.png");
	Pet = al_load_bitmap("pet.png");
	Garrafa = al_load_bitmap("garrafa.png");
	Papel = al_load_bitmap("papel.png");
	Vazio = al_load_bitmap("vazio.png");

	desenhos[0] = al_draw_bitmap(vazio, 0, 0, 0);
	desenhos[LATA] = al_draw_bitmap(Lata, 0, 0, 0);
	desenhos[GARRAFA] = al_draw_bitmap(Garrafa, 0, 0, 0);
	desenhos[PAPEL] = al_draw_bitmap(Papel, 0, 0, 0);
	desenhos[PET] = al_draw_bitmap(Pet, 0, 0, 0);

	//----------------------- fim das rotinas de inicializacao ---------------------------------------

	srand(2);
	iniciarJogo();
	int n_zeros = processaMatriz();
	while(n_zeros > 0) {
		do {
			atualizaOffset();
			atualizaMatriz();
		} while(processaMatriz());
		completaMatriz();
		n_zeros = processaMatriz();
	} 

	draw_scenario(display);
	al_flip_display();	

	int pontos, playing = 1, col_src, lin_src, col_dst, lin_dst, flag_animation=0;
	//enquanto playing for verdadeiro, faca:
	while(playing) {
		ALLEGRO_EVENT ev;
	  //espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				playing = 0;
			}

		}
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && !flag_animation) {
			getCell(ev.mouse.x, ev.mouse.y, &lin_src, &col_src);
		}
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && !flag_animation) {
			getCell(ev.mouse.x, ev.mouse.y, &lin_dst, &col_dst);
			if(distancia(lin_src, col_src, lin_dst, col_dst) == 1
				&& M[lin_src][col_src].type && M[lin_dst][col_dst].type) {
				swap(lin_src, col_src, lin_dst, col_dst);
				al_play_sample(som_swap, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
				flag_animation = 1; //nao permite que o usuario faca outro comando enquanto a animacao ocorre
			}

		}		
	    //se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		else if(ev.type == ALLEGRO_EVENT_TIMER) {
			pontos = processaMatriz();
			
			while(pontos > 0) {
			    draw_scenario(display);
				al_flip_display();
				pausa(timer);					
				atualizaOffset();
				atualizaMatriz();
				score+=pow(2,pontos);
				pontos = processaMatriz();
			}

		    //reinicializo a tela
		    draw_scenario(display);
			al_flip_display();		

			if(plays == 0)
				playing = 0;
			flag_animation = 0;
		}
	    //se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}

	} 

	al_rest(1);

	int record;
	//colore toda a tela de preto
	al_clear_to_color(al_map_rgb(230,240,250));
	sprintf(my_score, "Score: %d", score);
	al_draw_text(size_f, al_map_rgb(200, 0, 30), LARGURA_TELA/3, ALTURA_TELA/2, 0, my_score);
	if(newRecord(score, &record)) {
		al_draw_text(size_f, al_map_rgb(200, 20, 30), LARGURA_TELA/3, 100+ALTURA_TELA/2, 0, "NEW RECORD!");
	}
	else {
		sprintf(my_score, "Record: %d", record);
		al_draw_text(size_f, al_map_rgb(0, 200, 30), LARGURA_TELA/3, 100+ALTURA_TELA/2, 0, my_score);
	}
	//reinicializa a tela
	al_flip_display();	
	al_rest(2);	


	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_bitmap(image61);
	l_destroy_sample(som_swap);
    al_destroy_sample(som_preenche);
    al_destroy_audio_stream(musica)
	al_destroy_event_queue(event_queue);

	return 0;
}