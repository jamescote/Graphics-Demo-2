#pragma once
#include "stdafx.h"

// Contains a Transformation and Translation Matrix to Transform to World or Parent Space.
class Transformation
{
public:
	Transformation( vec3 vTranslation );
	~Transformation();

	// Equality operators
	bool operator== ( Transformation &pRHS);
	bool operator!= (Transformation &pRHS) { return !(*this == pRHS); }

	// Get the Transformation
	// bToWorld = if True, will create a Matrix from This Space to World Space.
	//			  if False, will create a Matrix from This Space to Parent Space.
	mat4 getTransformationMatrix( bool bToWorld );

	// Update Functions
	void updateRotation( const mat4 &mFurtherRotation );
	void translateByDelta( vec3 vTranslation );

private:	
	Transformation( Transformation* pCopy ); // Don't allow use of Copy Constructor

	mat4 m_TranslationMatrix;
	mat4 m_RotationMatrix;

	Transformation* m_pParent;
	vector<Transformation*> m_pChildren;

	friend class SceneGraph;
};

