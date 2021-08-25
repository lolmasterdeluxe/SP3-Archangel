/**
 CMapNode
 By: Ho Junliang
 Date: Aug 2021
 */
#ifndef MAP_NODE_H
#define MAP_NODE_H

#include <vector>

#include "GameObject.h"
#include "MapDataStruct.h"

using namespace std;

class CMapNode
{
	MapData* mapData;
	bool visited, entering;
	CMapNode* left, * right;
public:
	CMapNode();
	~CMapNode();
	CMapNode* GetLeft();
	CMapNode* GetRight();
	void SetLeft(CMapNode* map);
	void SetRight(CMapNode* map);
	const MapData* GetMapData();
	void SetMapData(MapData* mapData);
	void SetEntityData(vector<GOData*> entityDataList);
	bool IsVisited();
	void SetVisitStatus(bool visited);
	bool Entering(); // check if it is going in from front or back
	void SetEnterLocation(bool entering); // true if it is going from front, false if it is going from back
};

#endif