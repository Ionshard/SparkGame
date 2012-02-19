/***********************************************************************/
/*
Author:	Doron Nussbaum, 

Purpose: This file is a sample code for initializing Direct3D
Contained Features:
a. Obtaining Direct3D object
b. Obtaining the Direct3D graphic device
c. Clearing up a window using Direct3D device 
d. Preparing the windows message loop for a gaming loop 
	d.1. changing from GetMessage to PeekMessage
	d.2. adding a timer
	d.2. adding a sleep function

*/

#include "Spark_Game.h"
#include "Node.h"
#include "Edge.h"
#include "Spark.h"
#include "Cursor.h"
#include "menu.h"
//#include "Collision.h"

Cursor* cursor;

// define the screen resolution and keyboard macros
#define WINDOWX 0
#define WINDOWY 0
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
const int TOTAL_NODES = 34;
int TOTAL_EDGES = 0;
POINT point;
Point pos;
Point relpos;
Point abspos;
bool selectingpath = false;
bool drawmenu = false;

// global declarations
HWND ghWnd;
PDIRECT3D9 gpD3D;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 gpD3DDevice;    // the pointer to the device class
LPD3DXSPRITE gpD3DSprite;    // the pointer to our Direct3D Sprite interface
LPD3DXFONT gpDXFont;    // the pointer to the font object
LPDIRECT3DTEXTURE9 gpTextures;    // the pointer to the texture
LPDIRECT3DTEXTURE9 gpMenuTextures;
LPDIRECT3DVERTEXBUFFER9 gpVertexBuffer;	//Holds the edges
DIMOUSESTATE mouseState;
LPDIRECTINPUTDEVICE8 dinMouse;
LPDIRECTINPUT8 Din;


//Global Objects
Node* nodes;				//Stores all the nodes used for the game
vector<Edge> permEdges;		//Stores all the edges used in the game
vector<Edge> selectedEdges;
vector<Edge> edges;
vector<Spark> sparks;		//Stores all the spark objects
vector<Node*> s;//Selection Nodes 
vector<Spark*> collisionHandler[TOTAL_NODES][TOTAL_NODES];
menu myMenu;

// function prototypes
//HWND InitApp(HINSTANCE hInstance, WNDPROC WndProc, LPCTSTR winClassName, LPCTSTR title, int nCmdShow);
HRESULT initD3D(HWND ghWnd, HINSTANCE hInstance);    // sets up and initializes Direct3D
HRESULT render();    // renders a single frame
void cleanD3D(void);    // closes Direct3D and releases memory
HRESULT loadResources();
void cleanResources();
void drawDisplay();
void LoadTexture(LPDIRECT3DTEXTURE9* texture, LPCTSTR filename);
void DrawTexture(LPDIRECT3DTEXTURE9 texture, RECT texcoords, float x, float y, int a);
void getInput();
void handleInput();
void gameLogic();
void toEdge(vector<Edge>&);
void toEdge(Node* n);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND ghWnd, UINT message, WPARAM wParam, LPARAM lParam);



// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"Spark_Window";

    RegisterClassEx(&wc);

    ghWnd = CreateWindowEx(NULL, L"Spark_Window", L"Spark",
                          WS_OVERLAPPEDWINDOW, WINDOWX, WINDOWY, WINDOW_WIDTH, WINDOW_HEIGHT,
                          NULL, NULL, hInstance, NULL);

    ShowWindow(ghWnd, nCmdShow);
	ShowCursor(false);

    // set up and initialize Direct3D
    if(FAILED(initD3D(ghWnd, hInstance)))
		return -1;

	MSG msg;
	srand((int)time(0));
   	DWORD Spawntime = GetTickCount();
	int AIchangepath = 0;
	
	// enter the main loop
    while(TRUE)
    {
        DWORD starting_point = GetTickCount();

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

		getInput();
		handleInput();
		gameLogic();
        if(FAILED(render()))
			return -1;
		SetCursorPos(WINDOWX+200, WINDOWY+200);
		
		//Stall to 25 fps
        while ((GetTickCount() - starting_point) < 25);
		
		//if there are a shitload of sparks let some die before spawning, 
		//otherwise every so many milliseconds spew one out.
		//get rid of spark spawn slowing if better collsion detection made. 
		if (((GetTickCount() - Spawntime) > 2750) /*&& (sparks.size() < TOTAL_NODES * 1.25)*/)
		{	
			for (int i = 0; i < TOTAL_NODES; i++){
				nodes[i].spawn(sparks);
				
			}
			 //get new random seed 
			srand((int)time(0));
			AIchangepath = (rand() % 3);
				if (!AIchangepath){
					for (int i = 0; i < TOTAL_NODES; i++){
						//DELETE THE TEAM THAT WON'T BE AI
						if ((nodes[i].getTeam() == ENEMY) ||
							//(nodes[i].getTeam() == PLAYER) ||
							(nodes[i].getTeam() == NEUTRAL)	)
						{
							nodes[i].emptyPath();
						}
					}
				}
			Spawntime = GetTickCount();
		}//spawn if	
    }

    // clean up DirectX and COM
    cleanD3D();

    return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND ghWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    return DefWindowProc (ghWnd, message, wParam, lParam);
}

void transfer(){
	Node *n;
	stack<Node*> path = stack<Node*>();
	while (!s.empty()){ // While Stack is not Empty pop the nodes one at a time onto the path stack
		path.push(s.back());
		if (s.back()->getType() == SPAWNER || s.back()->getType() == RELAY)//CHECKS FOR RELAYS AND SPAWNERS IN THE TRANSFER
		{								
			n = path.top();
			n->setPath(path);
			while (!path.empty())
				path.pop();
			path.push(s.back());
			s.pop_back();
		}
		else
		{
			s.pop_back();
		}	
	}
	
}

void getInput()
{
	//GET MOUSE INPUT
	memset(&mouseState, 0, sizeof(DIMOUSESTATE));
	if(!dinMouse) return;	//THIS IS BAD!
	
	bool done = false;
	HRESULT hr;
	while(!done)
	{
		hr = dinMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
		if(FAILED(hr))
		{
			if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			{
				hr = dinMouse->Acquire();
				if(FAILED(hr))
					return ;	//Couldn't Get the Mouse
			}
			else
				return;
		}
		else
			done = true;	
	}
	relpos.x =  (float) mouseState.lX;
	relpos.y =  (float) mouseState.lY;

	if((abspos.x + mouseState.lX) < WINDOW_WIDTH && (abspos.x + mouseState.lX) > 0)
		abspos.x += mouseState.lX;
	if((abspos.y + mouseState.lY) < WINDOW_HEIGHT && (abspos.y + mouseState.lY) > 0)
		abspos.y += mouseState.lY;

	pos = abspos;
	GetCursorPos(&point);
}

