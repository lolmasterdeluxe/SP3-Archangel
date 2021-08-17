/**
 CCSVReader
 By: Ho Junliang
 Date: Aug 2021
 */
#include "CMapStorage.h"

vector<vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>>> CMapStorage::MapStorage;
vector<vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>>> CMapStorage::EntityStorage;

CMapStorage::CMapStorage()
{
	CCSVReader reader;
	string filename[1] = {
		"Maps/Map_Level_Test.csv"
	};
	MapStorage.clear();
	EntityStorage.clear();

	for (int lvl = 0; lvl < 1; lvl++)
	{
		vector<pair<string, float[5]>> values = reader.read_csv_map(filename[lvl]);

		vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>> MapInfo;
		vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>> EntityInfo;

		for (int i = 0; i < values.size(); i++)
		{
			pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]> info;
			if (values[i].first == "wall")
			{
				info.first = GameObject::GO_WALL;

				info.second[0] = Vector3(values[i].second[0], values[i].second[1]);
				info.second[1] = Vector3(values[i].second[2], values[i].second[3], 1);
				float deg = Math::DegreeToRadian(values[i].second[4]);
				info.second[2] = Vector3((float)cos(deg), (float)sin(deg));
				MapInfo.push_back(info);
			}
			else if (values[i].first == "player")
			{
				info.first = GameObject::GO_CUBE;

				info.second[0] = Vector3(values[i].second[0], values[i].second[1]);
				info.second[1] = Vector3(values[i].second[2], values[i].second[3], 1);
				float deg = Math::DegreeToRadian(values[i].second[4]);
				info.second[2] = Vector3((float)cos(deg), (float)sin(deg));
				EntityInfo.push_back(info);
			}
		}
		MapStorage.push_back(MapInfo);
		EntityStorage.push_back(EntityInfo);
	}
}

CMapStorage::~CMapStorage()
{
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

vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>> CMapStorage::GetMapInfo(int lvl)
{
	return MapStorage[lvl];
}
vector<pair<GameObject::GAMEOBJECT_TYPE, Vector3[3]>> CMapStorage::GetEntityInfo(int lvl)
{
	return EntityStorage[lvl];
}