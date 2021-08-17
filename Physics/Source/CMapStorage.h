/**
 CCSVReader
 By: Ho Junliang
 Date: Aug 2021
 */
#ifndef MAP_STORAGE_H
#define MAP_STORAGE_H

#include <vector>

#include "Vector3.h"
#include "CSVReader.h"
#include "GameObject.h"


class CMapStorage
{
	CMapStorage();
	// pointer to an instance
	static CMapStorage* s_mInstance;

	static vector<vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>>> MapStorage;
	static vector<vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>>> EntityStorage;
public:
	~CMapStorage();
	// Get the Singleton instance
	static CMapStorage* GetInstance();
	// Destroy this singleton instance
	static void Destroy();

	vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>> GetMapInfo(int lvl);
	vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>> GetEntityInfo(int lvl);

};


#endif