#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>

esat::SpriteHandle map,spsheet;
esat::SpriteHandle *player;

int op_menu=1;
bool multiplayer=false, game_start=false;

const int windowx=1000,windowy=750;

struct unit{
  float x,y;
  int v;
  int tipe;
  bool alive;
	esat::SpriteHandle *sprite;
	char dir = 0;
	char anim;
};

 unit Pack1;

void CutSprites()
{
	map = esat::SpriteFromFile("./Recursos/Sprites/Map.gif");
	spsheet = esat::SpriteFromFile("./Recursos/Sprites/SpriteGeneral.png");
	
	player = (esat::SpriteHandle*) calloc (8,sizeof(esat::SpriteHandle));
	*player = esat::SubSprite (spsheet,64,96,52,76);
	*(player+1) = esat::SubSprite (spsheet,132,96,52,76);
	*(player+2) = esat::SubSprite (spsheet,204,96,52,76);
	*(player+3) = esat::SubSprite (spsheet,272,96,52,76);
	*(player+4) = esat::SubSprite (spsheet,68,188,52,76);
	*(player+5) = esat::SubSprite (spsheet,136,188,52,76);
	*(player+6) = esat::SubSprite (spsheet,208,188,52,76);
	*(player+7) = esat::SubSprite (spsheet,276,188,52,76);
	
}


void Init()
{
	Pack1.sprite = (esat::SpriteHandle*) calloc (4,sizeof(esat::SpriteHandle));
	Pack1.x = 500;
	Pack1.y = 646;
	Pack1.dir = 0;
	
}


void PlayerMov( unit *Player)
{
	switch(Player -> dir){
		
		case 0:
			*(Player -> sprite) = *player;
			*(Player -> sprite+1) = *(player+1);
			*(Player -> sprite+2) = *(player+2);
			*(Player -> sprite+3) = *(player+3);
			break;
			
		case 1:
			*(Player -> sprite) = *(player+4);
			*(Player -> sprite+1) = *(player+5);
			*(Player -> sprite+2) = *(player+6);
			*(Player -> sprite+3) = *(player+7);
			break;
		
	}
	
}


void Player1Control( unit *Player)
{
	if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Left)){
		Player -> dir = 0;
		Player -> x -= 5;
		if(Player -> x <= -56 ){Player -> x = 1000;}
		++Player -> anim %= 4;
		
	} else if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right)){
		Player -> dir = 1;
		Player -> x += 5;
		if(Player -> x ==1000){Player -> x = -10;}
		++Player -> anim %= 4;
	}
	
}


void UpdateFrame()
{
	esat::DrawSprite(map,0,0);
	esat::DrawSprite(*(Pack1.sprite + Pack1.anim) , Pack1.x, Pack1.y);
	
}


void FreeSprites()
{
	free(Pack1.sprite);
	free(player);
	esat::SpriteRelease(spsheet);
	esat::SpriteRelease(map);
	
	
	
}

void Colition( cuadrado colbox1, cuadrado colbox2){
	
	if(colbox1.x2 < colbox2.x1 || colbox1.x1 > colbox2.x2)
		return false;
	else if(colbox1.y2 < colbox2.y1 || colbox1.y1 > colbox2.y2)
		return false;
	else{
		return true;
	}
}	

void Shot(){
	disparos *auxshots = NULL;
	
	if(esat::IsSpecialKeyDown(kSpecialKey_Space)){
		
		nShots++;
		shots = (disparos*)realloc(shots,sizeof(disparos)*nShots);
		
		auxshots = shots + nShots - 1;

		(*auxshots).x = (*player).x;
		(*auxshots).y = (*player).y;
		(*auxshots).direction = (*player).direction;
		(*auxshots).colbox.x1 = (*auxshots).x;
		(*auxshots).colbox.y1 = (*auxshots).y;
		(*auxshots).colbox.x2 = (*auxshots).x + 50;
		(*auxshots).colbox.y2 = (*auxshots).y + 5;
		
	}
}

void Menu(){

  esat::DrawSetTextSize(100);
  esat::DrawSetFillColor(255,255,255);
  esat::DrawSetTextSize(50);

  if(op_menu==1){
    esat::DrawSetFillColor(158,55,249);
  }else{
    esat::DrawSetFillColor(255,255,255);
  }
  esat::DrawText(250,250,"1");
  esat::DrawText(400,250,"1 PLAYER GAME");

  if(op_menu==2){
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
    op_menu=1;
  }if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_2) || esat::IsKeyDown('2')){
    op_menu=2;
  }if(esat::IsSpecialKeyDown(esat::kSpecialKey_Keypad_5) || esat::IsKeyDown('5')){
    game_start=true;
    if(op_menu==2){
      multiplayer=true;
    }
  }
  
}

void Interface(){

  esat::DrawSetTextSize(40);
  esat::DrawSetFillColor(255,255,0);

  esat::DrawText(100,70,"000000");
  esat::DrawText(465,70,"000000");
  esat::DrawText(845,70,"000000");

  esat::DrawSetFillColor(255,255,255);
  esat::DrawText(252,34,"0");
  esat::DrawText(730,34,"0");

}

int esat::main(int argc, char **argv) {
 
  double current_time,last_time;
  unsigned char fps=25;
	
	
  esat::WindowInit(windowx,windowy);
  WindowSetMouseVisibility(true);
  
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
		Player1Control(&Pack1);
		PlayerMov(&Pack1);


		UpdateFrame();
		Interface();
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
