#include "Edge.h"

Edge::Edge(){}
Edge::Edge(Node* nodeNum1, Node* nodeNum2, float aWidth, int colormod)
{
	width = aWidth;
	n1 = nodeNum1;
	n2 = nodeNum2;

	p1 = nodeNum1->getPosition();
	p2 = nodeNum2->getPosition();

	this->colormod = colormod;
}
bool Edge::operator==(Edge &rhs){return ((p1 == rhs.p1 && p2 == rhs.p2) || (p1 == rhs.p2 && p2 == rhs.p1));}

//Static Functions
void Edge::populateEdges(vector<Edge>& edges, Node* nodes, int size)
{
	Edge temp;
	for(int i = 0; i < size; i++)
	{
		for(unsigned int j = 0; j < nodes[i].getAdjList().size(); j++)
		{
			temp = Edge((nodes+i), nodes[i].getAdjList().at(j));
			if(!contains(edges, temp))
				edges.push_back(temp);
		}
		 
	}
}

bool Edge::contains (vector<Edge>& edges, Edge e){
	vector<Edge>::iterator it;
	for ( it=edges.begin(); it < edges.end(); it++){
		if (e==*it) {
			return true;
		}
	}

	return false;
}

void Edge::toVertices(vector<Edge>& edges, MyVertex* vertices)
{
	int j = 0;
	float temp =0;
	Point normal;
	Edge e;

	for(unsigned int i = 0; i < edges.size(); i++)
	{
		e= edges.at(i);
		normal = e.normalVector();
		
		temp = normal.y * -1;
		normal.y = normal.x;
		normal.x = temp;
		normal = normal * (e.width/2);

		vertices[i*4].x = e.p1.x - normal.x;
		vertices[i*4].y = e.p1.y - normal.y;
		vertices[i*4].z = 1;
		vertices[i*4].rhw = 1;
		vertices[i*4].color = teamColour[e.n1->getTeam()+e.colormod];


		vertices[i*4+1].x = e.p2.x - normal.x;
		vertices[i*4+1].y = e.p2.y - normal.y;
		vertices[i*4+1].z = 1;
		vertices[i*4+1].rhw = 1;
		vertices[i*4+1].color = teamColour[e.n2->getTeam()+e.colormod];
		
		vertices[i*4+2].x = e.p1.x + normal.x;
		vertices[i*4+2].y = e.p1.y + normal.y; 
		vertices[i*4+2].z = 1;
		vertices[i*4+2].rhw = 1;
		vertices[i*4+2].color = teamColour[e.n1->getTeam()+e.colormod];

		vertices[i*4+3].x = e.p2.x +normal.x;
		vertices[i*4+3].y = e.p2.y +normal.y;
		vertices[i*4+3].z = 1;
		vertices[i*4+3].rhw = 1;
		vertices[i*4+3].color = teamColour[e.n2->getTeam()+e.colormod];
	}	
	
}

Point Edge::normalVector(){
	float distance;
	Point v;	
	v = p1 - p2;
	distance = sqrt((v.x * v.x) + (v.y * v.y));
	v = v / distance;
	return v;

}

		

