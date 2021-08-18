#ifndef COLLISION_H
#define COLLISION_H

#include "GameObject.h"

struct Collision
{
	// The other object involved in the collision
	GameObject* go;
	// for physics relection of object
	Vector3 normal;
	
	// The axis in which we will push the object out
	Vector3 axis;
	// penetration distance
	float dist;

	Collision() : 
		go(nullptr),
		dist(0)
	{
	}
};

#endif