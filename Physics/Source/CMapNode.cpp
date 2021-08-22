/**
 CMapNode
 By: Ho Junliang
 Date: Aug 2021
 */
#include "CMapNode.h"

CMapNode::CMapNode() :
	left(nullptr),
	right(nullptr),
	mapData(nullptr),
	visited(false)
{
}

CMapNode::~CMapNode()
{
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
	return mapData;
}

void CMapNode::SetMapData(MapData* mapData)
{
	this->mapData = mapData;
}

bool CMapNode::IsVisited()
{
	return visited;
}

void CMapNode::SetVisitStatus(bool visited)
{
	this->visited = visited;
}
