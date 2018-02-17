#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

esat::SpriteHandle map,spsheet,*explode;
esat::SpriteHandle *playerwalk, *playerfly;
esat::SpriteHandle *martians;

int level=1;  // Tipo de enemigos por nivel
int time_=0;
int op=1,enemyamount;
int current_shots=0;
int current_enemies=0;
bool multiplayer=false, game_start=false;

const int windowx=1000,windowy=750,gravity=2;

struct cuadrado{
  float x1,x2,y1,y2;
};


struct spaceman{
  float x,y,vx,vy;
  cuadrado colbox;
  int lives=3,points=0;
  bool gravity=false;
  esat::SpriteHandle *sprite;
  char direction=0;
  char animation=0;
};
struct spaceman *player = NULL;


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
  bool alive=false;
  char color;
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
	playerwalk = (esat::SpriteHandle*) calloc (8,sizeof(esat::SpriteHandle));
	playerfly = (esat::SpriteHandle*) calloc (8,sizeof(esat::SpriteHandle));
  player=(struct spaceman*)malloc(1*sizeof(struct spaceman));
  player->sprite=(esat::SpriteHandle*)malloc(4*sizeof(esat::SpriteHandle));


  enemys = (struct enemigos*) malloc (6*sizeof(struct enemigos));
  for (int i=0; i<6; ++i){
    (enemys+i) -> sprite = (esat::SpriteHandle*)calloc(2,sizeof(esat::SpriteHandle));
  }

  //Walk animation
  *playerwalk=esat::SubSprite(spsheet,64,96,52,76); //IZQUIERDA
  *(playerwalk+1)=esat::SubSprite(spsheet,132,96,52,76);
  *(playerwalk+2)=esat::SubSprite(spsheet,204,96,52,76);
  *(playerwalk+3)=esat::SubSprite(spsheet,272,96,52,76);
  *(playerwalk+4)=esat::SubSprite(spsheet,68,188,52,76); //DERECHA
  *(playerwalk+5)=esat::SubSprite(spsheet,136,188,52,76);
  *(playerwalk+6)=esat::SubSprite(spsheet,208,188,52,76);
  *(playerwalk+7)=esat::SubSprite(spsheet,276,188,52,76);

  //JetPac animation
  *playerfly=esat::SubSprite(spsheet,577,102,53,74); //IZQUIERDA
  *(playerfly+1)=esat::SubSprite(spsheet,649,102,53,74);
  *(playerfly+2)=esat::SubSprite(spsheet,724,99,53,74);
  *(playerfly+3)=esat::SubSprite(spsheet,796,99,53,74);
  *(playerfly+4)=esat::SubSprite(spsheet,569,185,53,74); //DERECHA
  *(playerfly+5)=esat::SubSprite(spsheet,644,188,53,74);
  *(playerfly+6)=esat::SubSprite(spsheet,719,185,53,74);
  *(playerfly+7)=esat::SubSprite(spsheet,791,185,53,74);

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
  player -> y = 642;
  player -> direction = 0;
	player -> gravity = false;
  player -> animation = 0;
  player -> vx = 6;
  player -> vy = 4;
  player -> colbox = {500,545,642,715};
  //PLATAFORMAS
  platforms=(struct terreno*)malloc(5*sizeof(struct terreno));
  platforms[0].colbox={125,311,281,311};
  platforms[1].colbox={469,593,375,405};
  platforms[2].colbox={750,936,187,218};
  platforms[3].colbox={0,999,719,749};
  platforms[4].colbox={0,999,0,50};

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

	for(int i=0;i<5;i++){

		if(Col(colbox,platforms[i].colbox))
			return true;
	}

	return false;
}


