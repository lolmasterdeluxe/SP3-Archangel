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
	bool CheckIfEnteringLeft();
	bool CheckIfEnteringRight();

	void CreateLeft(CMapStorage::MAP_CATEGORY category);
	void CreateRight(CMapStorage::MAP_CATEGORY category);

	void DeleteMapNodes();
public:
	CMapMaker();
	~CMapMaker();
	
	/**
	@brief generates a (new) map layout and store it in map nodes
	@param lvl: To determine which boss to spawn at the end (0: demonlord, 1: metalgear, 2: 3rd realm boss)
	*/
	void GenerateMap(int lvl);

	/**
	@brief attempt to move to the next lvl on the left
	@return A bool True = moved to the next lvl on the left, False = remained on the same level (no lvl to the left)
	*/
	bool GoLeft();
	/**
	@brief attempt to move to the next lvl on the right
	@return A bool True = moved to the next lvl on the right, False = remained on the same level (no lvl to the right)
	*/
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