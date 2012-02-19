#pragma once
#include "Spark_Game.h"
#include "Node.h"


class EdgeSprite
{
private:
	point p1, p2;
	Node* n1, n2;
public
	EdgeSprite(Node node1, Node node2);
	void draw();
	static void populateEdges(vector<EdgeSprite>* edges);
	
};