// Utililzada al principio de cada nivel para cargar sprites enemigos
void SelectEnemiesLevel(){

  switch (level){

    case 1:{
      martians = (esat::SpriteHandle*) calloc (16,sizeof(esat::SpriteHandle));
      // TIPO 1
      //Verde
      martians[0] = esat::SubSprite(spsheet,682,327,49,32); // izq
      martians[1] = esat::SubSprite(spsheet,618,327,49,32);
      martians[2] = esat::SubSprite(spsheet,743,327,49,32), // der
      martians[3] = esat::SubSprite(spsheet,816,327,49,32);
      //Morado
      martians[4] = esat::SubSprite(spsheet,1005,0,49,32); // izq
      martians[5] = esat::SubSprite(spsheet,1005,33,49,32);
      martians[6] = esat::SubSprite(spsheet,1005,67,49,32); // der
      martians[7] = esat::SubSprite(spsheet,1005,100,49,32);
      //Rojo
      martians[8] = esat::SubSprite(spsheet,1005,134,49,32); // izq
      martians[9] = esat::SubSprite(spsheet,1005,170,49,32);
      martians[10] = esat::SubSprite(spsheet,1005,206,49,32); // der
      martians[11] = esat::SubSprite(spsheet,1005,243,49,32);
      //Azul
      martians[12] = esat::SubSprite(spsheet,1005,275,49,32); // izq
      martians[13] = esat::SubSprite(spsheet,1005,311,49,32);
      martians[14] = esat::SubSprite(spsheet,1005,344,49,32); // der
      martians[15] = esat::SubSprite(spsheet,1005,378,49,32);
    break;
    }

    case 2:{
      martians = (esat::SpriteHandle*) realloc (martians,8*sizeof(esat::SpriteHandle));
      //TIPO 2
      //Verde
      martians[0] = esat::SubSprite(spsheet,747,376,49,42);
      martians[1] = esat::SubSprite(spsheet,817,376,49,42);
      //Morado
      martians[2] = esat::SubSprite(spsheet,953,0,49,42),
      martians[3] = esat::SubSprite(spsheet,953,43,49,42);
      //Rojo
      martians[4] = esat::SubSprite(spsheet,955,87,49,42);
      martians[5] = esat::SubSprite(spsheet,955,131,49,42);
      //Azul
      martians[6] = esat::SubSprite(spsheet,955,176,49,42);
      martians[7] = esat::SubSprite(spsheet,955,217,49,42);
    break;
    }

    case 3:{
      //TIPO 3
      //Verde
      martians[0] = esat::SubSprite(spsheet,744,436,52,52);
      martians[1] = esat::SubSprite(spsheet,816,432,52,52);
      //Morado
      martians[2] = esat::SubSprite(spsheet,900,4,52,52),
      martians[3] = esat::SubSprite(spsheet,900,56,52,52);
      //Rojo
      martians[4] = esat::SubSprite(spsheet,900,125,52,52);
      martians[5] = esat::SubSprite(spsheet,900,184,52,52);
      //Azul
      martians[6] = esat::SubSprite(spsheet,905,253,52,52);
      martians[7] = esat::SubSprite(spsheet,905,305,52,52);
    break;
    }

    case 4:{
      //TIPO 4
      //Verde
      martians[0] = esat::SubSprite(spsheet,681,515,52,28); // izq
      martians[1] = esat::SubSprite(spsheet,744,512,52,28); // der
      //Morado
      martians[2] = esat::SubSprite(spsheet,1037,491,52,28),  // izq
      martians[3] = esat::SubSprite(spsheet,1090,491,52,28); // der
      //Rojo
      martians[4] = esat::SubSprite(spsheet,1039,539,52,28);  // izq
      martians[5] = esat::SubSprite(spsheet,1090,539,52,28); // der
      //Azul
      martians[6] = esat::SubSprite(spsheet,1038,585,52,28);  // izq
      martians[7] = esat::SubSprite(spsheet,1095,585,52,28); // der
    break;
    }

    case 5:{
      martians = (esat::SpriteHandle*) realloc (martians,4*sizeof(esat::SpriteHandle));
      //TIPO 5
      //Verde
      martians[0] = esat::SubSprite(spsheet,744,556,52,32);
      //Morado
      martians[1] = esat::SubSprite(spsheet,685,556,52,32),
      //Rojo
      martians[2] = esat::SubSprite(spsheet,806,556,52,32);
      //Azul
      martians[3] = esat::SubSprite(spsheet,807,512,52,32);
    break;
    }

    case 6:{
      //TIPO 6
      //Verde
      martians[0] = esat::SubSprite(spsheet,744,608,52,52);
      //Morado
      martians[1] = esat::SubSprite(spsheet,681,608,52,52),
      //Rojo
      martians[2] = esat::SubSprite(spsheet,864,608,52,52);
      //Azul
      martians[3] = esat::SubSprite(spsheet,807,608,52,52);
    break;
    }

    case 7:{
      martians = (esat::SpriteHandle*) realloc (martians,8*sizeof(esat::SpriteHandle));
      //TIPO 7
      //Verde
      martians[0] = esat::SubSprite(spsheet,679,672,51,41); // izq
      martians[1] = esat::SubSprite(spsheet,745,672,51,41); // der
      //Morado
      martians[2] = esat::SubSprite(spsheet,1056,0,51,41),  // izq
      martians[3] = esat::SubSprite(spsheet,1056,42,51,41); // der
      //Rojo
      martians[4] = esat::SubSprite(spsheet,1056,85,51,41);  // izq
      martians[5] = esat::SubSprite(spsheet,1056,128,51,41); // der
      //Azul
      martians[6] = esat::SubSprite(spsheet,1056,171,51,41);  // izq
      martians[7] = esat::SubSprite(spsheet,1056,212,51,41); // der
    break;
    }

    case 8:{
      martians = (esat::SpriteHandle*) realloc (martians,4*sizeof(esat::SpriteHandle));
      //TIPO 8
      //Verde
      martians[0] = esat::SubSprite(spsheet,744,732,52,48);
      //Morado
      martians[1] = esat::SubSprite(spsheet,686,732,52,48),
      //Rojo
      martians[2] = esat::SubSprite(spsheet,805,732,52,48);
      //Azul
      martians[3] = esat::SubSprite(spsheet,861,732,52,48);
    break;
    }
  }

}

