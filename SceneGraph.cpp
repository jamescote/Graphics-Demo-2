#include "SceneGraph.h"

// 
SceneGraph::SceneGraph()
{
	m_pWorld = new Transformation( vec3( 0.f, 0.f, 0.f ) );
}

// Destructor
SceneGraph::~SceneGraph()
{
	if ( NULL != m_pWorld )
		delete m_pWorld;
}

/************************************************************************\
 * Graph Manipulation                                                   *
\************************************************************************/
void SceneGraph::addTransformation( Transformation* pNewTransform, Transformation* pParent )
{
	if ( NULL != pNewTransform )
	{
		// Set Parent
		if ( NULL == pParent )
			pNewTransform->m_pParent = m_pWorld;
		else
			pNewTransform->m_pParent = pParent;

		// Add new transform to Parent's Child List
		pNewTransform->m_pParent->m_pChildren.push_back( pNewTransform );
	}
}

// Delete a given Transformation
void SceneGraph::removeTransformation( Transformation* pRemovalPtr )
{
	Transformation* pParent;

	if ( NULL != pRemovalPtr && NULL != pRemovalPtr->m_pParent )  // Exists with a Parent
	{
		pParent = pRemovalPtr->m_pParent;
		pParent->m_pChildren.erase( remove( pParent->m_pChildren.begin(), pParent->m_pChildren.end(), pRemovalPtr ), pParent->m_pChildren.end() );
		delete pRemovalPtr;
		pRemovalPtr = NULL;
	}
	else if ( NULL != pRemovalPtr && pRemovalPtr != m_pWorld )  // Just delete it if there's no parent
	{
		delete pRemovalPtr;
		pRemovalPtr = NULL;
	}
}
