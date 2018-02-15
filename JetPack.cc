#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

esat::SpriteHandle map,spsheet,*explode;

int level=0;  // Tipo de enemigos por nivel
int time_=0;
int op=1;
int current_shots=0;
bool multiplayer=false, game_start=false;

const int windowx=1000,windowy=750,gravity=2;

struct cuadrado{
  float x1,x2,y1,y2;
};


struct spaceman{
  float x,y,vx,vy;
  cuadrado colbox;
  int lives=3,points=0;
  bool gravity=true;
  esat::SpriteHandle *sprite;
  char direction=0;
  char animation;
};
struct spaceman *player;


struct disparos{
  float x,y;
  cuadrado colbox;
  int width,v;
  int direction;
};
struct disparos *shots;


struct enemigos{
  float x,y,vx,vy;
  cuadrado colbox;
  int points;
  bool alive=true;
  esat::SpriteHandle *sprite;
  char direction=0;
  char animation;
};
struct enemigos *enemys;


struct nave{
  float x,y;
  cuadrado colbox;
  int points=100;
  char pickup,fuel=0,piece;
  esat::SpriteHandle sprite;
};
struct nave *rocket;


struct objetos{
  float x,y=0;
  cuadrado colbox;
  int points;
  char pickup,active=0;
  esat::SpriteHandle sprite;
};
struct objetos *objects;


struct terreno{
  cuadrado colbox;
};
struct terreno *platforms;



void CutInitialSprites(){
	map = esat::SpriteFromFile("./Recursos/Sprites/Map.gif");
	spsheet=esat::SpriteFromFile("./Recursos/Sprites/SpriteGeneral.png");
  
  // MAIN CHARACTER SPRITES
  player=(struct spaceman*)malloc(1*sizeof(struct spaceman));
  player->sprite=(esat::SpriteHandle*)malloc(16*sizeof(esat::SpriteHandle));
  //Walk animation
  *(player->sprite)=esat::SubSprite(spsheet,64,96,52,76); //IZQUIERDA
  *(player->sprite+1)=esat::SubSprite(spsheet,132,96,52,76);
  *(player->sprite+2)=esat::SubSprite(spsheet,204,96,52,76);
  *(player->sprite+3)=esat::SubSprite(spsheet,272,96,52,76);
  *(player->sprite+4)=esat::SubSprite(spsheet,68,188,52,76); //DERECHA
  *(player->sprite+5)=esat::SubSprite(spsheet,136,188,52,76);
  *(player->sprite+6)=esat::SubSprite(spsheet,208,188,52,76);
  *(player->sprite+7)=esat::SubSprite(spsheet,276,188,52,76);
  //JetPac animation
  player->sprite[8]=esat::SubSprite(spsheet,572,100,53,74); //IZQUIERDA
  player->sprite[9]=esat::SubSprite(spsheet,646,100,53,74);
  player->sprite[10]=esat::SubSprite(spsheet,720,96,53,74);
  player->sprite[11]=esat::SubSprite(spsheet,792,96,53,74);
  player->sprite[12]=esat::SubSprite(spsheet,569,185,53,74); //DERECHA
  player->sprite[13]=esat::SubSprite(spsheet,644,188,53,74);
  player->sprite[14]=esat::SubSprite(spsheet,719,185,53,74);
  player->sprite[15]=esat::SubSprite(spsheet,791,185,53,74);
  
  // EXPLOSION SPRITES
  explode=(esat::SpriteHandle*)malloc(3*sizeof(esat::SpriteHandle));
  
  explode[0]=esat::SubSprite(spsheet,408,220,84,56); //GRANDE
  explode[1]=esat::SubSprite(spsheet,408,160,84,56); //MEDIANA
  explode[2]=esat::SubSprite(spsheet,408,100,84,56); //PEQUEÑA
  
  // objects SPRITES
  objects=(struct objetos*)malloc(6*sizeof(struct objetos));
  
  objects[0].sprite=esat::SubSprite(spsheet,68,320,52,36); //FUEL
  objects[1].sprite=esat::SubSprite(spsheet,68,380,44,44); //GEMA
  objects[2].sprite=esat::SubSprite(spsheet,60,444,52,32); //BOLA
  objects[3].sprite=esat::SubSprite(spsheet,60,504,52,28); //LINGOTES
  objects[4].sprite=esat::SubSprite(spsheet,60,564,52,44); //TRIANGULO
  objects[5].sprite=esat::SubSprite(spsheet,60,632,52,40); //RADIACTIVO
}



void Initiate(){
  player -> x = 500;
  player -> y = 646;
  player -> direction = 0;
  player -> vx = 6;
  player -> vy = 4;
  player -> colbox = {500,553,646,719};
  //PLATAFORMAS
  platforms=(struct terreno*)malloc(3*sizeof(struct terreno));
  platforms[0].colbox={125,311,281,311};
  platforms[1].colbox={469,593,375,405};
  platforms[2].colbox={750,936,187,218};
  platforms[3].colbox={0,999,719,749};
  
  //OBJETOS
  objects[0].points=100;
  objects[1].points=250;
  objects[2].points=250;
  objects[3].points=250;
  objects[4].points=250;
  objects[5].points=250;
}

