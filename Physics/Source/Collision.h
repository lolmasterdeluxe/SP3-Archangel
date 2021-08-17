#ifndef COLLISION_H
#define COLLISION_H

#include "GameObject.h"

struct Collision
{
	GameObject* go;
	Vector3 axis;
	float dist;
	Collision() : 
		go(nullptr),
		dist(0)
	{
	}
};

#endif