/**
 Map Storage
 By: Ho Junliang
 Date: Aug 2021
 */
#include "CMapStorage.h"

CMapStorage::CMapStorage()
{
	CCSVReader reader;
	string filename[TOTAL_CATEGORY][MAPS_PER_CATEGORY] = {
		{"Maps/Test/Map_Level_Test.csv"},
		{"Maps/Spawn/Map_Level_Spawn_1.csv"}
	};


	for (int category = 0; category < TOTAL_CATEGORY; category++)
	{
		MapList* mapList = new MapList();
		mapStorage[category] = mapList;
		for (int lvl = 0; lvl < MAPS_PER_CATEGORY; lvl++)
		{
			vector<pair<string, float[5]>> values = reader.read_csv_map(filename[category][lvl]);

			MapData* mapInfo = new MapData();
			vector<GOData*> wallInfo;
			vector<GOData*> entityInfo;

			for (int i = 0; i < values.size(); i++)
			{
				GOData* info = new GOData();
				if (values[i].first == "wall")
				{
					float deg = Math::DegreeToRadian(values[i].second[4]);
					float right = Math::DegreeToRadian(values[i].second[4] + 90);
					info->type = GameObject::GO_WALL;
					info->pos = Vector3(values[i].second[0], values[i].second[1]);
					info->scale = Vector3(values[i].second[2], values[i].second[3], 1);
					info->rot = Vector3((float)cos(deg), (float)sin(deg));
					wallInfo.push_back(info);
				}
				else if (values[i].first == "player")
				{
					float deg = Math::DegreeToRadian(values[i].second[4]);
					info->type = GameObject::GO_CUBE;
					info->pos = Vector3(values[i].second[0], values[i].second[1]);
					info->scale = Vector3(values[i].second[2], values[i].second[3], 1);
					info->rot = Vector3((float)cos(deg), (float)sin(deg));
					entityInfo.push_back(info);
				}
			}

			mapInfo->wallDataList = wallInfo;
			mapInfo->entityDataList = entityInfo;
			mapList->mapDataList.push_back(mapInfo);
		}
	}
}

CMapStorage::~CMapStorage()
{
	for (int i = 0; i < TOTAL_CATEGORY; i++)
	{
		delete mapStorage[i];
	}
}


CMapStorage* CMapStorage::s_mInstance = 0;
CMapStorage* CMapStorage::GetInstance()
{
	// If the instance does not exists...
	if (s_mInstance == nullptr)
	{
		// Create a new instance
		s_mInstance = new CMapStorage;
	}

	// Return the instance
	return s_mInstance;
}

void CMapStorage::Destroy()
{
	// If the instance exists...
	if (s_mInstance)
	{
		// Delete the singleton instance
		delete s_mInstance;
		// Set the handler to nullptr
		s_mInstance = nullptr;
	}
}

MapData* CMapStorage::GetMapInfo(CMapStorage::MAP_CATEGORY category, int number)
{
	if (number >= MAPS_PER_CATEGORY) number %= MAPS_PER_CATEGORY;
	return mapStorage[category]->mapDataList[number];
}