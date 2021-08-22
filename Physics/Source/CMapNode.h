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
	CMapNode* left, * right;
public:
	MapData* mapData;

	CMapNode();
	~CMapNode();
	CMapNode* GetLeft();
	CMapNode* GetRight();
	void SetLeft(CMapNode* map);
	void SetRight(CMapNode* map);
};

#endif