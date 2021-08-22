/**
 Map Storage
 By: Ho Junliang
 Date: Aug 2021
 */
#ifndef MAP_STORAGE_H
#define MAP_STORAGE_H

#define MAPS_PER_CATEGORY 1

#include <vector>

#include "Vector3.h"
#include "CSVReader.h"
#include "GameObject.h"
#include "MapDataStruct.h"

class CMapStorage
{
public:
	enum MAP_CATEGORY
	{
		CAT_TEST = 0,
		CAT_SPAWN,

		TOTAL_CATEGORY // must be last
	};

private:
	CMapStorage();
	// pointer to an instance
	static CMapStorage* s_mInstance;

	MapList* mapStorage[TOTAL_CATEGORY];

public:
	~CMapStorage();
	// Get the Singleton instance
	static CMapStorage* GetInstance();
	// Destroy this singleton instance
	static void Destroy();
	// Return mapStorage Data
	MapData* GetMapInfo(CMapStorage::MAP_CATEGORY category, int number);
};


#endif