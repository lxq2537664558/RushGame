//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Cmd.h"
#include "TerrainMesh.h"
#include "Operator.h"

namespace bg
{
namespace sc
{

class CCmdMoveObject : public CCmd
{
	struct SDiffMoveInfo
	{		
		// two way real-time params
		Ptr<CTerrainMesh::CObject> pObject;
		
		// memento
		int nCenterGridIndex;
		std::vector<int> vecBlockGridIndices;
		SDiffMoveInfo() : nCenterGridIndex(INVALID), pObject(NULL) { }
	};

	SDiffMoveInfo m_diff;
	
public:
	
	CCmdMoveObject() 
	{
	}
	
	~CCmdMoveObject() 
	{
	}
	
	virtual void Execute();
	virtual void UnExecute();
	
	void SetObject(CTerrainMesh::CObject * p) 
	{
		m_diff.pObject = p;
	}

	void SetCenterGridIndex(int n) 
	{
		m_diff.nCenterGridIndex = n;
	}
	
	void AddBlockGridIndex(int n)
	{
		m_diff.vecBlockGridIndices.push_back(n);
	}
};

}
}