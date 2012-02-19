#pragma once
// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <stack>
#include "dinput.h"
#include <ctime>


// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

typedef D3DXVECTOR3 Point;
#define D3DFVF_MYVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

#define EDGE_REG 5
#define EDGE_XL 7

#define WINDOW_WIDTH  1024 //nick, moved for menu class
#define WINDOW_HEIGHT 768



// The vertex type
struct MyVertex {
	float x, y, z, rhw;
	DWORD color;
};
//Spark Properties for a look up table
struct SparkProperties {
	float speed_factor;
	float radius;
	RECT location;
};

//Node Properties for a look up table
struct NodeProperties{
	float radius;
	RECT location;
};


// global declarations
extern HWND ghWnd;
extern PDIRECT3D9 gpD3D;    // the pointer to our Direct3D interface
extern LPDIRECT3DDEVICE9 gpD3DDevice;    // the pointer to the device class
extern LPD3DXSPRITE gpD3DSprite;    // the pointer to our Direct3D Sprite interface
extern LPD3DXFONT gpDXFont;    // the Pointer to the font object
extern LPDIRECT3DTEXTURE9 gpTextures;    // the pointer to the texture
extern LPDIRECT3DTEXTURE9 gpMenuTextures;
extern LPDIRECT3DVERTEXBUFFER9 gpVertexBuffer;	//Holds the edges
extern LPDIRECTINPUTDEVICE8 dinMouse;

enum SparkType {SEEDER, DEFENDER, AGGRESSOR, NONE};
enum NodeType {SPAWNER, RELAY, BASIC};
enum Teams {TEAM1, TEAM2, TEAM3};
//Look Up Tables
const SparkProperties sparkValues[] = {
	{3.5, 23, {150,0,200,45}},	    //Seeder Properties
	{1.5, 25, {150,100,200,150}},	//Defender Properties
	{5.5, 35, {148,178,218,248}}	//Agressor Properties
};

const NodeProperties nodeValues[] = {
	{37, {3,2,76,74}},	//Spawner Properties
	{33, {0,100,65,165}},	//Relay Properties
	{27, {0,200,60,255}}	//Basic Properties
};

const DWORD teamColour[] = {
	D3DCOLOR_ARGB(225, 0, 0, 225), //Main team colors
	D3DCOLOR_ARGB(225, 225, 0, 0), 
	D3DCOLOR_ARGB(225, 200, 50, 225),
	D3DCOLOR_ARGB(255, 100, 100, 255),  //lighter colours, for highlighting
	D3DCOLOR_ARGB(255, 255, 100, 100), 
	D3DCOLOR_ARGB(255, 225, 100, 255),
	D3DCOLOR_ARGB(175, 50, 50, 255), //darker for Spark effect
	D3DCOLOR_ARGB(175, 255, 50, 50), 
	D3DCOLOR_ARGB(175, 200, 125, 225),

};

//NICK
//NOTE THIS - IF YOU DO SOMETHING TEAM BASED USE THESE
//THAT WAY PLAYER CAN BE SET TO BLUE GREEN RED YELLOW WHATEVER
const int PLAYER			=	0;
const int ENEMY				=	1;
const int NEUTRAL			=	2;
const int score				=	0;

//CONSTANT MENU TERMS
const char healthstr[10]	=	"HEALTH:  "; 
const char upgradestr[14]	=	"UPGRADE IN:  "; 
const char teamstr[8]		=	"TEAM:  "; 
const char spawnstr[12]		=	"SPAWNING:  "; 
const char scorestr[9]		=	"SCORE:  ";
const char nS[8]			=	"SPAWNER";
const char nR[6]			=	"RELAY";
const char nB[6]			=	"BASIC";
const char sS[8]			=	"SEEDER";
const char sA[10]			=	"AGRESSOR";
const char sD[10]			=	"DEFENDER";
const char noSpawn[13]		=	"CAN'T SPAWN";
const char us[3]			=	"US";
const char them[5]			=	"THEM";
const char newt[10]			=	"UNCLAIMED";
const char maxedup[12]		=	"MAX UPGRADE";