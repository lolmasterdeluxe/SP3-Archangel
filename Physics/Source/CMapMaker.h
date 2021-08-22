/**
 CMapMaker
 By: Ho Junliang
 Date: Aug 2021
 */
#ifndef MAP_MAKER_H
#define MAP_MAKER_H

#include "CMapStorage.h"
#include "CMapNode.h"

// this class actually acts more like a manager than just a class that create map layouts
class CMapMaker
{
	CMapNode* current, * start, * newNode;
public:
	CMapMaker();
	~CMapMaker();

	void GenerateMap();
	void SaveMap();

	bool GoLeft();
	bool GoRight();

	const MapData* GetMapData();
	bool IsVisited();
};

#endif