/**
 CMapNode
 By: Ho Junliang
 Date: Aug 2021
 */
#include "CMapNode.h"

CMapNode::CMapNode() :
	left(nullptr),
	right(nullptr),
	visited(false),
	entering(true)
{
}

CMapNode::~CMapNode()
{
	left = right = nullptr;
}

CMapNode* CMapNode::GetLeft()
{
	return left;
}

CMapNode* CMapNode::GetRight()
{
	return right;
}

void CMapNode::SetLeft(CMapNode* map)
{
	this->left = map;
}

void CMapNode::SetRight(CMapNode* map)
{
	this->right = map;
}

const MapData* CMapNode::GetMapData()
{
	return &mapData;
}

void CMapNode::SetMapData(MapData* mapData)
{
	this->mapData = *mapData;
}

void CMapNode::SetEntityData(vector<GOData*> entityDataList)
{
	this->mapData.entityDataList.clear();
	this->mapData.entityDataList = entityDataList;
}

bool CMapNode::IsVisited()
{
	return visited;
}

void CMapNode::SetVisitStatus(bool visited)
{
	this->visited = visited;
}

bool CMapNode::Entering()
{
	return entering;
}

void CMapNode::SetEnterLocation(bool entering)
{
	this->entering = entering;
}
