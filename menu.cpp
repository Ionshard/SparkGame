#include "menu.h"

//PROBABLY CHANGED WHOLE THING
//

void menu::initialize(Node *n){

//pointers so you don't need to keep getting updates from node
//eg: node team changes, 
nodeHealth = n->getHealth();
nodeUpgrade = n->getUpgrade();
nodeTeam = n->getTeamPointer();
nodeType = n->getNodeType();
myTeam = PLAYER;
centre = Point(0,0,0);
RECT loc = {000,000,512,512};
location = loc;
p = n->getPosition();
spawnType = n->getSparkType();
//IMPLEMENT A SCORE SYSTEM 
//GIVE SYSTEM TO MENU
//OR SCORE WILL ALWAYS BE ZERO
//AND THAT IS SAD :(
upgraded = false;
drawMe = true;

p.z = 0;

if (p.y > WINDOW_HEIGHT - 205) {
	p.y = WINDOW_HEIGHT - 205;
}
if (p.x < 5) {
	p.x = 5;
}
if (p.x > WINDOW_WIDTH - 410) {
	p.x = WINDOW_WIDTH - 410;
}
if (p.y < 5) {
	p.y = 5;
}


}

menu& menu::update(){return *this;}

void menu::draw(){
	if (!drawMe) return;

	//IT LOOKS LIKE ALOT
	//BUT NOTE THE LACK OF LOOPS
	//A LOT OF THINGS NEED TO BE RESET BASED ON POSITION
	int sparkle = 0;
	//occasionally flashes.
	if (!(GetTickCount() % (rand() % 70 + 5))){sparkle = 6;}

	gpD3DSprite->Draw(gpMenuTextures, &location, &centre, &p, teamColour[*nodeTeam+sparkle]);

	//score
	RECT rec  = {(int) p.x + 40, (int) p.y + 40, (int) p.x + 200, (int) p.y + 150};
	RECT rec2 = {(int) p.x + 40, (int) p.y + 40, (int) p.x + 200, (int) p.y + 150};
	//node 
	RECT rec3 = {(int) p.x + 40, (int) p.y + 60, (int) p.x + 200, (int) p.y + 170};
	//spark
	RECT rec4 = {(int) p.x + 40, (int) p.y + 80, (int) p.x + 200, (int) p.y + 190};
	//health
	RECT rec5 = {(int) p.x + 40, (int) p.y + 100, (int) p.x + 200, (int) p.y + 210};
	RECT rec6 = {(int) p.x + 40, (int) p.y + 100, (int) p.x + 200, (int) p.y + 210};
	//upgrade
	RECT rec7 = {(int) p.x + 40, (int) p.y + 120, (int) p.x + 200, (int) p.y + 230};
	RECT rec8 = {(int) p.x + 40, (int) p.y + 120, (int) p.x + 200, (int) p.y + 230};
	//team
	RECT rec9 = {(int) p.x + 40, (int) p.y + 140, (int) p.x + 200, (int) p.y + 250};


    char healthNumstr[10];
	_itoa_s(*nodeHealth, healthNumstr, 10);

	char scoreNumstr[10];
	_itoa_s(score, scoreNumstr, 10);



   //score
	gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&scorestr, strlen((LPCSTR) &scorestr),
						&rec, DT_LEFT, teamColour[*nodeTeam+3]);
		
	gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&scoreNumstr, strlen((LPCSTR) &scoreNumstr),
					    &rec2, DT_RIGHT, teamColour[*nodeTeam+3]);

	//node type and spawn type
	if (*nodeType == SPAWNER){
			gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&nS, strlen((LPCSTR) &nS),
					    &rec3, DT_LEFT, teamColour[*nodeTeam+3]);
			
			if (*spawnType == SEEDER){
					gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&sS, strlen((LPCSTR) &sS),
					&rec4, DT_LEFT, teamColour[*nodeTeam+3]);
			}
			else if (*spawnType == DEFENDER){
					gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&sD, strlen((LPCSTR) &sD),
					&rec4, DT_LEFT, teamColour[*nodeTeam+3]);
			}
			else{
					gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&sA, strlen((LPCSTR) &sA),
					&rec4, DT_LEFT, teamColour[*nodeTeam+3]);
			}
	}
	else if (*nodeType == RELAY){
			gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&nR, strlen((LPCSTR) &nR),
					    &rec3, DT_LEFT, teamColour[*nodeTeam+3]);
			gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&noSpawn, strlen((LPCSTR) &noSpawn),
						&rec4, DT_LEFT, teamColour[*nodeTeam+3]);
	}
	else{
			gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&nB, strlen((LPCSTR) &nB),
					    &rec3, DT_LEFT, teamColour[*nodeTeam+3]);
			gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&noSpawn, strlen((LPCSTR) &noSpawn),
						&rec4, DT_LEFT, teamColour[*nodeTeam+3]);
	}


	//Health
	gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&healthstr, strlen((LPCSTR) &healthstr), 
					    &rec5, DT_LEFT, teamColour[*nodeTeam+3]);
	
	gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&healthNumstr, strlen((LPCSTR) &healthNumstr),
					  &rec6, DT_RIGHT, teamColour[*nodeTeam+3]);

	//upgrade

	char upgradeNumstr[10];
	

	if (*nodeType == SPAWNER){
		gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&maxedup, strlen((LPCSTR) &maxedup),
					  &rec7, DT_LEFT, teamColour[*nodeTeam+3]);
 
	}
	else if (*nodeType == RELAY){

		_itoa_s((3-*nodeUpgrade), upgradeNumstr, 10);

		gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&upgradestr, strlen((LPCSTR) &upgradestr),
					  &rec7, DT_LEFT, teamColour[*nodeTeam+3]);
		gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&upgradeNumstr, strlen((LPCSTR) &upgradeNumstr),
					  &rec8, DT_RIGHT, teamColour[*nodeTeam+3]);
	}
	else{ 

		_itoa_s((2-*nodeUpgrade), upgradeNumstr, 10);

		gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&upgradestr, strlen((LPCSTR) &upgradestr),
					  &rec7, DT_LEFT, teamColour[*nodeTeam+3]);
		gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&upgradeNumstr, strlen((LPCSTR) &upgradeNumstr),
					  &rec8, DT_RIGHT, teamColour[*nodeTeam+3]);
	}
	//team
	if (*nodeTeam == PLAYER){
		gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&us, strlen((LPCSTR) &us),
					  &rec9, DT_LEFT, teamColour[*nodeTeam+3]);
	}
	else if (*nodeTeam == ENEMY){
		gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&them, strlen((LPCSTR) &them),
					  &rec9, DT_LEFT, teamColour[*nodeTeam+3]);
	}
	else{
		gpDXFont->DrawTextA(gpD3DSprite, (LPCSTR)&newt, strlen((LPCSTR) &newt),
					  &rec9, DT_LEFT, teamColour[*nodeTeam+3]);
	}


}

