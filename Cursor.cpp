#include "Cursor.h"

Cursor::Cursor(LPDIRECT3DTEXTURE9 tex)
{
	texture = tex;
}
void Cursor::Draw(Point p)
{
	gpD3DSprite->Draw(texture, NULL, NULL, &p, D3DCOLOR_XRGB(255, 255, 255));
}