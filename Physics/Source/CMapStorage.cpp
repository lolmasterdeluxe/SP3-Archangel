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
		/*,
		{"Maps/LeftDungeon/Map_Level_Left_Dungeon.csv"},
		{"Maps/LeftRest/Map_Level_Left_Rest.csv"},
		{"Maps/LeftTreasure/Map_Level_Left_Treasure.csv"},
		{"Maps/LeftBoss/Map_Level_Left_Boss.csv"},
		{"Maps/RightDungeon/Map_Level_Right_Dungeon.csv"},
		{"Maps/RightRest/Map_Level_Right_Rest.csv"},
		{"Maps/RightTreasure/Map_Level_Right_Treasure.csv"},
		{"Maps/RightBoss/Map_Level_Right_Boss.csv"}*/
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

				// Set Pos, Rot, Scale
				float deg = Math::DegreeToRadian(values[i].second[4]);
				info->pos = Vector3(values[i].second[0], values[i].second[1]);
				info->scale = Vector3(values[i].second[2], values[i].second[3], 1);
				info->rot = Vector3((float)cos(deg), (float)sin(deg));

				// Set type and store in appropriate list
				if (values[i].first == "wall")
				{
					info->type = GameObject::GO_WALL;
					wallInfo.push_back(info);
				}
				else if (values[i].first == "player")
				{
					info->type = GameObject::GO_CUBE;
					entityInfo.push_back(info);
				}
				else if (values[i].first == "potion")
				{
					info->type = GameObject::GO_POTION;
					entityInfo.push_back(info);
				}
				else if (values[i].first == "maxPotion")
				{
					info->type = GameObject::GO_MAXPOTION;
					entityInfo.push_back(info);
				}
				else if (values[i].first == "manaPotion")
				{
					info->type = GameObject::GO_MANAPOTION;
					entityInfo.push_back(info);
				}
				else if (values[i].first == "gold")
				{
					info->type = GameObject::GO_GOLD;
					entityInfo.push_back(info);
				}
				else if (values[i].first == "chest")
				{
					info->type = GameObject::GO_CHEST;
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