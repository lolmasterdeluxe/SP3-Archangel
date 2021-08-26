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
	bool checkIfEnteringLeft();
	bool checkIfEnteringRight();
public:
	CMapMaker();
	~CMapMaker();
	
	// generate a map layout
	void GenerateMap();

	// try to go to the next room on the left
	bool GoLeft();
	// try to go to the next room on the right
	bool GoRight();

	// get struct that stores list of wall and entity data
	const MapData* GetMapData();
	// save GOData vector to current MapNode's entity List
	void SaveEntityData(vector<GOData*> entityDataList);
	// get if the room was visited before
	bool IsVisited();
	// set the room's visit Status to true
	void Visited();
	// get if the room is entered from the front(true) or the back(false)
	bool IsFromFront();
};

#endif