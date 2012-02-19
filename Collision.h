#pragma once
#include "spark_game.h"

class Collision{
private:
	Node* n1;
	Node* n2;
	int index1;
	int index2;
	vector<Spark> list;
public:
	Collision(Node* node1, Node* node2)
	{
		n1 = node1;
		n2 = node2;
		index1 = n1->getIndex();
		index2 = n2->getIndex();
	}
	void add(Spark& spark)
	{
		if((spark.destinationNode() == n1 || spark.destinationNode() == n2) &&
		   (spark.previousNode() == n1 || spark.previousNode() == n2))
		{
			list.push_back(spark);
		}
	}
	void handleCollisions()
	{

		for(int i = 0; i < list.size(); i++)
		{
			for(int j = 0; j < list.size(); j++)
			{
				list[i].checkCollisionSpark(list[j]);
			}
		}
	}
	void clear()
	{
		list.clear();
	}
};