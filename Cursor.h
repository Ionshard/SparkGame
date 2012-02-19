#pragma once
#include "Spark_Game.h"
using namespace std;

class Cursor
{
private:
	LPDIRECT3DTEXTURE9 texture;
public:
	Cursor(LPDIRECT3DTEXTURE9 tex);
	void Draw(Point p);
};