// Utilizada al principio de cada nivel para reservar la memoria necesaria
void SpriteEnemyReserve(){

  if (level == 5 || level == 6 || level == 8){
    for (int i = 0; i < 6; ++i){
      ((enemys+i) -> sprite) = (esat::SpriteHandle*)realloc(((enemys+i) -> sprite),sizeof(esat::SpriteHandle));

    }

  } else {
    for (int i = 0; i < 6; ++i){
      ((enemys+i) -> sprite) = (esat::SpriteHandle*)realloc(((enemys+i) -> sprite),2*sizeof(esat::SpriteHandle));
    }
  }
}


//Función para imprimir sprites de nivel 1 (moco)
//Se tiene en cuenta color,direccion y animación
void Enemies1 (enemigos *marcianitos){

      switch (marcianitos -> color){

        case 0:{
          if (marcianitos -> direction == 0){
            marcianitos -> sprite[0] = martians[0];
            marcianitos -> sprite[1] = martians[1];

          }else if (marcianitos -> direction ==1){
            marcianitos -> sprite[0] = martians[2];
            marcianitos -> sprite[1] = martians[3];
          }
        break;
        }

        case 1:{
          if (marcianitos -> direction == 0){
            marcianitos -> sprite[0] = martians[4];
            marcianitos -> sprite[1] = martians[5];

          }else if (marcianitos -> direction ==1){
            marcianitos -> sprite[0] = martians[6];
            marcianitos -> sprite[1] = martians[7];
          }
        break;
        }

        case 2:{
          if (marcianitos -> direction == 0){
            marcianitos -> sprite[0] = martians[8];
            marcianitos -> sprite[1] = martians[9];

          }else if (marcianitos -> direction ==1){
            marcianitos -> sprite[0] = martians[10];
            marcianitos -> sprite[1] = martians[11];
          }
        break;
        }

        case 3:{
          if (marcianitos -> direction == 0){
            marcianitos -> sprite[0] = martians[12];
            marcianitos -> sprite[1] = martians[13];

          }else if (marcianitos -> direction ==1){
            marcianitos -> sprite[0] = martians[14];
            marcianitos -> sprite[1] = martians[15];
          }
        break;
        }
      }
  }


