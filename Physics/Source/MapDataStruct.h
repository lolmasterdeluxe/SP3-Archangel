/**
 Map Data Structures
 By: Ho Junliang
 Date: Aug 2021
 */
#ifndef MAP_DATA_STRUCT_H
#define MAP_DATA_STRUCT_H

#include <vector>

#include "Vector3.h"
#include "GameObject.h"

using namespace std;

struct GOData
{
	GameObject::GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 scale;
	Vector3 rot;
	GOData() :
		type(GameObject::GO_WALL)
	{}
};

struct MapData
{
	vector<GOData*> wallDataList;
	vector<GOData*> entityDataList;
	~MapData()
	{
		for (vector<GOData*>::iterator it = wallDataList.begin(); it < wallDataList.end(); it++)
		{
			delete (*it);
			wallDataList.erase(it);
		}
		for (vector<GOData*>::iterator it = entityDataList.begin(); it < entityDataList.end(); it++)
		{
			delete (*it);
			entityDataList.erase(it);
		}
	}
};

struct MapList
{
	vector<MapData*> mapDataList;
	~MapList()
	{
		for (vector<MapData*>::iterator it = mapDataList.begin(); it < mapDataList.end(); it++)
		{
			delete (*it);
			mapDataList.erase(it);
		}
	}
};

#endif