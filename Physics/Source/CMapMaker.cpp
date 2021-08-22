/**
 CMapMaker
 By: Ho Junliang
 Date: Aug 2021
 */
#include "CMapMaker.h"

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
		delete start->GetLeft();
		delete start;
	}
}

void CMapMaker::GenerateMap()
{
	start = new CMapNode();
	start->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_SPAWN, 0));
	newNode = new CMapNode();
	newNode->SetMapData(CMapStorage::GetInstance()->GetMapInfo(CMapStorage::CAT_TEST, 0));
	start->SetLeft(newNode);
	newNode->SetRight(start);

	current = start;
}

bool CMapMaker::GoLeft()
{
	if (current->GetLeft() != nullptr)
	{
		current->SetVisitStatus(true);
		current = current->GetLeft();
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
