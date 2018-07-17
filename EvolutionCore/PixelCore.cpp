// PixelCore.cpp
//

#include "stdafx.h"
#include "gridPoint.h"
#include "EvolutionModelData.h"
#include "pixelCoordinates.h"
#include "PixelCore.h"

PixelCore::PixelCore
( 
    EvolutionModelData * pModel,
	PixelCoordinates   * pPixelCoordinates
)
{
	m_pModelWork        = pModel;
	m_pPixelCoordinates = pPixelCoordinates;
}

PixelCore::~PixelCore( )
{
	m_pModelWork        = nullptr;
	m_pPixelCoordinates = nullptr;
}

void PixelCore::SetSelection( PixelPoint const & pt1, PixelPoint const & pt2 )
{
    PixelRect const rectSelection( pt1, pt2 );	   // Current selection
    GridRect        rect( m_pPixelCoordinates->Pixel2GridRect( rectSelection ) );
    rect.ClipToGrid( );
	m_pModelWork->SetSelection( rect );
}

PixelPoint PixelCore::GetPoiCenter() const
{
	GridPoint  const gpPoi       = m_pModelWork->FindPOI( );
    PixelPoint const pixPointPoi = m_pPixelCoordinates->Grid2PixelPosCenter( gpPoi );
	return pixPointPoi;
}

PixelPoint PixelCore::GetNewCenter( PixelPoint const pntCenter ) const
{
	return m_pModelWork->IsPoiDefined( ) 
	       ? GetPoiCenter( )
	       : pntCenter;
}

bool PixelCore::CenterPoi( PixelPoint const pntCenter ) const
{
	return m_pPixelCoordinates->CenterPoi
		   ( 
			  pntCenter, 
			  m_pModelWork->FindPOI( )
		   );
}

void PixelCore::FitToRect( PixelRectSize const rectSize ) const
{
	m_pPixelCoordinates->FitGridToRect
	( 
		m_pModelWork->GetSelection(),
		rectSize
	);
	m_pModelWork->ResetSelection( );
}

void PixelCore::SetFieldSize( short const sNewFieldSize, PixelPoint const pntCenter )
{
    (void)m_pPixelCoordinates->SetGridFieldSize
	( 
		sNewFieldSize, 
		GetNewCenter( pntCenter ) 
	);
}
