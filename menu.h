#include "Spark_Game.h"
#include "Node.h"

class menu
{

public:
bool drawMe;

private:
int *nodeHealth;
int *nodeUpgrade;
int *nodeTeam; //different from myTeam, you can select an enemy node ad view it's stuff
int myTeam;
bool upgraded;
SparkType *spawnType;
NodeType *nodeType;
RECT location;
Point centre;
Point p;

public:
void draw();
menu& update();
void initialize(Node *n);


};