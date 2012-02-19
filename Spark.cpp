#include "Spark.h"
//Constructor
static int nextIndex = 0;
Spark::Spark (Node* n, SparkType atype)
{
	index = nextIndex++;
	preNode = NULL;
	type = atype;
	path = stack<Node*>();
	p = n->getPosition();
	radius = sparkValues[type].radius;
	speed_factor = sparkValues[type].speed_factor;
	centre = Point(radius,radius,0);
	moving = false;
	team = n->getTeam();
	v = Point(0,0,0);
	//IMAGE LOCATION
	location = sparkValues[type].location;
	int sparkle = 0;
	if (atype == SEEDER) life = 3;
	if (atype == DEFENDER) life = 2;
	if (atype == AGGRESSOR) life = 4;
	
}

int			Spark::getTeam(){return team;}
void		Spark::stop(){moving = false;}
void		Spark::start(){moving = true;}
bool		Spark::finalDestination(){return !(path.size() > 1);}
bool		Spark::isDead(){return !alive;}
void		Spark::kill(){alive = false;}
Point		Spark::getPosition(){return p;}
SparkType	Spark::getType(){return type;}
Node*		Spark::previousNode(){return preNode;}
Node*		Spark::destinationNode(){return path.top();}
int			Spark::getIndex(){return index;}

//SETS THE PATH OF THE SPARK
void Spark::setPath(stack<Node*> apath)
{
	if (apath.empty()){
		delete this;
	}
	else{
		path = apath;
	}
}
// RECALCULATES THE POSITION OF THE POINT OF THE VECTOR
void Spark::recalculatePosition()
{
	float distance;
	preNode = path.top();
	preNode->abandoned(*this);
	if (path.size() > 1)
	{

		//Removes its self from from list
		
		path.pop();
		v = path.top()->getPosition() - preNode->getPosition();
		distance = sqrt((v.x * v.x) + (v.y * v.y));
		v = v / distance;
		life--;
		if (life == -1){alive = false;}
	}
}

Spark& Spark::update()
{
	if(moving)
		p += speed_factor * v;
		
	checkCollisionNode();
	
	return *this;
}


void Spark::checkCollisionNode(){
	float node_x = path.top()->getPosition().x;
	float node_y = path.top()->getPosition().y;
	if ((((p.y) - node_y) * ((p.y) - node_y) + ((p.x) - node_x) * ((p.x) - node_x)) < 100)
	{

		// SET VECTOR BACK TO (0,0)
		p = path.top()->getPosition();
		path.top()->occupied(*this);
		path.top()->handleSpark(this); //NICK CHANGED REMOVED * TO MAKE IT A POINTER
	}
	if ((path.top()->getType() == RELAY) || (path.top()->getType() == SPAWNER)){	
		if (type == SEEDER) life = 3;
		if (type == DEFENDER) life = 2;
		if (type == AGGRESSOR) life = 4;
	}

}

bool Spark::checkCollisionSpark(Spark& s){

		//don't bother if on same team

	if (s.getTeam() != this->getTeam()){
		
		// spark interaction rules
		if ( ((s.getType() == AGGRESSOR) && (type == SEEDER)) || 
			 ((s.getType() == SEEDER) && (type == DEFENDER)) ||
			 ((s.getType() == DEFENDER) && (type == AGGRESSOR))||
			 ((s.getType() == SEEDER) && (type == SEEDER)) ||
			 ((s.getType() == DEFENDER) && (type == DEFENDER)) ||
			 ((s.getType() == AGGRESSOR) && (type == AGGRESSOR)) )
		{
			float s_x = s.getPosition().x;
			float s_y = s.getPosition().y;
			// if I'm weak to something, check how close we are. 
			if ((((p.y) - s_y) * ((p.y) - s_y) + ((p.x) - s_x) * ((p.x) - s_x)) < 100)
			{
				//oh no, he hit me and I'm weak to him so I die
				this->stop();
				this->kill();
				return true;
			}
		}
	}
	return false;
}
void Spark::draw(){
	//occasionally flashes. 
	if (!(GetTickCount() % (rand() % 30 + 10))){sparkle = 6;} //darker
	else if (!(GetTickCount() % (rand() % 50 + 10))){sparkle = 3;} //brighter, also may cause sezures
	else {sparkle = 0;} //normal

	gpD3DSprite->Draw(gpTextures, &location, &centre, &p, teamColour[team+sparkle]);

}

