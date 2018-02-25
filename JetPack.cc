#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

esat::SpriteHandle map,spsheet,*explode;
esat::SpriteHandle *playerwalk, *playerfly;
esat::SpriteHandle *martians;
esat::SpriteHandle *ship=NULL, *shipieces=NULL, *turbo;

int level_1=6; // Nivel Enemigo (0-7)
int level_2=0; // Nivel Enemigo (0-7)
int ex_level_1=0; //Nivel nave (0-15)/ Cada 4 niveles se divide en piezas
int ex_level_2=0; //Nivel nave (0-15)/ Cada 4 niveles se divide en piezas
bool nextlevel=false;
int UfoTime=0;
int time_=0;
int op=1;
int current_shots=0;
int turn=0; //Quien juega
bool multiplayer=false, game_start=false;

const int windowx=1000,windowy=750,gravity=2;
const int k_current_enemies = 8;

struct cuadrado{
  float x1,x2,y1,y2;
};


struct spaceman{
  float x,y,vx,vy;
  cuadrado colbox;
  int lives=3,points=0;
  bool gravity=false;
  char explodeanim = 0;
  bool dead = false;
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
  bool alive, dead;
  char explodeanim = 0;
  char color;
  esat::SpriteHandle *sprite;
  char direction=0;
  char animation;
};
struct enemigos *enemys = NULL;


struct nave{
  float x,y;
  cuadrado colbox;
  char dir;
  int points=100;
  char pickup = 0,fuel=0,piece = 0;
  char anim;
  esat::SpriteHandle sprite;
};
struct nave *rocket=NULL;


struct objetos{
  float x,y=0;
  cuadrado colbox;
  int points;
  char pickup,active=0,drop=0;
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

  // SHIP
  turbo = (esat::SpriteHandle*)realloc(turbo,2*sizeof(esat::SpriteHandle));
  ship = (esat::SpriteHandle*) realloc (ship,6*sizeof(esat::SpriteHandle));
  shipieces = (esat::SpriteHandle*) realloc (shipieces,4*sizeof(esat::SpriteHandle));