//Función para los sprites enemigos de nivel 5, 6 y 8
//Solo se tiene en cuenta el color,
// ya que ni cambian de animación ni de sprite con la direccion
void Enemies_568 (enemigos *marcianitos){

  switch (marcianitos -> color){

    case 0:{
      marcianitos -> sprite[0] = martians[0];
    break;
    }

    case 1:{
      marcianitos -> sprite[0] = martians[1];
    break;
    }

    case 2:{
      marcianitos -> sprite[0] = martians[2];
    break;
    }

    case 3:{
      marcianitos -> sprite[0] = martians[3];
    break;
    }

  }
}


//Función para los sprites enemigos de nivel 2,3,4 y 7
//Para los enemigos de nivel 2 y 3, no se tendra en cuenta la dirección, solo color y animación
//Para los enemigos de nivel 4 y 7 (pajaros y naves), se debe tener en cuenta que,
//al cambiar la animación cambiará la dirección del sprite
//En estos niveles vendría bien igualar la animación a la dirección constantemente,
//en lugar de incrementarla

void Enemies23_47 (enemigos *marcianitos){

  switch (marcianitos -> color){

    case 0:{
        marcianitos -> sprite[0] = martians[0];
        marcianitos -> sprite[1] = martians[1];
    break;
    }

    case 1:{
        marcianitos -> sprite[0] = martians[2];
        marcianitos -> sprite[1] = martians[3];
    break;
    }

    case 2:{
        marcianitos -> sprite[0] = martians[4];
        marcianitos -> sprite[1] = martians[5];
    break;
    }

    case 3:{
        marcianitos -> sprite[0] = martians[6];
        marcianitos -> sprite[1] = martians[7];
    break;
    }
  }
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

	if (esat::IsSpecialKeyPressed(key)) {
		Player -> gravity = true;
		cuadrado auxcolbox = Player -> colbox;
		auxcolbox.y1 -= Player -> vy;
		auxcolbox.y2 -= Player -> vy;
		++Player -> animation %= 4;
		if (!ColPlatforms(auxcolbox)){

			Player -> y -= Player -> vy;
			Player -> colbox.y1 -= Player -> vy;
			Player -> colbox.y2 -= Player -> vy;


		}

	} else if (Player -> gravity){
		cuadrado auxcolbox = Player -> colbox;
		auxcolbox.y1 += Player -> vy + 2;
		auxcolbox.y2 += Player -> vy + 2;
		if(!ColPlatforms(auxcolbox)){
			Player -> y += Player -> vy + 2;
			Player -> colbox.y1 += Player -> vy + 2;
			Player -> colbox.y2 += Player -> vy + 2;
			++Player -> animation %= 4;

		} else Player -> gravity = false;


	}else if (!Player -> gravity){
    cuadrado auxcolbox = Player -> colbox;
 		auxcolbox.y1 += Player -> vy + 2;
 		auxcolbox.y2 += Player -> vy + 2;
    if (!ColPlatforms(auxcolbox))
      Player -> gravity = true;
  }

}


void PlayerSprites(spaceman *Player){

	if (!Player -> gravity){
		switch (Player -> direction){

			case 0:{
				Player -> sprite[0] = playerwalk[0];
				Player -> sprite[1] = playerwalk[1];
				Player -> sprite[2] = playerwalk[2];
				Player -> sprite[3] = playerwalk[3];
				break;
			}

			case 1:{
				Player -> sprite[0] = playerwalk[4];
				Player -> sprite[1] = playerwalk[5];
				Player -> sprite[2] = playerwalk[6];
				Player -> sprite[3] = playerwalk[7];
				break;
			}
		}

	} else {
		switch (Player -> direction){

			case 0:{
				Player -> sprite[0] = playerfly[0];
				Player -> sprite[1] = playerfly[1];
				Player -> sprite[2] = playerfly[2];
				Player -> sprite[3] = playerfly[3];
				break;
			}

			case 1:{
				Player -> sprite[0] = playerfly[4];
				Player -> sprite[1] = playerfly[5];
				Player -> sprite[2] = playerfly[6];
				Player -> sprite[3] = playerfly[7];
				break;
			}
		}

	}
}

