#pragma once
#include "Spark_Game.h"
#include "Spark.h"

using namespace std;
class Spark;

extern vector<Spark> sparks;

class Node
{
private:
	int index;
	Point p;
	float r;
	RECT location;
	Point centre;
	
	vector <Node*> adjacency_list;
	vector <Spark*> occupied_list;
	stack <Node*> path;
	int team;
	NodeType type;
	SparkType spawnType;
	float spawnRate;
	int health;
	int upgrade;
	int select;
	DWORD initial;
public:
	Node();
	void initialize(Point position, NodeType aType, int aTeam = 2);
	Point getPosition();
	Point getCentre();
	int getIndex();
	void draw();
	void selected(bool b);
	Node& update();
	void spawn(vector<Spark>& sparks);
	void handleSpark(Spark * spark); //NICK CHANGED
	int getTeam();
	void changeTeam(int t);
	void setType(NodeType aType);
	void setPath(stack<Node*> aPath);
	stack<Node*> getPath();
	vector<Node*> getAdjList();
	void addAdj(Node* n);
	bool contains (Node *n);
	bool contains (Point p);
	NodeType getType();
	void setSpawn(SparkType s);
	void AIpathset(vector<Spark>& sparks);
	void AIrelaypathset(Spark *s);
	void emptyPath();
	void heal();
	void fullHeal();
	bool isSelected();
	//Brendan new Methods for Handling On-Node Spark Collisions
	void occupied(Spark &spark);
	void abandoned(Spark &spark);
	//menu pointers to node info
	int* getHealth();
	int* getUpgrade();
	NodeType* getNodeType();
	SparkType* getSparkType();
	int* getTeamPointer();
	bool isOccupied();
	void resetTimer();
	//Checks collisions on the node list
	void handleNodeOccupancy();
	Node* AIPathPicker(Node*n, SparkType sT); 
};
