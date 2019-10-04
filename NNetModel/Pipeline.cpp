// Pipeline.cpp 
//
// NNetModel

#include "stdafx.h"
#include "Geometry.h"
#include "Knot.h"
#include "PixelCoordsFp.h"
#include "win32_graphicsInterface.h"
#include "Pipeline.h"

MicroMeter Pipeline::distance( MicroMeterPoint const & npA, MicroMeterPoint const & npB )
{
	MicroMeterPoint npDiff   = npA - npB;
	MicroMeter      distance = MicroMeter
	(
		sqrt
		( 
			npDiff.GetXvalue() * npDiff.GetXvalue() + 
			npDiff.GetYvalue() * npDiff.GetYvalue() 
		)
	);
	return distance;
}

void Pipeline::initialize( )
{
	if ( m_pKnotStart && m_pKnotEnd )
	{
		MicroMeter   const segmentLength  = CoveredDistance( m_impulseSpeed, TIME_RESOLUTION );
		MicroMeter   const pipelineLength = distance( m_pKnotStart->GetPosition(), m_pKnotEnd->GetPosition() );
		unsigned int const iNrOfSegments  = CastToUnsignedInt(round(pipelineLength / segmentLength));

		m_potential.resize( iNrOfSegments, BASE_POTENTIAL );
	}
}

void Pipeline::SetStartKnot( Knot * pKnot )
{
	m_pKnotStart = pKnot;
	initialize();
}

void Pipeline::SetEndKnot( Knot * pKnot )
{
	m_pKnotEnd = pKnot;
	initialize();
}

MicroMeterPoint Pipeline::GetStartPoint( ) const 
{ 
	return m_pKnotStart ? m_pKnotStart->GetPosition() : MicroMeterPoint::NULL_VAL(); 
}

MicroMeterPoint Pipeline::GetEndPoint( ) const 
{ 
	return m_pKnotEnd ? m_pKnotEnd->GetPosition() : MicroMeterPoint::NULL_VAL();
}

MicroMeter Pipeline::GetWidth( ) const 
{ 
	return m_width; 
}

mV Pipeline::Step( mV const mVinput )
{
	mV mVcarry = mVinput;

	for ( vector<mV>::iterator iter = m_potential.begin( ); iter != m_potential.end( ); iter++ )
	{
		std::swap( * iter, mVcarry );
	}

	return mVcarry;
}

bool Pipeline::IsPointInShape( MicroMeterPoint const & point ) const
{
	MicroMeterPoint fDelta { GetEndPoint()  - GetStartPoint() };
	MicroMeterPoint fOrtho { fDelta.GetY(), - fDelta.GetX()   };

	double const dScaleFactor = m_width.GetValue() / sqrt( fOrtho.GetXvalue() * fOrtho.GetXvalue() + fOrtho.GetYvalue() * fOrtho.GetYvalue() );

	MicroMeterPoint fOrthoScaled = fOrtho * dScaleFactor;

	MicroMeterPoint const corner1 = GetStartPoint() + fOrthoScaled;
	MicroMeterPoint const corner2 = GetStartPoint() - fOrthoScaled;
	MicroMeterPoint const corner3 = GetEndPoint  () + fOrthoScaled;

	return IsPointInRect< MicroMeterPoint >( point, corner1, corner2, corner3 );
}

void Pipeline::Draw
( 
	GraphicsInterface   & Graphics,
	PixelCoordsFp const & coord
) const
{
	MicroMeter      const startOffset   = m_pKnotStart->GetExtension( ) * 0.8;
	MicroMeterPoint const startPoint    = GetStartPoint() + MicroMeterPoint( 0._MicroMeter, startOffset );

	MicroMeterPoint const vector        = GetEndPoint() - startPoint;
	MicroMeterPoint const segmentVector = vector / static_cast<double>(m_potential.size());
	fPIXEL          const fPixWidth     = coord.convert2fPixel( m_width ) ;
	fPixelPoint           fPixPoint1    = coord.convert2fPixelPos( startPoint );
	MicroMeterPoint       point2        = startPoint + segmentVector;

	for ( std::vector<mV>::const_iterator iter = m_potential.begin( ); iter != m_potential.end( ); iter++ )
	{
		assert( * iter < 200.0_mV );
		fPixelPoint     const fPixPoint2 = coord.convert2fPixelPos( point2 );
		int             const iLevel     = 255 - CastToInt( iter->GetValue() );
		assert( iLevel <= 255 );
		COLORREF        const color      = IsHighlighted( )	? RGB( iLevel, 0, iLevel ) : RGB( iLevel, 0, 0 );
		Graphics.AddfPixelLine( fPixPoint1, fPixPoint2, fPixWidth, color );
		point2    += segmentVector; 
		fPixPoint1 = fPixPoint2;
	}
}