void Player1Control (spaceman *Player, esat::SpecialKey dir0, esat::SpecialKey dir1){

	if (esat::IsSpecialKeyPressed(dir0)) {
		cuadrado auxcolbox = Player -> colbox;
		auxcolbox.x1 -= Player -> vx;
		auxcolbox.x2 -= Player -> vx;
		if(!ColPlatforms(auxcolbox)){
			Player -> direction = 0;
			Player -> x -= Player -> vx;
			Player -> colbox.x1 -= Player -> vx;
			Player -> colbox.x2 -= Player -> vx;
			if (Player -> x <= -56 ) {
				Player -> x = 1000;
				Player -> colbox.x1 = Player -> x;
				Player -> colbox.x2 = Player -> x + 53;
			}
			if (!Player -> gravity) ++Player -> animation %= 4;
		}


	} else if (esat::IsSpecialKeyPressed(dir1)) {
		cuadrado auxcolbox = Player -> colbox;
		auxcolbox.x1 += Player -> vx;
		auxcolbox.x2 += Player -> vx;
		if(!ColPlatforms(auxcolbox)){
			Player -> direction = 1;
			Player -> x += Player -> vx;
			Player -> colbox.x1 += Player -> vx;
			Player -> colbox.x2 += Player -> vx;
			if (Player -> x >=1000){
				Player -> x = -10;
				Player -> colbox.x1 = -10;
				Player -> colbox.x2 = Player -> x + 53;
			}
			if (!Player -> gravity) ++Player -> animation %= 4;
		}

	}

}

void ItemSpawn(){
	int Rand_;
	Rand_ = rand()%15 + 1;
	switch(Rand_){
		case 1,2,3,4,5:
			objects[0].x = rand()%957;
			objects[0].y = 0;
			objects[0].active=1;
			objects[0].pickup=0;
			objects[0].colbox={objects[0].x, objects[0].x + 52, objects[0].y, objects[0].y + 36};
			break;//fuel
		case 6,7:
			objects[1].x = rand()%957;
			objects[1].y = 0;
			objects[1].active=1;
			objects[1].pickup=0;
			objects[1].colbox={objects[1].x, objects[1].x + 44, objects[1].y, objects[1].y + 44};
			break;//diamond
		case 8,9:
			objects[2].x = rand()%957;
			objects[2].y = 0;
			objects[2].active=1;
			objects[2].pickup=0;
			objects[2].colbox={objects[2].x, objects[2].x + 52, objects[2].y, objects[2].y + 32};
			break;//emerald
		case 10,11:
			objects[3].x = rand()%957;
			objects[3].y = 0;
			objects[3].active=1;
			objects[3].pickup=0;
			objects[3].colbox={objects[3].x, objects[3].x + 52, objects[3].y, objects[3].y + 28};
			break;//gold
		case 12,13:
			objects[4].x = rand()%957;
			objects[4].y = 0;
			objects[4].active=1;
			objects[4].pickup=0;
			objects[4].colbox={objects[4].x, objects[4].x + 52, objects[4].y, objects[4].y + 44};
			break;//atomic
		case 14,15:
			objects[5].x = rand()%957;
			objects[5].y = 0;
			objects[5].active=1;
			objects[5].pickup=0;
			objects[5].colbox={objects[5].x, objects[5].x + 52, objects[5].y, objects[5].y + 40};
			break;//radioactive
	}
}