bool clickable(Node* n)
{
	return (n->getType() == SPAWNER || n->getType() == RELAY) && n->getTeam() == PLAYER;
}
void handleInput()
{
	// check the 'escape' key
    if(KEY_DOWN(VK_ESCAPE))
        PostMessage(ghWnd, WM_DESTROY, 0, 0);
	
	//if(KEY_DOWN(VK_RETURN))
    //    nodes[0].spawn(sparks);

	
	static int timer = 5;
	timer++;
	if(timer > 5)
	{
		//MOUSE INPUT HERE
		//RightClick
		if (mouseState.rgbButtons[1] & 0x80)
		{
			drawmenu = false;
			timer = 0;
			if(selectingpath)
			{
				for (int i = 0; i < TOTAL_NODES; i++)
				{	//CHECKS IF RIGHT CLICK WAS ON A RIGHT CLICKABLE NODE
					if (nodes[i].contains(pos) && clickable(nodes+i)) //If So Restart
					{
						nodes[i].selected(true);
						selectingpath = true;
						s.clear();
						selectedEdges.clear();
						s.push_back(nodes + i);
					
					}
					else 
					{	//RIGHT CLICK THAT IS NOT ON A CLICKABLE NODE
						selectingpath = false;	//If So Transfer
						nodes[i].selected(false);
						if (s.size() > 1)
						{
							transfer();
						}
						else
							s.clear();
						selectedEdges.clear();
					}		
				}
			}
			else
			{
				Point p;
				for (int i = 0; i < TOTAL_NODES; i++)
				{
					if (nodes[i].contains(pos)  && clickable(nodes+i))
					{
						selectedEdges.clear();
						nodes[i].selected(true);
						selectingpath = true;
						s.push_back(nodes + i);
					}
				}
			}
		}
		

		//Left Click
		else if (mouseState.rgbButtons[0] & 0x80 )
		{
			drawmenu = false;
			timer = 0;
			bool nohit = true;
			if(selectingpath)
			{
				for (int i = 0; i < TOTAL_NODES; i++)
				{
					if (nodes[i].contains(pos))
					{
						if (s.back()->contains(nodes + i)){
							s.push_back(nodes + i);	
							nodes[i].selected(true);
							nohit = false;
						}
					}
				}
				
				if(nohit)
				{
					transfer();
					selectingpath = false;
					selectedEdges.clear();
				}

			}
			else
			{
				Point p;
				for (int i = 0; i < TOTAL_NODES; i++)
				{
					if (nodes[i].contains(pos)){
							nodes[i].selected(true);
							toEdge(nodes+i);
							nohit = false;
							drawmenu = true;
					}else{
						nodes[i].selected(false);
					}
				}
				if(nohit)
					selectedEdges.clear();
			}
		}
	}
}

void gameLogic()
{

	static DWORD slowcoll = GetTickCount();	
	
	if ((GetTickCount() - slowcoll) > 25)
	{
	//Check to see if Nodes are being claimed.
	//If Not Reset Timer
		
	//CHECK	
	for (unsigned int i = 0; i < TOTAL_NODES ; i++){
		if (!nodes[i].isOccupied())
				nodes[i].resetTimer();
		}	
		


		if (!sparks.empty())
		{
			for(unsigned int i = 0; i < sparks.size(); i++)
			{
				if (sparks[i].previousNode() != NULL)
				{
					int x = sparks[i].destinationNode()->getIndex();
					int y = sparks[i].previousNode()->getIndex();
					collisionHandler[x][y].push_back(&sparks[i]);
					collisionHandler[y][x].push_back(&sparks[i]);
				}
			}
		}		
		for(unsigned int i = 0; i < permEdges.size(); i++)
		{
			int x = edges[i].n1->getIndex();
			int y = edges[i].n2->getIndex();
			if (collisionHandler[x][y].size()>1)
			{
				for (unsigned int j = 0 ; j < collisionHandler[x][y].size() ; j++)
				{
						for (unsigned int k = 0 ; k < collisionHandler[x][y].size(); k++)
						{
							
							collisionHandler[x][y].at(j)->checkCollisionSpark(*collisionHandler[x][y].at(k));		
						}
				}
			}
			if (!collisionHandler[x][y].empty())
				collisionHandler[x][y].clear();
		}
		slowcoll = GetTickCount();
	}
}
// this is the function used to render a single frame
HRESULT render(void)
{

	//Clear Background
    gpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    gpD3DDevice->BeginScene();    // begins the 3D scene

	//Get the vertex representation of the edges
	toEdge(selectedEdges);
	edges = permEdges;
	for(unsigned int i = 0; i < selectedEdges.size(); i++)
	{
		edges.push_back(selectedEdges.at(i));
	}

	gpVertexBuffer->Release();
	//Create a the vertex buffer to hold the edges
	if( FAILED( gpD3DDevice->CreateVertexBuffer( edges.size()*4 * sizeof( struct MyVertex ),
                                                  0, D3DFVF_MYVERTEX,
                                                  D3DPOOL_DEFAULT, &gpVertexBuffer, NULL ) ) )
    {
        return E_FAIL;
    }


	MyVertex* pVertices;
	if( FAILED( gpVertexBuffer->Lock(0, sizeof(MyVertex) * edges.size()*4, ( void** )&pVertices, 0 ) ) )
        return E_FAIL;
	Edge::toVertices(edges, pVertices);
    gpVertexBuffer->Unlock();

	//Draw the edges
	gpD3DDevice->SetStreamSource( 0, gpVertexBuffer, 0, sizeof(MyVertex) );
	gpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	gpD3DDevice->SetFVF(D3DFVF_MYVERTEX);
	gpD3DDevice->SetRenderState(D3DRS_COLORVERTEX, FALSE);
	for(unsigned int i = 0; i < edges.size(); i++)
	{
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i*4, 2);
	}

	//Draw Sprites
	gpD3DSprite->Begin(D3DXSPRITE_ALPHABLEND);

    drawDisplay();

    gpD3DSprite->End();

    gpD3DDevice->EndScene();    // ends the 3D scene
	
    gpD3DDevice->Present(NULL, NULL, NULL, NULL);

	return S_OK;

}


