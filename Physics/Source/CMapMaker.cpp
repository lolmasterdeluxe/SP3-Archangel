/**
 CMapMaker
 By: Ho Junliang
 Date: Aug 2021
 */
#include "CMapMaker.h"

bool CMapMaker::checkIfEnteringLeft()
{
	newNode = current;
	while (newNode != nullptr)
	{
		if (newNode == start) return false;
		newNode = newNode->GetLeft();
	}
	return true;
}

bool CMapMaker::checkIfEnteringRight()
{
	newNode = current;
	while (newNode != nullptr)
	{
		if (newNode == start) return false;
		newNode = newNode->GetRight();
	}
	return true;
}

CMapMaker::CMapMaker() :
	current(nullptr),
	start(nullptr),
	newNode(nullptr)
{
}

CMapMaker::~CMapMaker()
{
	if (start)
	{
		current = newNode = start->GetLeft();
		while (newNode != nullptr)
		{
			current = newNode->GetLeft();
			delete current;
		}
		current = newNode = start->GetRight();
		while (newNode != nullptr)
		{
			current = newNode->GetRight();
			delete current;
		}
		delete start;
	}
}

void CMapMaker::GenerateMap()
{
	start = new CMapNode();
	start->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_SPAWN, 0));

	newNode = new CMapNode();
	newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_LEFT_DUNGEON, 0));
	start->SetLeft(newNode);
	newNode->SetRight(start);
	current = newNode;

	newNode = new CMapNode();
	newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_LEFT_REST, 0));
	current->SetLeft(newNode);
	newNode->SetRight(current);
	current = newNode;

	newNode = new CMapNode();
	newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_LEFT_BOSS, 0));
	current->SetLeft(newNode);
	newNode->SetRight(current);
	current = newNode;

	newNode = new CMapNode();
	newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_LEFT_TREASURE, 0));
	current->SetLeft(newNode);
	newNode->SetRight(current);
	current = newNode;
	
	newNode = new CMapNode();
	newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_TEST, 0));
	start->SetRight(newNode);
	newNode->SetLeft(start);

	current = start;
}

bool CMapMaker::GoLeft()
{
	if (current->GetLeft() != nullptr)
	{
		current->SetVisitStatus(true);
		current = current->GetLeft();
		current->SetEnterLocation(checkIfEnteringLeft());
		return true;
	}
	return false;
}

bool CMapMaker::GoRight()
{
	if (current->GetRight() != nullptr)
	{
		current->SetVisitStatus(true);
		current = current->GetRight();
		current->SetEnterLocation(checkIfEnteringRight());
		return true;
	}
	return false;
}

const MapData* CMapMaker::GetMapData()
{
	return current->GetMapData();
}

bool CMapMaker::IsVisited()
{
	return current->IsVisited();
}

bool CMapMaker::IsFromFront()
{
	return current->Entering();
}
