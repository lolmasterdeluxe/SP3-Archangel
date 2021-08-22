/**
 CMapMaker
 By: Ho Junliang
 Date: Aug 2021
 */
#ifndef MAP_MAKER_H
#define MAP_MAKER_H

#include "CMapStorage.h"
#include "CMapNode.h"

class CMapMaker
{
	CMapNode* current, * start, * newNode;
public:
	CMapMaker();
	~CMapMaker();

	void GenerateMap();

	bool GoLeft();
	bool GoRight();

	MapData* GetMapData();
};

#endif