// this draws the display
void drawDisplay()
{

	for(int i = 0; i < TOTAL_NODES; i++)
	{
		if (nodes[i].isSelected()) 
		{
			myMenu.initialize(&nodes[i]);
		}
		nodes[i].update().draw();
	}

	vector<Spark>::iterator spark = sparks.begin();
	while(spark != sparks.end())
	{
		
		if(spark->isDead())
		{
			spark = sparks.erase(spark);
		}
		else
		{
			spark->update();
			spark->draw();
			spark++;
		}
	}

	if(drawmenu)
		myMenu.draw();
	cursor->Draw(pos);

		

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/* HOW TO DISPLAY TEXT
    // display the font
    SetRect(&Part, 535, 453, 630, 470);
    static char strAmmoText[10];
    _itoa_s(ammo, strAmmoText, 10);
    gpDXFont->DrawTextA(NULL,
                      (LPCSTR)&strAmmoText,
                      strlen((LPCSTR) &strAmmoText),
                      &Part,
                      DT_RIGHT,
                      D3DCOLOR_ARGB(255, 255, 255, 255));
                     // D3DCOLOR_ARGB(255, 120, 120, 255));
*/
}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// this draws a portion of the specified texture
void DrawTexture(LPDIRECT3DTEXTURE9 texture, RECT texcoords, float x, float y, int a)
{
    D3DXVECTOR3 center(0.0f, 0.0f, 0.0f), position(x, y, 0.0f);
    gpD3DSprite->Draw(texture, &texcoords, &center, &position, D3DCOLOR_ARGB(a, 255, 255, 255));

    return;
}

// this function initializes and prepares Direct3D for use
HRESULT initD3D(HWND ghWnd, HINSTANCE hInstance)
{
	
    gpD3D = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS gpD3Dpp;

    ZeroMemory(&gpD3Dpp, sizeof(gpD3Dpp));
    gpD3Dpp.Windowed = TRUE;
    gpD3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    gpD3Dpp.hDeviceWindow = ghWnd;
    gpD3Dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    gpD3Dpp.BackBufferWidth = WINDOW_WIDTH;
    gpD3Dpp.BackBufferHeight = WINDOW_HEIGHT;


    // create a device class using this information and the info from the gpD3Dpp stuct
    gpD3D->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      ghWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &gpD3Dpp,
                      &gpD3DDevice);

    D3DXCreateSprite(gpD3DDevice, &gpD3DSprite);    // create the Direct3D Sprite object


	//*****mouse
	
	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
	(void **) &Din, NULL);
	// get the mouse device
    
	Din->CreateDevice(GUID_SysMouse, // the default mouse
						&dinMouse, // the mouse object
						NULL); // com related
	// set the data format
	dinMouse->SetDataFormat(&c_dfDIMouse);
	// set the priviliges
	dinMouse->SetCooperativeLevel(ghWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	// acquire the mouse
	dinMouse->Acquire();

    if(FAILED(loadResources()))
		return E_FAIL;
	
	return S_OK;
}

//CREATES VECTOR OF SELECTED GLOWING EDGES
void toEdge(vector<Edge>& e){
	for (unsigned int i = 1 ; i < s.size(); i++){
		e.push_back(Edge(s[i-1], s[i],EDGE_XL,3));		
	}
}

