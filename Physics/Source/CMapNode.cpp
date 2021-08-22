/**
 CMapNode
 By: Ho Junliang
 Date: Aug 2021
 */
#include "CMapNode.h"

CMapNode::CMapNode() :
	left(nullptr),
	right(nullptr),
	mapData(nullptr)
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