void DrawCol(cuadrado colbox){
	
	esat::DrawSetStrokeColor(255,255,255);
	esat::DrawSetFillColor(255,255,255);

	esat::DrawLine(colbox.x1 , colbox.y1 , colbox.x2 , colbox.y1);
	esat::DrawLine(colbox.x2 , colbox.y1 , colbox.x2 , colbox.y2);
	esat::DrawLine(colbox.x2 , colbox.y2 , colbox.x1 , colbox.y2);
	esat::DrawLine(colbox.x1 , colbox.y2 , colbox.x1 , colbox.y1);
}

bool Col (cuadrado colbox1, cuadrado colbox2){
	if(colbox1.x2 < colbox2.x1 || colbox1.x1 > colbox2.x2)
		return false;
	else if(colbox1.y2 < colbox2.y1 || colbox1.y1 > colbox2.y2)
		return false;
	else{
		return true;
	}
}

bool ColPlatforms(cuadrado colbox){
	terreno *auxplats = platforms;
	bool iscolliding;
	while(auxplats != NULL){
		iscolliding = Col(colbox,(*auxplats).colbox);
		auxplats++;
	}
	return iscolliding;
}

void Shot (esat::SpecialKey key){
	if (esat::IsSpecialKeyDown(key)){

		current_shots++;
		shots = (disparos*)realloc(shots,sizeof(disparos)*current_shots);
		disparos *auxshot = shots + (current_shots-1);
		
		(*auxshot).direction = (*player).direction;
		(*auxshot).colbox.x1 = (*player).x;
		(*auxshot).colbox.y1 = (*player).y;
		(*auxshot).colbox.x2 = (*auxshot).colbox.x1 + 50;
		(*auxshot).colbox.y2 = (*auxshot).colbox.y1 + 10;

	}
}

void ShotsMovement (){
	disparos *auxshot = shots;
	
	for(int i=0;i<current_shots;i++){
		switch((*auxshot).direction){
			case 0:
				(*auxshot).colbox.x1 -=10;
				(*auxshot).colbox.x2 -=10;
				break;
			case 1:
				(*auxshot).colbox.x1 +=10;
				(*auxshot).colbox.x2 +=10;
				break;
		}
		auxshot++;
	}
}

void DrawShoots (){
	disparos *auxshot = shots;
	esat::DrawSetStrokeColor(255,255,255);
	esat::DrawSetFillColor(255,255,255);
	for(int i=0;i<current_shots;i++){
		esat::DrawLine((*auxshot).colbox.x1 , (*auxshot).colbox.y1 + 5, (*auxshot).colbox.x2 , (*auxshot).colbox.y1 + 5);
		auxshot++;
	}
}




void Fly (spaceman *Player, esat::SpecialKey key){
	
	if (esat::IsSpecialKeyPressed(key) && 
	!Player -> gravity && Player -> direction == 0) {
		Player -> animation = 8;
		Player -> gravity = true;
		
	} else if (esat::IsSpecialKeyPressed(key) && 
	!Player -> gravity && Player -> direction == 1) {
		Player -> animation = 12;
		Player -> gravity = true;
	}
	
	if (Player -> gravity && Player -> animation >= 11 && 
	Player -> direction == 0) {
		Player -> animation = 8;
		
	}else if (Player -> gravity && Player -> animation >= 15 && 
	Player -> direction == 1) {
		Player -> animation = 12;
	}
	
	if (esat::IsSpecialKeyPressed(key) && 
	Player -> gravity && Player -> y >= 60) {
		cuadrado auxcolbox = Player -> colbox;
		auxcolbox.y1 -= Player -> vy;
		auxcolbox.y2 -= Player -> vy;
		//if (!ColPlatforms(auxcolbox)){
			Player -> y -= Player -> vy;
			Player -> colbox.y1 -= Player -> vy;
			Player -> colbox.y2 -= Player -> vy;
			++Player -> animation;
		//}
		
		
	} else if (Player -> gravity){
		cuadrado auxcolbox = Player -> colbox;
		auxcolbox.y1 += Player -> vy + 2;
		auxcolbox.y2 += Player -> vy + 2;
		//if(!ColPlatforms(auxcolbox)){
			Player -> y += Player -> vy + 2;
			Player -> colbox.y1 += Player -> vy + 2;
			Player -> colbox.y2 += Player -> vy + 2;
			++Player -> animation;	
		//}
		
	}
	
	if (Player -> y >= 646 && Player -> gravity) {
	  Player -> gravity = false;
		if (Player -> direction == 0) {
			Player -> animation = 0;
		} else if (Player -> direction = 1) Player -> animation = 4;
	}

}



