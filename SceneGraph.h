#pragma once
#include "stdafx.h"
#include "Transformation.h"

// Manages Different Translations
class SceneGraph
{
public:
	SceneGraph();
	~SceneGraph();

	// Graph Manipulation
	void addTransformation( Transformation* pNewTransform, Transformation* pParent );
	void removeTransformation( Transformation* pRemovalPtr );

private:
	// This root can never be removed.
	Transformation* m_pWorld;
	
};

