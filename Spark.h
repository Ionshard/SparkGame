#pragma once
#include "Spark_Game.h"
#include "Node.h"
#include <cmath>
using namespace std;
class Node;
class Spark
{
private:
	//Location and Movement Essential Items
	Point p;
	Point v;
	int team;
	float speed_factor, radius;
	float rotation;
	bool moving;
	bool alive;
	stack<Node*> path;
	Node* preNode;
	// IMAGE LOCATION 
	RECT location;
	Point centre;
	int index;
	int sparkle; 
	SparkType type; 
	int life;

public:

//Constructor
	Spark (Node* n, SparkType type);
	//SETS THE PATH OF THE SPARK
	void setPath(stack<Node*> apath);
	// RECALCULATES THE POSITION OF THE POINT OF THE VECTOR
	void recalculatePosition();
	Spark& update();
	bool finalDestination();
	void checkCollisionNode();
	bool checkCollisionSpark(Spark &s);
	void draw();
	int getTeam();
	void stop();
	void start();
	bool isDead();
	void kill();
	Point getPosition();
	SparkType getType();
	Node* destinationNode();
	Node* previousNode();
	int getIndex();
};