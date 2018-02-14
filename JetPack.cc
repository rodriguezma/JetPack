#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>

esat::SpriteHandle map,spsheet,*explode;

int levelenemy,levelrocket;  // Tipo de enemigos por nivel

int op=1;
bool multiplayer=false, game_start=false;

const int windowx=1000,windowy=750,gravity=2;

 // STRUCTS
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
	
};
struct disparos *shots;


struct enemigos{
  float x,y;
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
  char pickup,fuel=0,piece;
  esat::SpriteHandle sprite;
};
struct nave *rocket;


struct objetos{
  float x,y=0;
  cuadrado colbox;
  int points;
  char pickup;
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
  player=(struct spaceman*)calloc(1,sizeof(struct spaceman));
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

void InitiateSpaceman(){
	player -> x = 500;
	player -> y = 646;
	player -> direction = 0;
	player -> vx = 6;
	player -> vy = 4;
}

void Initiate(){
  InitiateSpaceman();
  
  //PLATAFORMAS
  platforms=(struct terreno*)malloc(4*sizeof(struct terreno));
  platforms[0].colbox={125,281,311,311};
  platforms[1].colbox={469,375,593,405};
  platforms[2].colbox={750,187,936,218};
  platforms[3].colbox={0,719,999,749};
  
  //OBJETOS
  objects[0].points=100;
  objects[1].points=250;
  objects[2].points=250;
  objects[3].points=250;
  objects[4].points=250;
  objects[5].points=250;
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
		Player -> y -= Player -> vy;
		++Player -> animation;
		
	} else if (Player -> gravity){
		Player -> y += Player -> vy + 2;
		++Player -> animation;	
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
		if (Player -> x <= -56 ) {Player -> x = 1000;}
		if (!Player -> gravity) ++Player -> animation %= 4;
		
	} else if (esat::IsSpecialKeyPressed(dir1)) {
			Player -> direction = 1;
			Player -> x += Player -> vx;
			if (Player -> x >=1000) {Player -> x = -10;}
			if ((Player -> animation < 3 || Player -> animation >= 7) && !Player -> gravity) {
				Player -> animation = 3; 
			}
			if (!Player -> gravity) ++Player -> animation;
	}
	
}



void UpdateFrame(){
	esat::DrawSprite(map,0,0);
	esat::DrawSprite(*(player -> sprite + player -> animation) , player -> x, player -> y);
	
}


void FreeSprites(){
	free(player);
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
