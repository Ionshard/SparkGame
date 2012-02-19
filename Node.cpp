#include "Node.h"

static int nextIndex = 0;

Node::Node()
{
	index = nextIndex++;
}
void Node::initialize(Point position, NodeType aType, int aTeam)
{
	p = position;
	setType(aType);
	team = aTeam;
	select = 0;
	if		(aType == BASIC)	{health = 50;}//NICK
	else if (aType == RELAY)	{health = 150;}//NICK
	else if (aType == SEEDER)	{health = 300;}//NICK
	upgrade = 0;
	//**************************
	//******HEALTH SYSTEM*******
	//**************************
	//seeders sap health. 
	//defenders heal health, upgrade nodes, takes 2 defenders and kills them
	//basic -> relay, relay -> seeder, Seeder is max
	//if health goes to zero, killing team gains node.
	//aggressors don't give a fart about health.
	//when a node dies, it gets downgraded. 
	//seeder-> relay, relay -> basic, basic stays basic
	//****All this stuff can go in the node menu
	

}
Point	Node::getPosition(){return p;}
Point	Node::getCentre(){return centre;}
int		Node::getIndex(){return index;}
void	Node::draw(){gpD3DSprite->Draw(gpTextures, &location, &centre, &p, teamColour[team+select]);}
Node&	Node::update(){return *this;}
void	Node::setSpawn(SparkType s){spawnType = s;}
void	Node::setPath(stack<Node*> aPath){path = aPath;}
stack<Node*> Node::getPath(){return path;}
int		Node::getTeam(){return team;}
void	Node::changeTeam(int t){ team = t;}


void	Node::spawn(vector<Spark>& sparks)
{
	if((type == SPAWNER) && !(path.empty()))
	{
			
			Spark theSpawn(this, spawnType);
			theSpawn.setPath(path);
			theSpawn.start();
			sparks.push_back(theSpawn);
			occupied(theSpawn);
	}

	//IF YOU DON'T HAVE A SET PATH, MAKE ONE
	if((type == SPAWNER) && (path.empty()) && (team != PLAYER))
	{
		AIpathset(sparks);
	}
}

void Node::setType(NodeType aType)
{
	type = aType;
	switch(type){
		case SPAWNER: spawnType = SEEDER; break;
		case RELAY:
		case BASIC: spawnType = NONE; break;
	}
	r = nodeValues[type].radius;
	location = nodeValues[type].location;
	centre = Point(r,r, 0);
}

NodeType Node::getType(){return type;}

vector<Node*> Node::getAdjList(){return adjacency_list;}
void Node::addAdj(Node* n){

	if (!contains(n)){
		adjacency_list.push_back(n);
		n->addAdj(this);
	}
	
}





//Handle the spark's collision
void Node::handleSpark(Spark * spark)  //NICK CHANGED THIS TO A POINTER
									//NOTE THIS
{
	//my teams Spark	
	if (isOccupied())
		handleNodeOccupancy();
	
	if(team == spark->getTeam())
	{	
		//NICK
		spark->start();

		//relays sparks
		if (((type == RELAY)||(type == SPAWNER))&& (team != PLAYER)){AIrelaypathset(spark);}//NICK	
		if (((type == RELAY)||(type == SPAWNER))&& (team == PLAYER)){spark->setPath(path);}
		if (spark->getType() == DEFENDER){
				if (type == BASIC){
					upgrade++;
					if (upgrade >= 2){
						this->setType(RELAY);
						upgrade = 0;
						fullHeal();
					}
					spark->stop();
					spark->kill();
				}
				else if (type == RELAY){
					upgrade++;
					if (upgrade >= 3){
						this->setType(SPAWNER);
						upgrade = 0;
						fullHeal();//NICK
					}
					spark->stop();
					spark->kill();
				}
				this->heal();
				
		}
	}
	//Other teams spark, aggressors don't care if on enemy node
	if((team != spark->getTeam()) && (spark->getType() != AGGRESSOR) && (spark->getType() != DEFENDER))
	{
		//HOW TO DEAL WITH BEING CAPTURED.
		//REQUIRES IMPROVMENT
		if (spark->getType() == SEEDER){
			spark->stop();
			if (initial == 0)
			{
				initial = GetTickCount();
			}			
			DWORD current = GetTickCount();
			DWORD difference = current - initial;
			if (health < 0)//NICK
			{
				if (type == SPAWNER) this->setType(RELAY);
				else if (type == RELAY) this->setType(BASIC);
				team = spark->getTeam();
				initial = 0;
				spark->start();
				fullHeal();
			}
			else health--;//NICK
	}
	}
	else
	{
		if(spark->finalDestination())
		{
			spark->stop();
			spark->kill();
		}
		else
		{
			spark->recalculatePosition();
		}
	}

	//Last Node
	
}