void toEdge(Node* n)
{
	if(n->getTeam() != PLAYER)
		return;

	Node* preNode = NULL;
	Node* currNode = NULL;
	stack<Node*> path = n->getPath();
	while(!path.empty())
	{
		currNode = path.top();

		if(currNode->getType() == SPAWNER && currNode->getTeam() == PLAYER)//|| currNode->getType() == RELAY) Uncomment when relay's have path
			path = currNode->getPath();
		path.pop();

		if(preNode != NULL)
			selectedEdges.push_back(Edge(preNode, currNode, EDGE_XL, 3));
		preNode = currNode;
	}
}

// this loads the display graphics and font
HRESULT loadResources()
{
	LPDIRECT3DTEXTURE9 cursorTex;
    LoadTexture(&gpTextures, L"sparks.png");
	LoadTexture(&cursorTex, L"cursor.png");
	LoadTexture(&gpMenuTextures, L"window-concept-noback.png");
	cursor = new Cursor(cursorTex);

	nodes = new Node[TOTAL_NODES];
	
	
	// MANUAL CREATION OF THE NODES


	nodes[0].initialize(Point(462,384,0), BASIC, NEUTRAL);
	nodes[1].initialize(Point(562,384,0), BASIC, NEUTRAL);
	
	nodes[2].initialize(Point(362,284,0), BASIC, NEUTRAL);
	nodes[3].initialize(Point(362,484,0), BASIC, NEUTRAL);
	
	nodes[4].initialize(Point(662,284,0), BASIC, NEUTRAL);
	nodes[5].initialize(Point(662,484,0), BASIC, NEUTRAL);
	
	nodes[6].initialize(Point(262,384,0), SPAWNER, PLAYER);
	nodes[7].initialize(Point(762,384,0), SPAWNER, ENEMY);

	nodes[8].initialize(Point(062,384,0), BASIC, NEUTRAL);
	nodes[9].initialize(Point(162,284,0), BASIC, NEUTRAL);
	nodes[10].initialize(Point(162,484,0), BASIC, NEUTRAL);
	
	nodes[11].initialize(Point(962,384,0), BASIC, NEUTRAL);
	nodes[12].initialize(Point(862,284,0), BASIC, NEUTRAL);
	nodes[13].initialize(Point(862,484,0), BASIC, NEUTRAL);

	nodes[14].initialize(Point(262,184,0), BASIC, NEUTRAL);
	nodes[15].initialize(Point(262,584,0), BASIC, NEUTRAL);
	nodes[16].initialize(Point(762,184,0), BASIC, NEUTRAL);
	nodes[17].initialize(Point(762,584,0), BASIC, NEUTRAL);

	nodes[18].initialize(Point(062,184,0), BASIC, NEUTRAL);
	nodes[19].initialize(Point(162,84,0), BASIC, NEUTRAL);
	nodes[20].initialize(Point(362,84,0), BASIC, NEUTRAL);
	nodes[21].initialize(Point(462,184,0), BASIC, NEUTRAL);
	
	nodes[22].initialize(Point(562,184,0), BASIC, NEUTRAL);
	nodes[23].initialize(Point(662,84,0), BASIC, NEUTRAL);
	nodes[24].initialize(Point(862,84,0), BASIC, NEUTRAL);
	nodes[25].initialize(Point(962,184,0), BASIC, NEUTRAL);

	nodes[26].initialize(Point(062,584,0), BASIC, NEUTRAL);
	nodes[27].initialize(Point(162,684,0), BASIC, NEUTRAL);
	nodes[28].initialize(Point(362,684,0), BASIC, NEUTRAL);
	nodes[29].initialize(Point(462,584,0), BASIC, NEUTRAL);

	nodes[30].initialize(Point(562,584,0), BASIC, NEUTRAL);
	nodes[31].initialize(Point(662,684,0), BASIC, NEUTRAL);
	nodes[32].initialize(Point(862,684,0), BASIC, NEUTRAL);
	nodes[33].initialize(Point(962,584,0), BASIC, NEUTRAL);


	nodes[0].addAdj(&nodes[2]);
	nodes[2].addAdj(&nodes[3]);
	nodes[3].addAdj(&nodes[0]);
	
	nodes[1].addAdj(&nodes[4]);
	nodes[4].addAdj(&nodes[5]);
	nodes[5].addAdj(&nodes[1]);

	nodes[6].addAdj(&nodes[2]);
	nodes[6].addAdj(&nodes[3]);
	nodes[7].addAdj(&nodes[4]);
	nodes[7].addAdj(&nodes[5]);

	nodes[8].addAdj(&nodes[9]);
	nodes[8].addAdj(&nodes[10]);
	nodes[9].addAdj(&nodes[10]);	
	nodes[9].addAdj(&nodes[6]);
	nodes[10].addAdj(&nodes[6]);

	nodes[11].addAdj(&nodes[12]);
	nodes[11].addAdj(&nodes[13]);
	nodes[12].addAdj(&nodes[13]);	
	nodes[12].addAdj(&nodes[7]);
	nodes[13].addAdj(&nodes[7]);

	nodes[14].addAdj(&nodes[2]);
	nodes[14].addAdj(&nodes[6]);
	nodes[14].addAdj(&nodes[9]);
	
	nodes[15].addAdj(&nodes[3]);
	nodes[15].addAdj(&nodes[6]);
	nodes[15].addAdj(&nodes[10]);

	nodes[16].addAdj(&nodes[4]);
	nodes[16].addAdj(&nodes[7]);
	nodes[16].addAdj(&nodes[12]);
	
	nodes[17].addAdj(&nodes[5]);
	nodes[17].addAdj(&nodes[7]);
	nodes[17].addAdj(&nodes[13]);

	nodes[18].addAdj(&nodes[19]);
	nodes[18].addAdj(&nodes[9]);
	nodes[18].addAdj(&nodes[8]);

	nodes[19].addAdj(&nodes[14]);
	nodes[19].addAdj(&nodes[9]);

	nodes[20].addAdj(&nodes[14]);
	nodes[20].addAdj(&nodes[2]);
	
	nodes[21].addAdj(&nodes[20]);
	nodes[21].addAdj(&nodes[2]);
	nodes[21].addAdj(&nodes[0]);

	nodes[22].addAdj(&nodes[23]);
	nodes[22].addAdj(&nodes[4]);
	nodes[22].addAdj(&nodes[1]);

	nodes[23].addAdj(&nodes[4]);
	nodes[23].addAdj(&nodes[16]);

	nodes[24].addAdj(&nodes[16]);
	nodes[24].addAdj(&nodes[12]);
	
	nodes[25].addAdj(&nodes[24]);
	nodes[25].addAdj(&nodes[12]);
	nodes[25].addAdj(&nodes[11]);

	nodes[26].addAdj(&nodes[8]);
	nodes[26].addAdj(&nodes[10]);
	nodes[26].addAdj(&nodes[27]);

	nodes[27].addAdj(&nodes[10]);
	nodes[27].addAdj(&nodes[15]);

	nodes[28].addAdj(&nodes[15]);
	nodes[28].addAdj(&nodes[3]);
	
	nodes[29].addAdj(&nodes[0]);
	nodes[29].addAdj(&nodes[3]);
	nodes[29].addAdj(&nodes[28]);

	nodes[30].addAdj(&nodes[31]);
	nodes[30].addAdj(&nodes[5]);
	nodes[30].addAdj(&nodes[1]);

	nodes[31].addAdj(&nodes[17]);
	nodes[31].addAdj(&nodes[5]);

	nodes[32].addAdj(&nodes[17]);
	nodes[32].addAdj(&nodes[13]);
	
	nodes[33].addAdj(&nodes[32]);
	nodes[33].addAdj(&nodes[13]);
	nodes[33].addAdj(&nodes[11]);

	nodes[21].addAdj(&nodes[22]);
	nodes[29].addAdj(&nodes[30]);

	nodes[28].addAdj(&nodes[31]);
	nodes[20].addAdj(&nodes[23]);

	nodes[0].addAdj(&nodes[1]);



	
	
	//YE OLDE MAPE OLDE
	/*//point(x,y,z), topleft = (0,0,0) bottomright = (800,600,0)
	nodes[0].initialize(Point(050,050,0), SPAWNER, PLAYER);
	nodes[1].initialize(Point(050,550,0), RELAY, PLAYER);
	nodes[2].initialize(Point(125,300,0), BASIC, NEUTRAL);
	nodes[3].initialize(Point(400,100,0), BASIC, NEUTRAL);
	nodes[4].initialize(Point(750,550,0), SPAWNER, ENEMY);
	nodes[5].initialize(Point(750,050,0), RELAY, NEUTRAL);
	nodes[6].initialize(Point(675,300,0), BASIC, NEUTRAL);
	nodes[7].initialize(Point(400,500,0), BASIC, NEUTRAL);
	
	nodes[8].initialize(Point(200,150,0), BASIC, NEUTRAL);
	nodes[9].initialize(Point(600,150,0), BASIC, NEUTRAL);
	nodes[10].initialize(Point(200,450,0), BASIC, NEUTRAL);
	nodes[11].initialize(Point(600,450,0), BASIC, NEUTRAL);

	nodes[12].initialize(Point(400,300,0), RELAY, NEUTRAL);

	//Adjacency list MANUAL FILL
	nodes[1].addAdj(&nodes[2]);
	nodes[2].addAdj(&nodes[0]);
	nodes[5].addAdj(&nodes[6]);
	nodes[6].addAdj(&nodes[4]);

	nodes[0].addAdj(&nodes[3]);
	nodes[3].addAdj(&nodes[5]);
	nodes[4].addAdj(&nodes[7]);
	nodes[7].addAdj(&nodes[1]);

	nodes[2].addAdj(&nodes[12]);
	nodes[3].addAdj(&nodes[12]);
	nodes[6].addAdj(&nodes[12]);
	nodes[7].addAdj(&nodes[12]);

	nodes[8].addAdj(&nodes[12]);
	nodes[9].addAdj(&nodes[12]);
	nodes[10].addAdj(&nodes[12]);
	nodes[11].addAdj(&nodes[12]);

	nodes[0].addAdj(&nodes[8]);
	nodes[2].addAdj(&nodes[8]);
	nodes[3].addAdj(&nodes[8]);

	nodes[4].addAdj(&nodes[11]);
	nodes[6].addAdj(&nodes[11]);
	nodes[7].addAdj(&nodes[11]);

	nodes[5].addAdj(&nodes[9]);
	nodes[6].addAdj(&nodes[9]);
	nodes[3].addAdj(&nodes[9]);

	nodes[1].addAdj(&nodes[10]);
	nodes[2].addAdj(&nodes[10]);
	nodes[7].addAdj(&nodes[10]);*/

	D3DXCreateFont(gpD3DDevice, 20, 0, FW_BOLD, 1, false, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                   L"Times_New_Roman", &gpDXFont);

	Edge::populateEdges(permEdges, nodes, TOTAL_NODES);
	TOTAL_EDGES = permEdges.size();
	//Create a the vertex buffer to hold the intitial edges
	if( FAILED( gpD3DDevice->CreateVertexBuffer( TOTAL_EDGES*4 * sizeof( struct MyVertex ),
                                                  0, D3DFVF_MYVERTEX,
                                                  D3DPOOL_DEFAULT, &gpVertexBuffer, NULL ) ) )
    {
        return E_FAIL;
    }
	

	return S_OK;
}

// this loads a texture from a file
void LoadTexture(LPDIRECT3DTEXTURE9* texture, LPCTSTR filename)
{
    D3DXCreateTextureFromFileEx(gpD3DDevice, filename, D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, 
        D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), NULL, NULL, texture);

    return;
}

//Clean the resources
void cleanResources()
{
	gpTextures->Release();
	gpMenuTextures->Release();
	gpDXFont->Release();
	delete [] nodes;
	//delete sparks;
}
// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
    
    gpD3DDevice->Release();
    gpD3D->Release();

    return;
}