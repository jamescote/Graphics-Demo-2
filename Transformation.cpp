#include "Transformation.h"


// Takes translation Matrix, and 3 angles in degrees for X, Y and Z
Transformation::Transformation( vec3 vTranslation )
{
	m_TranslationMatrix = translate( mat4( 1.f ), vTranslation );
	m_RotationMatrix = mat4( 1.f );

	m_pParent = NULL;
}

// Destructor
Transformation::~Transformation()
{

}

// Comparative Overload.
bool Transformation::operator== (Transformation &pRHS)
{
	bool bReturn = this->m_pParent == pRHS.m_pParent;							// Same Parent
	bReturn &= this->m_TranslationMatrix == pRHS.m_TranslationMatrix;			// Same Translation
	bReturn &= this->m_pChildren.size() == pRHS.m_pChildren.size();				// Same Number of Children

	bReturn &= this->m_RotationMatrix == pRHS.m_RotationMatrix;					// Same Rotation Matrices
		
	if ( bReturn )	// If  Same Number of Children
		for ( unsigned int i = 0; i < this->m_pChildren.size(); ++i )
			bReturn &= this->m_pChildren[i] == pRHS.m_pChildren[i];				// Same Children

	return bReturn;
}

// Get the Transformation
mat4 Transformation::getTransformationMatrix( bool bToWorld )
{
	// Apply This Transformation
	mat4 mReturnMatrix = m_RotationMatrix * m_TranslationMatrix;

	// Apply Parent Transformations if desired.
	if ( bToWorld && NULL != m_pParent )
		mReturnMatrix = m_pParent->getTransformationMatrix( bToWorld ) * mReturnMatrix;

	// Return resulting Transform
	return mReturnMatrix;
}

// Update Functions
// Further Rotate the current Rotation Transform.
void Transformation::updateRotation( const mat4 &mFurtherRotation )
{
	m_RotationMatrix = mFurtherRotation * m_RotationMatrix;
}

// given delta vector, modify the translation matrix for new translation
void Transformation::translateByDelta( vec3 vTranslation )
{
	mat4 pNewTranslate = translate( mat4( 1.f ), vTranslation );
	m_TranslationMatrix = pNewTranslate * m_TranslationMatrix;
}
