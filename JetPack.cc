#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>

esat::SpriteHandle map,spsheet;

int levelenemy,levelrocket;  // Tipo de enemigos por nivel

int op=1;
bool multiplayer=false, game_start=false;

const int windowx=1000,windowy=750;

struct cuadrado{
  float x1,x2,y1,y2;
};


struct spaceman{
  float x,y,vx = 5,vy = 5;
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
  float x,y;
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


void CutSprites()
{
	map = esat::SpriteFromFile("./Recursos/Sprites/Map.gif");
	spsheet = esat::SpriteFromFile("./Recursos/Sprites/SpriteGeneral.png");
	player = (struct spaceman*) calloc (1,sizeof(struct spaceman));
	player -> sprite = (esat::SpriteHandle*) calloc (8,sizeof(esat::SpriteHandle));
	*(player -> sprite) = esat::SubSprite (spsheet,64,96,52,76);
	*(player -> sprite+1) = esat::SubSprite (spsheet,132,96,52,76);
	*(player -> sprite+2) = esat::SubSprite (spsheet,204,96,52,76);
	*(player -> sprite+3) = esat::SubSprite (spsheet,272,96,52,76);
	*(player -> sprite+4) = esat::SubSprite (spsheet,68,188,52,76);
	*(player -> sprite+5) = esat::SubSprite (spsheet,136,188,52,76);
	*(player -> sprite+6) = esat::SubSprite (spsheet,208,188,52,76);
	*(player -> sprite+7) = esat::SubSprite (spsheet,276,188,52,76);
	
}


void Init()
{
	(player -> x) = 500;
	(player -> y) = 646;
	(player -> direction) = 0;
	player -> vx = 6;
	player -> vy = 6;
	
}




void Player1Control( spaceman *Player)
{
	if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Left)){
		Player -> direction = 0;
		Player -> x -= Player -> vx;
		if(Player -> x <= -56 ){Player -> x = 1000;}
		++Player -> animation %= 4;
		
	} else if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right)){
			Player -> direction = 1;
			Player -> x += Player -> vx;
			if(Player -> x ==1000){Player -> x = -10;}
			if (Player -> animation < 3 || Player -> animation >= 7) 
				Player -> animation = 3; 
			++Player -> animation;
	}
	
}


void UpdateFrame()
{
	esat::DrawSprite(map,0,0);
	esat::DrawSprite(*(player -> sprite + player -> animation) , player -> x, player -> y);
	
}


void FreeSprites()
{
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
  
	CutSprites();
	Init();

  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
	last_time = esat::Time();

	if(!game_start){
		Menu();
	}
	
	esat::DrawBegin();
    esat::DrawClear(0,0,0);
    
	if(game_start){
		Player1Control(player);
    
    
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