  // ENEMIES
  enemys = (struct enemigos*) malloc (k_current_enemies*sizeof(struct enemigos));
  for (int i=0; i<k_current_enemies; ++i){
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

  explode[2]=esat::SubSprite(spsheet,414,100,71,49); //GRANDE
  explode[1]=esat::SubSprite(spsheet,415,165,71,49); //MEDIANA
  explode[0]=esat::SubSprite(spsheet,413,219,71,49); //PEQUEÑA

  // objects SPRITES
  objects=(struct objetos*)malloc(6*sizeof(struct objetos));

  objects[0].sprite=esat::SubSprite(spsheet,68,320,52,36); //FUEL
  objects[1].sprite=esat::SubSprite(spsheet,68,380,44,44); //GEMA
  objects[2].sprite=esat::SubSprite(spsheet,60,444,52,32); //BOLA
  objects[3].sprite=esat::SubSprite(spsheet,60,504,52,28); //LINGOTES
  objects[4].sprite=esat::SubSprite(spsheet,60,564,52,44); //TRIANGULO
  objects[5].sprite=esat::SubSprite(spsheet,60,632,52,40); //RADIACTIVO

  // Turbo NAVE
  turbo[0]=esat::SubSprite(spsheet,348,636,56,52);
  turbo[1]=esat::SubSprite(spsheet,436,636,56,52);

}

// Utililzada al principio de cada nivel para cargar sprites enemigos
void SelectEnemiesLevel(int level){

  switch (level){

    case 0:{
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

    case 1:{
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

    case 2:{
      martians = (esat::SpriteHandle*) realloc (martians,8*sizeof(esat::SpriteHandle));
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

    case 3:{
      martians = (esat::SpriteHandle*) realloc (martians,8*sizeof(esat::SpriteHandle));
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

    case 4:{
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

    case 5:{
      martians = (esat::SpriteHandle*) realloc (martians,4*sizeof(esat::SpriteHandle));
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

    case 6:{
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

    case 7:{
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
void SpriteEnemyReserve(int level){

  if (level == 4 || level == 5 || level == 7){
    for (int i = 0; i < k_current_enemies; ++i){
      ((enemys+i) -> sprite) = (esat::SpriteHandle*)realloc(((enemys+i) -> sprite),sizeof(esat::SpriteHandle));

    }

  } else {
    for (int i = 0; i < k_current_enemies; ++i){
      ((enemys+i) -> sprite) = (esat::SpriteHandle*)realloc(((enemys+i) -> sprite),2*sizeof(esat::SpriteHandle));
    }
  }
}

void SpriteShipLevel(int exlevel){

  if (exlevel < 4){
    //NAVE 1

    ship[0] = esat::SubSprite(spsheet,4,893,50,143); //Blanco
    ship[1] = esat::SubSprite(spsheet,52,893,50,143);//Pintado 1
    ship[2] = esat::SubSprite(spsheet,103,893,50,143);//Pintado 2
    ship[3] = esat::SubSprite(spsheet,152,893,50,143);//Pintado 3
    ship[4] = esat::SubSprite(spsheet,203,893,50,143);//Pintado 4
    ship[5] = esat::SubSprite(spsheet,254,893,50,143);//Morado


    shipieces[0] = esat::SubSprite(spsheet,239,556,50,50);//Pieza Base
    shipieces[1] = esat::SubSprite(spsheet,233,725,50,98);//Medio Cuerpo
    shipieces[2] = esat::SubSprite(spsheet,240,487,50,50);//Pieza Mitad
    shipieces[3] = esat::SubSprite(spsheet,238,420,50,50);//Pieza Cabeza


  }else if (exlevel < 8){
    //NAVE 2
    ship[0] = esat::SubSprite(spsheet,308,893,50,143);
    ship[1] = esat::SubSprite(spsheet,362,893,50,143);
    ship[2] = esat::SubSprite(spsheet,414,893,50,143);
    ship[3] = esat::SubSprite(spsheet,466,893,50,143);
    ship[4] = esat::SubSprite(spsheet,518,893,50,143);
    ship[5] = esat::SubSprite(spsheet,572,893,50,143);

    shipieces[0] = esat::SubSprite(spsheet,336,554,50,50);//Pieza Base
    shipieces[1] = esat::SubSprite(spsheet,327,720,50,98);//Medio Cuerpo
    shipieces[2] = esat::SubSprite(spsheet,336,488,50,50);//Pieza Mitad
    shipieces[3] = esat::SubSprite(spsheet,333,420,50,50);//Pieza Cabeza

  }else if (exlevel < 12){
    //NAVE 3
    ship[0] = esat::SubSprite(spsheet,623,893,50,143);
    ship[1] = esat::SubSprite(spsheet,676,893,50,143);
    ship[2] = esat::SubSprite(spsheet,730,893,50,143);
    ship[3] = esat::SubSprite(spsheet,782,893,50,143);
    ship[4] = esat::SubSprite(spsheet,834,893,50,143);
    ship[5] = esat::SubSprite(spsheet,888,893,50,143);

    shipieces[0] = esat::SubSprite(spsheet,460,558,50,50);//Pieza Base
    shipieces[1] = esat::SubSprite(spsheet,455,726,50,98);//Medio Cuerpo
    shipieces[2] = esat::SubSprite(spsheet,460,489,50,50);//Pieza Mitad
    shipieces[3] = esat::SubSprite(spsheet,460,420,50,50);//Pieza Cabeza

  }else {
    //NAVE 4
    ship[0] = esat::SubSprite(spsheet,942,893,50,143);
    ship[1] = esat::SubSprite(spsheet,996,893,50,143);
    ship[2] = esat::SubSprite(spsheet,1050,893,50,143);
    ship[3] = esat::SubSprite(spsheet,1104,893,50,143);
    ship[4] = esat::SubSprite(spsheet,1056,701,50,143);
    ship[5] = esat::SubSprite(spsheet,1108,701,50,143);

    shipieces[0] = esat::SubSprite(spsheet,587,555,50,50);//Pieza Base
    shipieces[1] = esat::SubSprite(spsheet,580,724,50,98);//Medio Cuerpo
    shipieces[2] = esat::SubSprite(spsheet,587,490,50,50);//Pieza Mitad
    shipieces[3] = esat::SubSprite(spsheet,587,420,50,50);//Pieza Cabeza

  }
}

void InitShip(int level){

  if (level == 0 || level%4 == 0){
    rocket = (struct nave*) realloc (rocket, 3*sizeof(struct nave));
    rocket[0].sprite = shipieces[0];
    rocket[0].colbox = {662,710,670,720};
    rocket[0].piece = 0;
    rocket[1].sprite = shipieces[2];
    rocket[1].colbox = {508,558,325,375};
    rocket[1].piece = 0;
    rocket[2].sprite = shipieces[3];
    rocket[2].colbox = {190,240,230,280};
    rocket[2].piece = 0;

  }else {
    rocket = (struct nave*) realloc (rocket, sizeof(struct nave));
    rocket[0].fuel = 0;
    rocket[0].sprite = ship[rocket[0].fuel];
    rocket[0].colbox = {662,710,570,720};
    rocket[0].piece = 2;
  }

}

void PlayerInit(){
  player -> x = 500;
  player -> y = 642;
  player -> direction = 0;
	player -> gravity = false;
  player -> animation = 0;
  player -> dead = false;
  player -> explodeanim = 0;
  player -> vx = 10;
  player -> vy = 6;
  player -> colbox = {500,545,642,715};

  if(multiplayer){
    (player+1) -> x = 500;
    (player+1) -> y = 642;
    (player+1) -> direction = 0;
    (player+1) -> gravity = false;
    (player+1) -> animation = 0;
    (player+1) -> dead = false;
    (player+1) -> explodeanim = 0;
    (player+1) -> vx = 6;
    (player+1) -> vy = 4;
    (player+1) -> colbox = {500,545,642,715};
  }

}

void Initiate(){

  PlayerInit();
  player -> lives = 3;
  player -> points = 0;
  (player+1) -> lives = 3;
  (player+1) -> points = 0;

  //PLATAFORMAS
  platforms=(struct terreno*)malloc(5*sizeof(struct terreno));
  platforms[0].colbox={125,311,281,311};
  platforms[1].colbox={469,593,375,405};
  platforms[2].colbox={750,936,187,218};
  platforms[3].colbox={-50,1050,719,749};
  platforms[4].colbox={-50,1050,0,50};

  //OBJETOS
  objects[0].points=100;
  objects[1].points=250;
  objects[2].points=250;
  objects[3].points=250;
  objects[4].points=250;
  objects[5].points=250;

  for(int i = 0; i<6; ++i){
    objects[i].active = 0;
    objects[i].x = 0;
    objects[i].y = 0;
    objects[i].pickup = 0;
    objects[i].drop = 0;
  }

  //ENEMIGOS
  if(turn%2==0){
    SelectEnemiesLevel(level_1);
  }else{
    SelectEnemiesLevel(level_2);
  }
  for (int i = 0; i < k_current_enemies; ++i){
    enemys[i].color = rand()%4;
    enemys[i]. animation = 0;
    enemys[i].alive = false;
    enemys[i].dead = false;
  }

  //NAVE
  if(turn%2==0){
    InitShip(ex_level_1);
  }else{
    InitShip(ex_level_2);
  }
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

void DrawPiece(nave *ship){


  if (ship -> pickup == 1){
    ship -> colbox.x1 = player -> colbox.x1;
    ship -> colbox.y1 = player -> colbox.y1;
    ship -> colbox.x2 = player -> colbox.x1+(esat::SpriteWidth(ship->sprite));
    ship -> colbox.y2 = player -> colbox.y1+(esat::SpriteHeight(ship->sprite));

  }else if (Col(player -> colbox,ship -> colbox) && ship -> piece == 0){
    ship -> pickup = 1;
  }
}

void LeavePiece(nave *ship){
  char speed = 5;

  if ((ship->pickup == 1 && ship->colbox.x1 > rocket[0].colbox.x1 &&
      ship->colbox.x1 < rocket[0].colbox.x2)){
    ship -> pickup = 0;
    ship -> piece = 1;
    ship -> colbox.x1 = rocket[0].colbox.x1;

  }else if (ship -> piece == 1 && !ColPlatforms(ship->colbox)){
    ship -> colbox.y1 += speed;
    ship -> colbox.y2 += speed;

  } else if (ColPlatforms(ship->colbox)) ship -> piece = 0;

  if (player->dead && ship->pickup == 1){
    ship -> pickup = 0;
    ship -> piece = 1;
  }
}

void AddPiece(nave *spaceship){

  if (Col(spaceship -> colbox, rocket[0].colbox)){
    spaceship -> piece = 5;
    ++rocket[0].piece;

    if (rocket[0].piece < 2){
      rocket[0].colbox.y1 -= 50;
      rocket[0].sprite = shipieces[rocket[0].piece];

    }else {
      rocket[0].fuel = 0;
      rocket[0].colbox.y1 -= 43;
      rocket[0].sprite = ship[rocket[0].fuel];

  }
    spaceship -> colbox.x1 = 0;
    spaceship -> colbox.x2 = 0;
    spaceship -> colbox.y1 = 0;
    spaceship -> colbox.y2 = 0;
    esat::SpriteRelease(spaceship -> sprite);

  }

}

void Pieces(nave *piece1, nave *piece2){

  if (rocket[0].piece == 0){
    DrawPiece (piece1);
    LeavePiece(piece1);
    AddPiece(piece1);

  } else if (rocket[0].piece == 1){
    DrawPiece (piece2);
    LeavePiece(piece2);
    AddPiece(piece2);

  }

}

void FuelRecharge(objetos *fuel, nave *spaceship){

	if (Col(fuel -> colbox, spaceship -> colbox) && spaceship -> piece == 2 && fuel -> active == 1){
		++spaceship -> fuel;
		spaceship -> sprite = ship[spaceship -> fuel];
		fuel -> active = 0;
		fuel -> y = 0;
		fuel -> x = 51;
		fuel -> pickup=0;
		fuel -> drop = 0;
	}
}


void Enemies0 (enemigos *marcianitos){

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

void Enemies_457 (enemigos *marcianitos){

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

void Enemies12_36 (enemigos *marcianitos){

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

void EnemySprite (enemigos *Tmarcianos, int level){

  if (level == 0){
    Enemies0(Tmarcianos);

  } else if (level == 4 || level == 5 || level == 7) {
    Enemies_457(Tmarcianos);

  } else Enemies12_36(Tmarcianos);

}

void Shot (esat::SpecialKey key){
	if (esat::IsSpecialKeyDown(key)){

		current_shots++;
		shots = (disparos*)realloc(shots,sizeof(disparos)*current_shots);
		disparos *auxshot = shots + (current_shots-1);

		(*auxshot).direction = (*player).direction;
		(*auxshot).colbox.x1 = (*player).x;
		(*auxshot).colbox.y1 = (*player).y + ((esat::SpriteHeight(player->sprite[player -> animation]))/2);
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

void PlayerDead(spaceman *character){  //Colisiones jugador/enemigos

  for (int i = 0; i < k_current_enemies; i++){
    if (Col(character->colbox, enemys[i].colbox) && !character->dead && !enemys[i].dead){
      printf("colision con enemigo: %d\n",i);
      printf("(%f,%f|%f,%f),(%f,%f|%f,%f)",character -> colbox.x1,character -> colbox.y1,character -> colbox.x2,character -> colbox.y2,enemys[i].colbox.x1,enemys[i].colbox.y1,enemys[i].colbox.x2,enemys[i].colbox.y2);
      enemys[i].dead = true;
      enemys[i].explodeanim = 0;
      character -> dead = true;
      character -> explodeanim = 0;
      if(multiplayer && turn%2==1){
        --(character+1) -> lives;
      }else{
        --character -> lives;
      }
      if(multiplayer){
        if(turn%2==0 && ((character+1)->lives!=0)){
          turn++;
        } else if(turn%2==1 && (character->lives!=0)){
          turn++;
        }
      }
    }
  }
  printf("%d,%d,%d\n",character -> dead,character -> explodeanim , time_%10);
  if (character -> dead && character -> explodeanim >= 2 && time_%10 == 0){

    PlayerInit();

  }else if (character -> dead && time_%10 == 0){
    ++character -> explodeanim;
  }

}

void ItemSpawn(){
	int Rand_;
	if(objects[0].active==0 && rocket[0].piece == 2 && rocket[0].fuel != 5){Rand_ = 1;}else{Rand_ = rand()%5 + 2;}
	switch(Rand_){
		case 1:
			objects[0].x = rand()%957;
			objects[0].y = 51;
			objects[0].active=1;
			objects[0].pickup=0;
			objects[0].drop = 0;
			objects[0].colbox={objects[0].x, objects[0].x + 52, objects[0].y, objects[0].y + 36};
			break;//fuel
		case 2:
			objects[1].x = rand()%957;
			objects[1].y = 51;
			objects[1].active=1;
			objects[1].pickup=0;
			objects[1].drop = 0;
			objects[1].colbox={objects[1].x, objects[1].x + 44, objects[1].y, objects[1].y + 44};
			break;//diamond
		case 3:
			objects[2].x = rand()%957;
			objects[2].y = 51;
			objects[2].active=1;
			objects[2].pickup=0;
			objects[2].drop = 0;
			objects[2].colbox={objects[2].x, objects[2].x + 52, objects[2].y, objects[2].y + 32};
			break;//emerald
		case 4:
			objects[3].x = rand()%957;
			objects[3].y = 51;
			objects[3].active=1;
			objects[3].pickup=0;
			objects[3].drop = 0;
			objects[3].colbox={objects[3].x, objects[3].x + 52, objects[3].y, objects[3].y + 28};
			break;//gold
		case 5:
			objects[4].x = rand()%957;
			objects[4].y = 51;
			objects[4].active=1;
			objects[4].pickup=0;
			objects[4].drop = 0;
			objects[4].colbox={objects[4].x, objects[4].x + 52, objects[4].y, objects[4].y + 44};
			break;//atomic
		case 6:
			objects[5].x = rand()%957;
			objects[5].y = 51;
			objects[5].active=1;
			objects[5].pickup=0;
			objects[5].drop = 0;
			objects[5].colbox={objects[5].x, objects[5].x + 52, objects[5].y, objects[5].y + 40};
			break;//radioactive
	}
}

void DrawItems(){

	for(int i = 0;i < 6;++i){
		if(objects[i].active==1){//If active, draw object
			esat::DrawSprite(objects[i].sprite, objects[i].x, objects[i].y);
			DrawCol(objects[i].colbox);

			if((!ColPlatforms(objects[i].colbox) && objects[i].pickup == 0)||(!ColPlatforms(objects[i].colbox) && objects[i].drop == 1)){
				objects[i].y += 5;
				objects[i].colbox.y1 += 5;
				objects[i].colbox.y2 += 5;
				FuelRecharge(&objects[0],&rocket[0]);
			}

			if(Col(objects[i].colbox,player -> colbox) && objects[i].pickup == 0){
				if(i==0){
					objects[i].pickup = 1;

          if(multiplayer && turn%2==1){
            (player+1) -> points += objects[i].points;
          }else{
            player -> points += objects[i].points;
          }


					player -> points += objects[i].points;

				}else{
					objects[i].active = 0;
					player -> points += objects[i].points;
				}
			}

			if(objects[i].pickup == 1 && objects[i].drop == 0){
				objects[i].x = player -> x;
				objects[i].y = player -> y;
				objects[i].colbox.x1 = player -> colbox.x1;
				objects[i].colbox.x2 = player -> colbox.x2;
				objects[i].colbox.y1 = player -> colbox.y1;
				objects[i].colbox.y2 = player -> colbox.y2;

				if (objects[i].active == 1 && ((objects[i].colbox.x1 > rocket[i].colbox.x1 &&
        objects[i].colbox.x1 < rocket[i].colbox.x2) || player -> dead) ){
				  objects[i].pickup = 0;
					if(objects[i].colbox.x1 > rocket[i].colbox.x1 && objects[i].colbox.x1 < rocket[i].colbox.x2){
					objects[i].drop = 1;
					}
          if (!player -> dead) objects[i].x = rocket[i].colbox.x1;
					objects[i].colbox={objects[i].x, objects[i].x + 52, objects[i].y, objects[i].y + 36};
				}
			}

		}
	}

}

void InitMeteorites(enemigos *marcianito){

  marcianito -> colbox.x2 = marcianito -> colbox.x1 + 49;
  marcianito -> colbox.y2 = marcianito -> colbox.y1 + 32;

  marcianito -> vy = rand()%3 - 1;

  if(marcianito -> direction==1){
    marcianito -> vx = 3;
  }else{
    marcianito -> vx = -3;
  }

}

void InitBalls(enemigos *marcianito){

  marcianito -> colbox.x2 = marcianito -> colbox.x1 + 52;
  marcianito -> colbox.y2 = marcianito -> colbox.y1 + 52;

  if(rand()%2==0)
    marcianito -> vx = 3;
  else
    marcianito -> vx = -3;

  if(marcianito -> direction==1){
    marcianito -> vx = 3;
  }else{
    marcianito -> vx = -3;
  }

}

void InitFurBalls(enemigos *marcianito){

  marcianito -> colbox.x2 = marcianito -> colbox.x1 + 49;
  marcianito -> colbox.y2 = marcianito -> colbox.y1 + 42;

  marcianito -> vy = rand()%7 - 3;
  if(marcianito -> direction==1){
    marcianito -> vx = 3;
  }else{
    marcianito -> vx = -3;
  }

}

void InitDarts(enemigos *marcianito){

  marcianito -> colbox.x2 = marcianito -> colbox.x1 + 52;
  marcianito -> colbox.y2 = marcianito -> colbox.y1 + 28;

  marcianito -> vy = 0;
  marcianito -> vx = 0;

}

void InitUfos(enemigos *marcianito){

  marcianito -> colbox.x2 = marcianito -> colbox.x1 + 52;
  marcianito -> colbox.y2 = marcianito -> colbox.y1 + 32;

  marcianito -> vy = rand()%7 - 3;
  if(marcianito -> direction==1){
    marcianito -> vx = 3;
  }else{
    marcianito -> vx = -3;
  }

}

void InitCrosses(enemigos *marcianito){
  marcianito -> colbox.x2 = marcianito -> colbox.x1 + 52;
  marcianito -> colbox.y2 = marcianito -> colbox.y1 + 52;

  if(rand()%2==0)
    marcianito -> vx = 3;
  else
    marcianito -> vx = -3;

  if(marcianito -> direction==1){
    marcianito -> vx = 3;
  }else{
    marcianito -> vx = -3;
  }
}

void InitSpaceShips(enemigos *marcianito){
  marcianito -> colbox.x2 = marcianito -> colbox.x1 + 51;
  marcianito -> colbox.y2 = marcianito -> colbox.y1 + 41;

  marcianito -> vy = rand()%3 - 1;

  if(marcianito -> direction==1){
    marcianito -> vx = 3;
  }else{
    marcianito -> vx = -3;
  }

}

void InitBlobs(enemigos *marcianito){

  marcianito -> colbox.x2 = marcianito -> colbox.x1 + 52;
  marcianito -> colbox.y2 = marcianito -> colbox.y1 + 48;

  marcianito -> vy = 0;
  marcianito -> vx = 0;

}

void InitEnemies(enemigos *marcianito){
  switch(level_1){
    case 0:

    InitMeteorites(marcianito);
    break;
    case 1:
    InitFurBalls(marcianito);
    break;
    case 2:
    InitBalls(marcianito);
    break;
    case 3:
    InitDarts(marcianito);
    break;
    case 4:
    InitUfos(marcianito);
    break;
    case 5:
    InitCrosses(marcianito);
    break;
    case 6:
    InitSpaceShips(marcianito);
    break;
    case 7:
    InitBlobs(marcianito);
    break;
  }
}

void EnemiesSpawn(int level){

  for(int i =0;i<k_current_enemies;i++){
    if(!enemys[i].alive){
      enemys[i].alive = true;
      enemys[i].colbox = {0,0,(float)(100 + rand()%550),0};
      if(rand()%2==1){
        enemys[i].colbox.x1 = 2;
        enemys[i].direction = 1;
      }else{
        enemys[i].colbox.x1 = 945;
        enemys[i].direction = 0;
      }

      InitEnemies(&enemys[i]);

      switch(level){
        case 0:
          enemys[i].points=25;break;
        case 1:
          enemys[i].points=80;break;
        case 2:
          enemys[i].points=40;break;
        case 3:
          enemys[i].points=55;break;
        case 4:
          enemys[i].points=50;break;
        case 5:
          enemys[i].points=60;break;
        case 6:
          enemys[i].points=25;break;
        case 7:
          enemys[i].points=50;break;
      }
    }
  }
}


void BallsMovement(){

  for(int i=0;i<k_current_enemies;i++){
    if (!enemys[i].dead){
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
      }
    }
  }
}

void DartsMovement(){
  for(int i=0;i<k_current_enemies;i++){
    if((*player).colbox.y1 + 35 > enemys[i].colbox.y1
    && (*player).colbox.y1 + 35 < enemys[i].colbox.y2){
      switch(enemys[i].direction){
        case 0:
        enemys[i].vx = -10;
        break;
        case 1:
        enemys[i].vx = 10;
        break;
      }
    }
    if(ColPlatforms(enemys[i].colbox) && !enemys[i].dead){
      enemys[i].dead = true;
      enemys[i].explodeanim = 0;
    }
  }
}

void UfoMovement(){
  double wx,wy,vx,vy,m;
  wx = (*player).colbox.x1 + 25;
  wy = (*player).colbox.y1 + 30;

  for(int i=0;i<k_current_enemies;i++){
    if(!enemys[i].dead){
      if(UfoTime<esat::Time()){
        vx = enemys[i].colbox.x1 + 25;
        vy = enemys[i].colbox.y1 + 25;

        vx = wx - vx;
        vy = wy - vy;

        m=(vx*vx)+(vy*vy);
        m=sqrt(m);

        vx = vx / m;
        vy = vy / m;

        vx *= 4;
        vy *= 4;

        enemys[i].vx = vx;
        enemys[i].vy = vy;
        printf("%f,%f\n",vx,vy);
      }

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
      if(ColPlatforms(auxcolbox))
        enemys[i].vy = - enemys[i].vy;

  }
}
if(UfoTime<esat::Time())
  UfoTime = esat::Time()+1000;
}

void MeteoritesMovement(){
  for(int i=0;i<k_current_enemies;i++){
    if (!enemys[i].dead && ColPlatforms( enemys[i].colbox)){
      enemys[i].dead = true;
      enemys[i].explodeanim = 0;
    }
  }
}

void RandMovement(){

  for(int i=0;i<k_current_enemies;i++){
    if(!enemys[i].dead){
      if(UfoTime<esat::Time()){
        switch(rand()%3){
          case 0:
            enemys[i].vy = 3;
          break;
          case 1:
            enemys[i].vy = 0;
          break;
          case 2:
            enemys[i].vy = -3;
          break;
        }

      }

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
      if(ColPlatforms(auxcolbox))
        enemys[i].vy = - enemys[i].vy;

  }
}
if(UfoTime<esat::Time())
  UfoTime = esat::Time()+1000;
}

void EnemiesBehavior(){

  switch(level_1){
    case 0:
    MeteoritesMovement();
    break;
    case 1:
    BallsMovement();
    break;
    case 2:
    RandMovement();
    break;
    case 3:
    DartsMovement();
    break;
    case 4:
    UfoMovement();
    break;
    case 5:
    RandMovement();
    break;
    case 6:
    MeteoritesMovement();
    break;
    case 7:
    UfoMovement();
    break;

  }



}


void EnemiesMovement(){
  for(int i=0; i<k_current_enemies; i++){
    if (!enemys[i].dead){
      enemys[i].colbox.x1 += enemys[i].vx;
      enemys[i].colbox.x2 += enemys[i].vx;
      enemys[i].colbox.y1 += enemys[i].vy;
      enemys[i].colbox.y2 += enemys[i].vy;
    }
  }
}

void EnemiesLimits(){
	for(int i=0;i<k_current_enemies;i++){
		if(enemys[i].colbox.x1>1000){
			enemys[i].colbox.x1 -= 1000;
			enemys[i].colbox.x2 -= 1000;
		}else if(enemys[i].colbox.x1<0){
			enemys[i].colbox.x1 += 1000;
			enemys[i].colbox.x2 += 1000;
		}
	}
}

void EnemiesShoting(){
  for (int i = 0; i < k_current_enemies; ++i){
    for (int k = 0; k < current_shots; ++k){

      if (Col(enemys[i].colbox,shots[k].colbox) && !enemys[i].dead){
        enemys[i].dead = true;
        enemys[i].explodeanim = 0;
      }
    }
  }
}

void EnemiesDead(){


  for (int i = 0; i < k_current_enemies; ++i){
    if (enemys[i].dead && enemys[i].explodeanim > 2){
      enemys[i].color = rand()%4;
      enemys[i].explodeanim = 0;
      enemys[i].dead = false;
      enemys[i].alive = false;
    }
  }
}


void DrawEnemies(int level){

  for (int i = 0; i < k_current_enemies; ++i){
    if (!enemys[i].dead){
      if(turn%2==0){

        EnemySprite(enemys+i,level_1);

      }else{
        EnemySprite(enemys+i,level_2);
      }

      esat::DrawSprite((enemys[i].sprite[enemys[i].animation]), enemys[i].colbox.x1, enemys[i].colbox.y1);

      if (time_ % 5 ==0 && level < 3){
        ++enemys[i].animation %= 2;
      }else if (level == 3 || level == 6) enemys[i].animation = enemys[i].direction;

    }else {

      esat::DrawSprite(explode[enemys[i].explodeanim], enemys[i].colbox.x1, enemys[i].colbox.y1);

      if (time_%10 == 0) ++enemys[i].explodeanim;
    }
  }
}

void DrawShip(int level){

  if (level == 0 || level%4 == 0){
    for (int i=0; i<3; ++i){
      if (rocket[i].piece != 5)
      esat::DrawSprite(rocket[i].sprite, rocket[i].colbox.x1, rocket[i].colbox.y1);

    }

  }else esat::DrawSprite(rocket[0].sprite, rocket[0].colbox.x1, rocket[0].colbox.y1);
}



void GameOver(spaceman *Player){

	if (Player -> lives <= 0 && !Player -> dead && !multiplayer){
		game_start = false;
    SpriteShipLevel(ex_level_1);
    Initiate();
  }

  if(multiplayer && (Player -> lives <= 0 && !Player -> dead) && ((Player+1) -> lives <= 0 && !(Player+1) -> dead)){
      game_start =false;
      SpriteShipLevel(ex_level_2);
      Initiate();
    }

}

bool NextLevel(spaceman *Player){

  if (rocket[0].fuel == 5 && Col(Player->colbox, rocket[0].colbox)){
    return true;

  }else return false;

}


void ToNextLevel(spaceman *Player){

  int sc1,sc2,lv1,lv2;

  if (nextlevel){
    rocket[0].colbox.y1 -= (Player -> vy *rocket[0].dir);
    rocket[0].colbox.y2 -= (Player -> vy *rocket[0].dir);
    esat::DrawSprite(map,0,0);
    DrawShip(ex_level_1);
    esat::DrawSprite(turbo[rocket[0].anim],rocket[0].colbox.x1-2,rocket[0].colbox.y2-5);
    ++rocket[0].anim%=2;


  } else if (NextLevel(Player) && !nextlevel){
      nextlevel = true;
      rocket[0].anim = 0;
      rocket[0].dir = 1;
  }

  if (rocket[0].colbox.y2<(-52)){
    rocket[0].dir *= (-1);
    esat::Sleep(800);

  } else if (rocket[0].colbox.y2>720 && rocket[0].dir<0){
    nextlevel=false;
    rocket[0].dir=1;
    rocket[0].fuel=0;
    sc1=Player->points;
    lv1=Player->lives;
    sc2=(Player+1)->points;
    lv2=(Player+1)->lives;
    ++level_1%=8;
    ++level_2%=8;
    ++ex_level_1%=16;
    ++ex_level_2%=16;
    SpriteEnemyReserve(level_1);
    SpriteEnemyReserve(level_2);
    SpriteShipLevel(ex_level_1);
    SpriteShipLevel(ex_level_2);
    Initiate();
    Player->points=sc1;
    Player->lives=lv1;
    (Player+1)->points=sc2;
    (Player+1)->lives=lv2;
  }

}



void UpdateFrame(){

	esat::DrawSprite(map,0,0);
  if (!player -> dead){
	   esat::DrawSprite(*(player -> sprite + player -> animation) , player -> x, player -> y);
  } else esat::DrawSprite(explode[player -> explodeanim], player -> x, player -> y);

  if(turn%2==0){
    DrawEnemies(level_1);

    DrawShip(ex_level_1);
  }else{
    DrawEnemies(level_2);
    DrawShip(ex_level_2);
  }
	DrawShoots();
	DrawItems();
	DrawCol((*player).colbox);

	DrawCol(platforms[0].colbox);
	DrawCol(platforms[1].colbox);
	DrawCol(platforms[2].colbox);
	DrawCol(platforms[3].colbox);
  DrawCol(rocket[0].colbox);
  //DrawCol(rocket[1].colbox);

	for(int i=0;i<k_current_enemies;i++){
		DrawCol(enemys[i].colbox);
	}

}

void FreeSprites(){
	free(playerwalk);
	free(playerfly);
  free(martians);
  for (int i = 0; i < k_current_enemies; ++i){
    //esat::SpriteRelease(enemys[i].sprite);
      free(enemys[i].sprite);
  }
	free(platforms);
	free(shots);
  free(enemys);
  free(player -> sprite);
	free(player);
  free(ship);
  free(shipieces);
  free(rocket);
	free(objects);
	free(explode);
  free(turbo);
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
		Initiate();
    if(op==2){
      multiplayer=true;
    }

  }

}

void Interface(spaceman *Player){

	char *score1,*score2,*live1,*live2;

	esat::DrawSetFillColor(255,255,0);
	score1=(char*)calloc(6,sizeof(char));
	live1=(char*)calloc(1,sizeof(char));

  itoa(Player->points,score1,10);

  if(multiplayer){
    score2=(char*)calloc(6,sizeof(char));
    live2=(char*)calloc(1,sizeof(char));
    itoa((Player+1)->points,score2,10);
  }

	if(Player->points<10){
		esat::DrawText(100,70,"00000");
		esat::DrawText(175,70,score1);
	}else if(Player->points<100){
		esat::DrawText(100,70,"0000");
		esat::DrawText(160,70,score1);
	}else if(Player->points<1000){
		esat::DrawText(100,70,"000");
		esat::DrawText(145,70,score1);
	}else if(Player->points<10000){
		esat::DrawText(100,70,"00");
		esat::DrawText(130,70,score1);
	}else if(Player->points<100000){
		esat::DrawText(100,70,"0");
		esat::DrawText(115,70,score1);
	}else{
		esat::DrawText(100,70,score1);
	}

	esat::DrawText(455,70,"999999");

  if(multiplayer){
    if((Player+1)->points<10){
      esat::DrawText(845,70,"00000");
      esat::DrawText(920,70,score2);
    }else if((Player+1)->points<100){
      esat::DrawText(845,70,"0000");
      esat::DrawText(905,70,score2);
    }else if((Player+1)->points<1000){
      esat::DrawText(845,70,"000");
      esat::DrawText(890,70,score2);
    }else if((Player+1)->points<10000){
      esat::DrawText(845,70,"00");
      esat::DrawText(875,70,score2);
    }else if((Player+1)->points<100000){
      esat::DrawText(845,70,"0");
      esat::DrawText(860,70,score2);
    }else{
      esat::DrawText(845,70,score2);
    }
  }else{
    esat::DrawText(845,70,"000000");
  }

	esat::DrawSetFillColor(255,255,255);

	itoa(Player->lives,live1,10);

	esat::DrawText(250,35,live1);


	if(multiplayer){
		itoa((Player+1)->lives,live2,10);
		esat::DrawText(700,35,live2);
	}

	free(score1);
	free(live1);
	if(multiplayer){
    free(score2);
		free(live2);
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
  if(turn%2==0){
     SpriteEnemyReserve(level_1);
     SpriteShipLevel(ex_level_1);

  }else{
    SpriteShipLevel(ex_level_2);
    SpriteEnemyReserve(level_2);
  }
	  Initiate();

  UfoTime = esat::Time()+1000;


  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
	last_time = esat::Time();

  if(!game_start)
    Menu();



	esat::DrawBegin();
    esat::DrawClear(0,0,0);

	if(game_start && !nextlevel){
    EnemiesDead();
		if(turn%2==0){

      EnemiesSpawn(level_1);

    }else{
      EnemiesSpawn(level_2);
    }
		PlayerSprites(player);

    if (!player -> dead){
  		Player1Control(player,esat::kSpecialKey_Left,esat::kSpecialKey_Right);
  		Fly(player,esat::kSpecialKey_Up);
    }
      PlayerDead(player);
    	Shot(esat::kSpecialKey_Space);
    	ShotsMovement();

      if(turn%2==0){
        if (ex_level_1 == 0 || ex_level_1%4 ==0)
          Pieces(&rocket[1],&rocket[2]);
      }else{
        if (ex_level_2 == 0 || ex_level_2%4 ==0)
          Pieces(&rocket[1],&rocket[2]);
      }
      EnemiesShoting();

      EnemiesBehavior();

    	EnemiesMovement();

    	EnemiesLimits();


		if(time_%250==0){
			ItemSpawn();

		}

		UpdateFrame();
		GameOver(player);
    Interface(player);
    ++time_;
	}

  ToNextLevel(player);

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
