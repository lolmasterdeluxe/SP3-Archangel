/**
 CMapMaker
 By: Ho Junliang
 Date: Aug 2021
 */
#include "CMapMaker.h"

bool CMapMaker::CheckIfEnteringLeft()
{
	newNode = current;
	while (newNode != nullptr)
	{
		if (newNode == start) return false;
		newNode = newNode->GetLeft();
	}
	return true;
}

bool CMapMaker::CheckIfEnteringRight()
{
	newNode = current;
	while (newNode != nullptr)
	{
		if (newNode == start) return false;
		newNode = newNode->GetRight();
	}
	return true;
}

void CMapMaker::CreateLeft(CMapStorage::MAP_CATEGORY category)
{
	newNode = new CMapNode();
	newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(category, Math::RandIntMinMax(0, MAPS_PER_CATEGORY - 1)));
	current->SetLeft(newNode);
	newNode->SetRight(current);
	current = newNode;
}

void CMapMaker::CreateRight(CMapStorage::MAP_CATEGORY category)
{
	newNode = new CMapNode();
	newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(category, Math::RandIntMinMax(0, MAPS_PER_CATEGORY - 1)));
	current->SetRight(newNode);
	newNode->SetLeft(current);
	current = newNode;
}

void CMapMaker::DeleteMapNodes()
{
	if (start)
	{
		current = newNode = start->GetLeft();
		while (newNode != nullptr)
		{
			current = newNode;
			newNode = newNode->GetLeft();
			cout << "deleting node\n";
			if (current != NULL) delete current;
			current = nullptr;
			cout << "deleted node\n";
		}

		current = newNode = start->GetRight();
		while (newNode != nullptr)
		{
			current = newNode;
			newNode = newNode->GetRight();
			cout << "deleting node\n";
			if (current != NULL) delete current;
			current = nullptr;
			cout << "deleted node\n";
		}
		
		delete start;
	}
	start = current = newNode = nullptr;
}

CMapMaker::CMapMaker() :
	current(nullptr),
	start(nullptr),
	newNode(nullptr)
{
}

CMapMaker::~CMapMaker()
{
	DeleteMapNodes();
}


void CMapMaker::GenerateMap(int lvl)
{
	DeleteMapNodes();
	bool spawnedBossRoom = false;
	start = new CMapNode();
	//start->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_SPAWN, 0));
	start->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_SPAWN, Math::RandIntMinMax(0, MAPS_PER_CATEGORY - 1)));
	current = start;

	//newNode = new CMapNode();
	//newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_RIGHT_TREASURE, 0));
	//current->SetRight(newNode);
	//newNode->SetLeft(current);
	//
	//current = start;
	//newNode = new CMapNode();
	//newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_LEFT_TREASURE, 0));
	//current->SetLeft(newNode);
	//newNode->SetRight(current);
	for (unsigned int i = 0; i < Math::RandIntMinMax(1, 2); i++)
	{
		CreateLeft(CMapStorage::CAT_LEFT_DUNGEON);
	}
	if (Math::RandIntMinMax(1, 2) == 1)
	{
		CreateLeft(CMapStorage::CAT_LEFT_REST);
		CreateLeft(CMapStorage::CAT_LEFT_BOSS);
		spawnedBossRoom = true;
	}
	else
	{
		CreateLeft(CMapStorage::CAT_LEFT_TREASURE);
	}

	current = start;

	for (unsigned int i = 0; i < Math::RandIntMinMax(1, 2); i++)
	{
		CreateRight(CMapStorage::CAT_RIGHT_DUNGEON);
	}
	if (spawnedBossRoom)
	{
		CreateRight(CMapStorage::CAT_RIGHT_TREASURE);
	}
	else
	{
		CreateRight(CMapStorage::CAT_RIGHT_REST);
		CreateRight(CMapStorage::CAT_RIGHT_BOSS);
	}

	current = start;
}

bool CMapMaker::GoLeft()
{
	if (current->GetLeft() != nullptr)
	{
		current = current->GetLeft();
		current->SetEnterLocation(CheckIfEnteringLeft());
		return true;
	}
	return false;
}

bool CMapMaker::GoRight()
{
	if (current->GetRight() != nullptr)
	{
		current = current->GetRight();
		current->SetEnterLocation(CheckIfEnteringRight());
		return true;
	}
	return false;
}

const MapData* CMapMaker::GetMapData()
{
	return current->GetMapData();
}

void CMapMaker::SaveEntityData(vector<GOData*> entityDataList)
{
	current->SetEntityData(entityDataList);
}

bool CMapMaker::IsVisited()
{
	return current->IsVisited();
}

void CMapMaker::Visited()
{
	current->SetVisitStatus(true);
}

bool CMapMaker::IsFromFront()
{
	return current->Entering();
}