bool Node::contains (Node *n){
	vector<Node*>::iterator it;
	for ( it=adjacency_list.begin(); it < adjacency_list.end(); it++){
		if ( *it==n) {
			return true;
		}
	}

	return false;
}

void Node::selected (bool b){
///*	
	if (b){
		select = 3;//set this to the number of possible teams
	}
	else{
		select = 0;
	}
//*/
}
// MOUSE LOCATION CRITICAL
bool Node::contains(Point q)
{
	if (sqrt((p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x)) < r)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//THIS PROBABLY CHANGED, THE WHOLE THING.
//SPAWNER AI, SETS PATHS
void Node::AIpathset(vector<Spark>& sparks){
	
	/*Chooses a Spark at random (with a bias towards Seeders),	
	then Picks a path for it. pathfinding: seeders and Aggressors 
	go toward ENEMY nodes, defenders go toward ally nodes.
	if none of desired node, pick one at random. 
	next, repeat again.
	for third and forth nodes, pic completely at random where to go. 	
	*/


	//AM I A SPAWNER?
	//THAT COULD BE IMPORTANT.
	if((type == SPAWNER))
	{
		//NEUTRAL team
		if (team == NEUTRAL){

			//MOST BASIC FORM OF THE AI
			//PICKS 4 RANDOM ADJ NODES
			//SEND SPARKS TO THEM
			stack<Node*> s = stack<Node*>();
			this->setSpawn(AGGRESSOR);
			Node * node1;
			Node * node2;
			Node * node3;
			Node * node4;

			node1 = AIPathPicker(this,  spawnType);
			node2 = AIPathPicker(node1, spawnType);
			node3 = AIPathPicker(node2, spawnType);
			node4 = AIPathPicker(node3, spawnType);

			s.push(node4);
			s.push(node3);
			s.push(node2);
			s.push(node1);
			s.push(this);
				
			//Make the spark, give it a path, add to spark list
			Spark theSpawn(this, spawnType);
			theSpawn.setPath(s);
			theSpawn.start();
			sparks.push_back(theSpawn);
		
		path = s;
		return;
		

		}

		bool goCrazy = !(rand() % 7);
		this->setSpawn(SEEDER);
		//PATH FOR NODE
		stack<Node*> s = stack<Node*>();
		bool seedFound = false; 

		//is there an unlclaimed node near me? If there is I better go get it.
		if (!goCrazy){
		for (int i = adjacency_list.size()-1 ; i >= 0; i--){
				if (adjacency_list[i]->getTeam() != getTeam()){
					seedFound = true;
					break;
				}
			}
		}
		//there isn't? what to do...
		if ((!seedFound) || goCrazy){
			if (!(rand() % 3)) this->setSpawn(DEFENDER);
			else if (!(rand() % 2)) this->setSpawn(AGGRESSOR);
			else this->setSpawn(SEEDER);
		}

		Node * node1;
		Node * node2;
		Node * node3;
		Node * node4;
		
		node1 = AIPathPicker(this,  spawnType);
		node2 = AIPathPicker(node1, spawnType);
		
		if (spawnType != DEFENDER){
			
			node3 = AIPathPicker(node2, spawnType);
			
			if (spawnType == AGGRESSOR){
				node4 = AIPathPicker(node3, spawnType);
				s.push(node4);
			}
			if ((spawnType == SEEDER) || (spawnType == AGGRESSOR)){
				s.push(node3);
			}
		}
			s.push(node2);
			s.push(node1);
			s.push(this);
			
		//Make the spark, give it a path, add to spark list
		Spark theSpawn(this, spawnType);
		theSpawn.setPath(s);
		theSpawn.start();
		sparks.push_back(theSpawn);
		
		path = s;
	}
}



void Node::AIrelaypathset(Spark *aSpark){
	if (!path.empty()){ 
		aSpark->setPath(path);
		return;
	}
	if((type == RELAY))
	{	
		aSpark->stop();
		Node * node1;
		Node * node2;
		Node * node3;
		Node * node4;
		stack<Node*> s = stack<Node*>();

		node1 = AIPathPicker(this,  aSpark->getType());
		node2 = AIPathPicker(node1, aSpark->getType());
		
		if (aSpark->getType() != DEFENDER){
			
			node3 = AIPathPicker(node2, aSpark->getType());
			
			if (aSpark->getType() == AGGRESSOR){
				node4 = AIPathPicker(node3, aSpark->getType());
				s.push(node4);
			}
			if ((aSpark->getType() == SEEDER) || (aSpark->getType() == AGGRESSOR)){
				s.push(node3);
			}
		}
			s.push(node2);
			s.push(node1);
			s.push(this);
			
		aSpark->setPath(s);
		aSpark->start();
		path = s;
	}
}
//force node to generate new path by killing old one
void Node::emptyPath(){
  while (!path.empty())
  {
     path.pop();
  }
}

void Node::heal(){
	//heals node
	if		(type == BASIC)		{
		health = 50;
	}
	else if (type == RELAY)		{
		if (health < 150) 
				health=+50;
	}
	else if (type == SEEDER)	{
		if (health < 300) 
				health=+50;
	}
}

void Node::fullHeal(){
	//fully heals node
	if		(type == BASIC)		{health = 50;}
	else if (type == RELAY)		{health = 150;}
	else if (type == SEEDER)	{health = 300;}
}

//MENU POINTERS TO DATA
int* Node::getHealth(){return &health;}
int* Node::getUpgrade(){return &upgrade;}
NodeType* Node::getNodeType(){return &type;}
SparkType* Node::getSparkType(){return &spawnType;}
int* Node::getTeamPointer(){return &team;}

bool Node::isSelected (){
	if (select == 3) {return true;}
	else {return false;}
}


void Node::occupied(Spark&	aspark){ // Brendan New Method to Help Handle Collisions on the nodes themselves.
	occupied_list.push_back(&aspark);
}

void Node::abandoned(Spark & aspark){ // Brendan New Method to Help Handle COllisions on the nodes themselves
	int i = 0;
	for (unsigned int i = 0; i < occupied_list.size(); i++)
	{				
		if (occupied_list[i]->getIndex() == aspark.getIndex()){
			occupied_list.erase(occupied_list.begin()+i);
		}
	}
}

void Node::resetTimer(){
	initial = 0;
}

bool Node::isOccupied(){
	if (occupied_list.size() > 0)
		return true;
	else
		return false;
}

void Node::handleNodeOccupancy(){  //Method to compare nodes in an occupancy list BRENDAN
	
	for (unsigned int i = 0 ; i < occupied_list.size() ;i++)
	{
		for (unsigned int j = 0 ; j < occupied_list.size() ; j++)
		{
			if (isOccupied())
				occupied_list[i]->checkCollisionSpark(*occupied_list[j]);
		}
	}
}

Node* Node::AIPathPicker(Node *n, SparkType sT){
		
		int nodeNum = -1;
		Node* newNode;

		//a random node for newts
		if (n->adjacency_list.size() == 0) return n; //poor lonely node has noplace to go
		
		//neutrals bounce around thier spawner but won't venture far from home.
		//no ramming into the homespawner of us or them. 
		if (team == NEUTRAL) {
			if (this == n)
				return newNode = n->adjacency_list[rand() % ((int) n->adjacency_list.size())];
			else
				return this;
		}

		bool goCrazy = (!(rand() % 14)); // 1 in 14 of just going whereever the hell they want
										 //allows for getting out of situations where AI would always pick the same node
										 //number is high because it effects every single AI path selection
		

		//A DEFENDER, TRY AND HIDE AT HOME
		if (sT == DEFENDER){
			if (!goCrazy){
				for (int i = n->adjacency_list.size()-1 ; i >= 0; i--){
					if (n->adjacency_list[i]->getTeam() == getTeam()){
							nodeNum = i;
							if ((rand() % 2) && (n->adjacency_list[i] != n)) break;
					}
				}
			}
			if ((goCrazy) || nodeNum == -1){nodeNum = (rand() % n->adjacency_list.size());}
			
			newNode = n->adjacency_list[nodeNum];
		
			return newNode;
		}

		//AGG/SEED HEAD FOR ENEMY/NEWT TURF
		else if ((sT == AGGRESSOR) || (sT == SEEDER)){

			if (!goCrazy){	
				for (int i = 0; i < (int) n->adjacency_list.size(); i++){
						if (n->adjacency_list[i]->getTeam() != getTeam()){
							nodeNum = i;
							if ((rand() % 2) && n->adjacency_list[i] != n)  break;
							if (sT == AGGRESSOR) //aggressors prefer the enemy, seeders are content with neutral nodes
								if (((rand() % 2) || (n->adjacency_list[i]->getTeam() != NEUTRAL)) && n->adjacency_list[i] != n)  break;
						}
				}
			}
			if ((goCrazy) ||(nodeNum == -1)){
				nodeNum = (rand() % n->adjacency_list.size());
			}
			newNode = n->adjacency_list[nodeNum];
			return newNode;
		}
		else return n; 
}