void DrawItems(){
	
	for(int i = 0;i < 6;++i){
		if(objects[i].active==1){//If active, draw object

			esat::DrawSprite(objects[i].sprite, objects[i].x, objects[i].y);
			if(!ColPlatforms(objects[i].colbox)&&objects[i].pickup!=1){//No colision = go down
			objects[i].y += 5;
			objects[i].colbox.y1 += 5;
			objects[i].colbox.y2 += 5;
			}else if(objects[0].pickup == 1){//Pickup fuel
			objects[0].x = player -> x;
			objects[0].y = player -> y;
			objects[0].colbox.x1 = player -> colbox.x1;
			objects[0].colbox.x2 = player -> colbox.x2;
			objects[0].colbox.y1 = player -> colbox.y1;
			objects[0].colbox.y2 = player -> colbox.y2;
			}
			if(Col(objects[i].colbox, player -> colbox)){//Pickup other items
				objects[i].pickup = 1;
				if(i!=0){objects[i].active = 0;}
			}
			DrawCol(objects[i].colbox);
		}
	}
	
}

void EnemiesSpawn(){
	float time = esat::Time();
	srand(time);
	for(int i =0;i<6;i++){
		if(!enemys[i].alive){
			enemys[i].alive = true;
			if(rand()%2==0){
				enemys[i].colbox = {0,50,0,0};
				enemys[i].colbox.y1 = (float)(70 + rand()%600);
				enemys[i].colbox.y2 = enemys[i].colbox.y1 + 50;
				enemys[i].vx = rand()%3 + 1;
				enemys[i].vy = rand()%5 - 2;
				enemys[i].direction = 1;
			}else{
				enemys[i].colbox = {949,999,0,0};
				enemys[i].colbox.y1 = (float)(70 + rand()%600);
				enemys[i].colbox.y2 = enemys[i].colbox.y1 + 50;
				enemys[i].vx = - (rand()%3 + 1);
				enemys[i].vy = rand()%5 - 2;
				enemys[i].direction = 0;
			}
		}
	}
}

void EnemiesMovement(){
	for(int i=0;i<6;i++){
		cuadrado auxcolbox = enemys[i].colbox;
		auxcolbox.x1 += enemys[i].vx;
		auxcolbox.x2 += enemys[i].vx;
		if(ColPlatforms(auxcolbox)){
			enemys[i].vx = - enemys[i].vx;
			if(enemys[i].direction == 0)
				enemys[i].direction = 1;
			else
				enemys[i].direction = 0;
		}
		auxcolbox = enemys[i].colbox;
		auxcolbox.y1 += enemys[i].vy;
		auxcolbox.y2 += enemys[i].vy;
		if(ColPlatforms(auxcolbox)){
			enemys[i].vy = - enemys[i].vy;
			if(enemys[i].direction == 0)
				enemys[i].direction = 1;
			else
				enemys[i].direction = 0;
		}
		enemys[i].colbox.x1 += enemys[i].vx;
		enemys[i].colbox.x2 += enemys[i].vx;
		enemys[i].colbox.y1 += enemys[i].vy;
		enemys[i].colbox.y2 += enemys[i].vy;
	}
}

void EnemiesLimits(){
	for(int i=0;i<6;i++){
		if(enemys[i].colbox.x1>1000){
			enemys[i].colbox.x1 = -50;
			enemys[i].colbox.x2 = 0;
		}else if(enemys[i].colbox.x1<-50){
			enemys[i].colbox.x1 = 1000;
			enemys[i].colbox.x2 = 1050;
		}
	}
}

void UpdateFrame(){
	esat::DrawSprite(map,0,0);
	esat::DrawSprite(*(player -> sprite + player -> animation) , player -> x, player -> y);

	DrawShoots();
	DrawCol((*player).colbox);
	DrawCol(platforms[0].colbox);
	DrawCol(platforms[1].colbox);
	DrawCol(platforms[2].colbox);
	DrawCol(platforms[3].colbox);

	for(int i=0;i<6;i++){
		DrawCol(enemys[i].colbox);
	}



}


void FreeSprites(){
	free(playerwalk);
	free(playerfly);
  free(martians);
  free(enemys);
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
		PlayerSprites(player);
		Player1Control(player,esat::kSpecialKey_Left,esat::kSpecialKey_Right);
		Fly(player,esat::kSpecialKey_Up);
    	Shot(esat::kSpecialKey_Space);
    	ShotsMovement();
    	EnemiesSpawn();
    	EnemiesMovement();
    	EnemiesLimits();

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
