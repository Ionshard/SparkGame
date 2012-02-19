#pragma once
#include "Spark_Game.h"
#include "Node.h"

class NodeSprite
{
private:
	point p;
	float r;
	Node* node;
	RECT location;
	point centre;
public:
	void initialize(point position, float radius, Node* aNode)
	{
		p = position;
		r = radius;
		node = aNode;
		centre = point(0,0,0);
		SetRect(&location, 20, 0, 90, 70);
	}
	point getPosition(){return p;}
	void draw(){d3dspt->Draw(Textures, &location, &centre, &p, teamColour[node->getTeam()]);}
	void update(){}


};
	