void Player1Control (spaceman *Player, esat::SpecialKey dir0, esat::SpecialKey dir1){
	
	if (esat::IsSpecialKeyPressed(dir0)) {
		Player -> direction = 0;
		Player -> x -= Player -> vx;
		Player -> colbox.x1 -= Player -> vx;
		Player -> colbox.x2 -= Player -> vx;
		if (Player -> x <= -56 ) {Player -> x = 1000;}
		if (!Player -> gravity) ++Player -> animation %= 4;
		
	} else if (esat::IsSpecialKeyPressed(dir1)) {
			Player -> direction = 1;
			Player -> x += Player -> vx;
			Player -> colbox.x1 += Player -> vx;
			Player -> colbox.x2 += Player -> vx;
			if (Player -> x >=1000) {Player -> x = -10;}
			if ((Player -> animation < 3 || Player -> animation >= 7) && !Player -> gravity) {
				Player -> animation = 3; 
			}
			if (!Player -> gravity) ++Player -> animation;
	}
	
}

void ItemSpawn(){
	int Rand_;
	Rand_ = rand()%15 + 1;
	printf("%d",Rand_);
	switch(Rand_){
		case 1,2,3,4,5:
			objects[0].x = rand()%957;
			objects[0].y = 0;
			objects[0].active=1;
			break;
		case 6,7:
			objects[1].x = rand()%957;
			objects[1].y = 0;
			objects[1].active=1;
			break;
		case 8,9:
			objects[2].x = rand()%957;
			objects[2].y = 0;
			objects[2].active=1;
			break;
		case 10,11:
			objects[3].x = rand()%957;
			objects[3].y = 0;
			objects[3].active=1;
			break;
		case 12,13:
			objects[4].x = rand()%957;
			objects[4].y = 0;
			objects[4].active=1;
			break;
		case 14,15:
			objects[5].x = rand()%957;
			objects[5].y = 0;
			objects[5].active=1;
			break;
	}
}


void UpdateFrame(){
	esat::DrawSprite(map,0,0);
	esat::DrawSprite(*(player -> sprite + player -> animation) , player -> x, player -> y);
	
	for(int i=0;i<6;++i){
		if(objects[i].active==1){
			esat::DrawSprite(objects[i].sprite,objects[i].x,objects[i].y);
			if(objects[i].y<=678){
			objects[i].y += 5;
			}
		}
	}
	DrawShoots();
	DrawCol((*player).colbox);
	DrawCol(platforms[0].colbox);
	DrawCol(platforms[1].colbox);
	DrawCol(platforms[2].colbox);
	DrawCol(platforms[3].colbox);


	
}


void FreeSprites(){
	free(player);
	free(objects);
	free(explode);
	esat::SpriteRelease(spsheet);
	esat::SpriteRelease(map);
	
}

void Menu(){

  esat::DrawSetTextSize(100);
  esat::DrawSetFillColor(255,255,255);
  esat::DrawText(150,100,"JetPac Game Selection");
  esat::DrawSetTextSize(50);

  if(op==1){
    esat::DrawSetFillColor(158,55,249);
  }else{
    esat::DrawSetFillColor(255,255,255);
  }
  esat::DrawText(250,250,"1");
  esat::DrawText(400,250,"1 PLAYER GAME");

  if(op==2){
    esat::DrawSetFillColor(158,55,249);
  }else{
    esat::DrawSetFillColor(255,255,255);
  }
  esat::DrawText(250,400,"2");
  esat::DrawText(400,400,"2 PLAYERS GAME");

  esat::DrawSetFillColor(255,255,255);
  esat::DrawText(250,550,"5");
  esat::DrawText(400,550,"START GAME");

  esat::DrawSetTextSize(40);
  esat::DrawText(300,700,"1983");
  esat::DrawText(400,700," A.C.G");
  esat::DrawText(500,700,"ALL RIGHTS RESERVED");

  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_1) || esat::IsKeyDown('1')){
    op=1;
  }if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_2) || esat::IsKeyDown('2')){
    op=2;
  }if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_5) || esat::IsKeyDown('5')){
    game_start=true;
    if(op==2){
      multiplayer=true;
    }
  }
  
}

int esat::main(int argc, char **argv) {
 
  double current_time,last_time;
  unsigned char fps=25;
  srand(time(NULL));
	
  esat::WindowInit(windowx,windowy);
  WindowSetMouseVisibility(true);
  esat::DrawSetTextFont("Recursos/fonts/Pixelopolis 9000.ttf");
  
	CutInitialSprites();
	Initiate();

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
	last_time = esat::Time();

	if(!game_start){
		Menu();
	}
	
	esat::DrawBegin();
    esat::DrawClear(0,0,0);
    
	if(game_start){
		Player1Control(player,esat::kSpecialKey_Left,esat::kSpecialKey_Right);
		Fly(player,esat::kSpecialKey_Up);
    	Shot(esat::kSpecialKey_Space);
    	ShotsMovement();
    		
		if(time_%250==0){
			ItemSpawn();
			printf("hola!");
		}
		++time_;
		UpdateFrame();
		
	}
		
    esat::DrawEnd();
	
	do{
    current_time = esat::Time();
    }while((current_time-last_time)<=1000.0/fps);
    esat::WindowFrame();
  }
	
	FreeSprites();
  
  esat::WindowDestroy();
  return 0;
}
