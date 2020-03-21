// Pipeline.cpp 
//
// NNetModel

#include "stdafx.h"
#include <cmath>
#include "assert.h"
#include "Geometry.h"
#include "PixelCoordsFp.h"
#include "tHighlightType.h"
#include "Direct2D.h"
#include "NNetParameters.h"
#include "NNetModel.h"
#include "Pipeline.h"

using std::fill;

MicroMeter const Pipeline::STD_ARROW_SIZE { 30.0_MicroMeter };

MicroMeter Pipeline::m_arrowSize { STD_ARROW_SIZE };

Pipeline::Pipeline( MicroMeterPoint const umUnused )
  :	Shape( ShapeType::Value::pipeline ),
	m_pKnotStart ( nullptr ),
	m_pKnotEnd   ( nullptr ),
	m_width      ( PIPELINE_WIDTH ),
	m_potential  ( ),
	m_potIter    ( )
{
}

void Pipeline::Clear( )
{
	Shape::Clear( );
	LockShape();
	fill( m_potential.begin(), m_potential.end(), 0.0_mV );
	UnlockShape();
}

void Pipeline::Recalc( )
{
	if ( m_pKnotStart && m_pKnotEnd )
	{
		LockShape();
		meterPerSec  const pulseSpeed    { meterPerSec( m_pNNetModel->GetParameterValue( tParameter::pulseSpeed ) ) };
		MicroMeter   const segmentLength { CoveredDistance( pulseSpeed, m_pNNetModel->GetTimeResolution( ) ) };
		MicroMeter   const pipelineLength{ Distance( m_pKnotStart->GetPosition(), m_pKnotEnd->GetPosition() ) };
		unsigned int const iNrOfSegments { max( 1, CastToUnsignedInt(round(pipelineLength / segmentLength)) ) };
		m_potential.resize( iNrOfSegments, BASE_POTENTIAL );
		m_potIter = m_potential.begin();
		UnlockShape();
	}
}

void Pipeline::SetStartKnot( BaseKnot * const pBaseKnot )
{
	m_pKnotStart = pBaseKnot;
	Recalc( );
}

void Pipeline::SetEndKnot( BaseKnot * const pBaseKnot )
{
	m_pKnotEnd = pBaseKnot;
	Recalc( );
}

void Pipeline::dislocate( BaseKnot * const pBaseKnot, MicroMeter const dislocation )
{ 
	MicroMeterPoint const umVector  { GetVector( ) };
	MicroMeterPoint const umNewPnt  { OrthoVector( umVector, dislocation ) };
	pBaseKnot->MoveShape( umNewPnt );
}

MicroMeterPoint Pipeline::GetStartPoint( ) const 
{ 
	return m_pKnotStart ? m_pKnotStart->GetPosition() : MicroMeterPoint::NULL_VAL(); 
}

MicroMeterPoint Pipeline::GetEndPoint( ) const 
{ 
	return m_pKnotEnd ? m_pKnotEnd->GetPosition() : MicroMeterPoint::NULL_VAL();
}

MicroMeter Pipeline::GetLength( ) const
{
	return Distance( GetStartPoint( ), GetEndPoint( ) );
}

bool Pipeline::IsPointInShape( MicroMeterPoint const & point ) const
{
	MicroMeterPoint const fDelta{ GetEndPoint( ) - GetStartPoint( ) };
	if ( IsCloseToZero( fDelta ) )
		return false;
	MicroMeterPoint const fOrthoScaled{ OrthoVector( fDelta, m_width ) };
	MicroMeterPoint const corner1     { GetStartPoint( ) + fOrthoScaled };
	MicroMeterPoint const corner2     { GetStartPoint( ) - fOrthoScaled };
	MicroMeterPoint const corner3     { GetEndPoint  ( ) + fOrthoScaled };
	return IsPointInRect< MicroMeterPoint >( point, corner1, corner2, corner3 );
}

MicroMeterPoint Pipeline::GetVector( ) const
{
	MicroMeterPoint const umStartPoint { GetStartPoint( ) };
	MicroMeterPoint const umEndPoint   { GetEndPoint  ( ) };
	MicroMeterPoint const umvector{ umEndPoint - umStartPoint };
	assert( ! IsCloseToZero( umvector ) );
	return umvector;
}

void Pipeline::DrawExterior( PixelCoordsFp & coord, tHighlightType const type ) const
{
	MicroMeterPoint const umStartPoint { GetStartPoint( ) };
	MicroMeterPoint const umEndPoint   { GetEndPoint  ( ) };
	if ( umStartPoint != umEndPoint )
	{
		fPIXEL       const fPixWidth  { coord.convert2fPixel( m_width ) };
		fPixelPoint  const fStartPoint{ coord.convert2fPixelPos( umStartPoint ) };
		fPixelPoint  const fEndPoint  { coord.convert2fPixelPos( umEndPoint   ) };
		D2D1::ColorF const colF       { GetFrameColor( type ) };

		m_pGraphics->DrawLine( fStartPoint, fEndPoint, fPixWidth, colF );

		if ( m_arrowSize > 0.0_MicroMeter )
			m_pGraphics->DrawArrow
			(
				coord.convert2fPixelPos( (umEndPoint * 2.f + umStartPoint) / 3.f ), 
				coord.convert2fPixelSize( umEndPoint - umStartPoint ), 
				colF, 
				coord.convert2fPixel( m_arrowSize ),
				fPixWidth / 2 
			);
	}
}

void Pipeline::drawSegment( fPixelPoint & fP1, fPixelPoint const fPixSegVec, fPIXEL const fWidth, mV const voltage ) const
{
	fPixelPoint  const fP2  { fP1 + fPixSegVec };
	D2D1::ColorF const colF { GetInteriorColor( voltage ) };
	m_pGraphics->DrawLine( fP1, fP2, fWidth, colF );
	fP1 = fP2;
}

void Pipeline::DrawInterior( PixelCoordsFp & coord )
{
	MicroMeterPoint const umStartPoint { GetStartPoint( ) };
	MicroMeterPoint const umEndPoint   { GetEndPoint  ( ) };
	MicroMeterPoint const umVector     { umEndPoint - umStartPoint };
	if ( ! IsCloseToZero( umVector ) )
	{
		fPIXEL      const fWidth     { coord.convert2fPixel( m_width * PIPELINE_INTERIOR ) };
		fPixelPoint const fPixVector { coord.convert2fPixelSize( umVector ) };
		fPixelPoint const fPixSegVec { fPixVector / CastToFloat(m_potential.size()) };
		fPixelPoint       fPoint1    { coord.convert2fPixelPos( umStartPoint ) };

		LockShape();
		for( auto iter = m_potIter; iter != m_potential.end(); ++ iter )
			drawSegment( fPoint1, fPixSegVec, fWidth, * iter );
		for( auto iter = m_potential.begin(); iter != m_potIter; ++ iter )
			drawSegment( fPoint1, fPixSegVec, fWidth, * iter );
		UnlockShape();
	}
}

Pipeline const * Cast2Pipeline( Shape const * pShape )
{
	assert( pShape->IsPipeline() );
	return static_cast<Pipeline const *>(pShape);
}

Pipeline * Cast2Pipeline( Shape * pShape )
{
	assert( pShape->IsPipeline() );
	return static_cast<Pipeline *>(pShape);
}
