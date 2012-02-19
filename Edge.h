#pragma once
#include "Spark_Game.h"
#include "Node.h"

class Edge
{
public:
	float width;
	Point p1, p2;
	Node* n1;
	Node* n2;
	int colormod;
	Edge();
	Edge(Node* nodeNum1, Node* nodeNum2, float aWidth = 5, int colormod = 0);
	bool operator==(Edge &rhs);
	
	Point normalVector();
	static void populateEdges(vector<Edge>& edges, Node* nodes, int size);

	static bool contains (vector<Edge>& edges, Edge e);

	static void toVertices(vector<Edge>& edges, MyVertex* vertices);
	
};