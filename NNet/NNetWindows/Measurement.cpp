// Measurement.cpp
//
// NNetWindows

#include "stdafx.h"
#include <sstream> 
#include <iomanip>
#include "Direct2D.h"
#include "Measurement.h"

using std::wostringstream;
using std::setprecision;
using std::fixed;
using std::endl;

static D2D1::ColorF const COL_STRONG { 0.0f, 0.4f, 0.0f, 1.0f };
static D2D1::ColorF const COL_WEAK   { 0.0f, 1.0f, 0.0f, 0.2f };

void Measurement::Initialize( D2D_driver * const pGraphics )
{
	m_pGraphics   = pGraphics;
	m_pTextFormat = pGraphics->NewTextFormat( 12.f );
}

void Measurement::ResetLimits()
{
	if ( m_fPixClientWidth.IsNotNull() )
	{
		m_fPixLeftLimit  = m_fPixClientWidth * 0.1f;
		m_fPixRightLimit = m_fPixClientWidth * 0.9f;
	}
}

void Measurement::SetClientRectSize( PIXEL const width, PIXEL const height )
{
	m_fPixClientWidth  = Convert2fPixel( width );
	m_fPixClientHeight = Convert2fPixel( height );
	ResetLimits();
}

bool Measurement::Select( fPixel const fPix )
{
	if ( m_bActive )
	{
		m_bSelectedLeft  = IsClose2LeftLimit ( fPix );
		m_bSelectedRight = IsClose2RightLimit( fPix );
		if ( m_bSelectedLeft || m_bSelectedRight )
			return true;
	}
	return false;
}

void Measurement::MoveSelection( fPixel const fPix )
{
	if ( m_bActive )
	{
		if ( m_bSelectedLeft && (fPix < m_fPixRightLimit) )
			m_fPixLeftLimit = fPix;
		else if ( m_bSelectedRight && (fPix > m_fPixLeftLimit) )
			m_fPixRightLimit = fPix;
		m_bLimitsMoved = true;
	}
}

void Measurement::verticalLine( fPixel const fPixPosX ) const
{
	m_pGraphics->DrawLine
	( 
		fPixelPoint( fPixPosX, 0._fPixel ), 
		fPixelPoint( fPixPosX, m_fPixClientHeight ), 
		1._fPixel, 
		RGB( 0, 0, 0 )  
	);
}

void Measurement::emphasizedLineLeft( fPixel const fPixPosX ) const
{
	fPixelRect const rect
	{ 
		fPixPosX, 
		0._fPixel, 
		fPixPosX + GRADIENT_WIDTH, 
		m_fPixClientHeight 
	};

	m_pGraphics->FillGradientRect( rect, COL_STRONG, COL_WEAK   );
}

void Measurement::emphasizedLineRight( fPixel const fPixPosX ) const
{
	fPixelRect const rect
	{ 
		fPixPosX - GRADIENT_WIDTH, 
		0._fPixel, 
		fPixPosX, 
		m_fPixClientHeight 
	};

	m_pGraphics->FillGradientRect( rect, COL_WEAK, COL_STRONG );
}

void Measurement::measuringArea() const
{
	fPixelRect const rect
	{ 
		m_fPixLeftLimit + (m_bSelectedLeft ? GRADIENT_WIDTH : 0.0_fPixel), 
		0._fPixel, 
		m_fPixRightLimit - (m_bSelectedRight ? GRADIENT_WIDTH : 0.0_fPixel),
		m_fPixClientHeight 
	};
	m_pGraphics->FillRectangle( rect, COL_WEAK );
}

void Measurement::textArea( fMicroSecs const fMicroSecsPerPixel ) const
{
	static COLORREF const COLOR        { RGB( 0, 0, 0 ) };  // CLR_BLACK
	fPixel          const fPixDistance { m_fPixRightLimit - m_fPixLeftLimit };
	fMicroSecs      const usMeasured   { fMicroSecsPerPixel * fPixDistance.GetValue() };
	fHertz          const frequency    { Frequency( usMeasured) };

	PIXEL posRight { Convert2PIXEL(m_fPixRightLimit - GRADIENT_WIDTH) };
	PIXEL posTop   { Convert2PIXEL(GRADIENT_WIDTH) };

	PixelRect pixRect
	( 
		posRight - 55_PIXEL, // left
		posTop,              // top
		posRight,            // right
		posTop + 35_PIXEL    // bottom
	);

	wostringstream wBuffer;
	wstring        wstrTime;
	Format2wstring( usMeasured, wstrTime );
	wBuffer << wstrTime << endl;
	wBuffer << fixed << setprecision(2);
	wBuffer << frequency << L" Hz";

	static D2D1::ColorF const COLOR_TEXT     { D2D1::ColorF::Black }; 
	static D2D1::ColorF const COL_BACKGROUND { D2D1::ColorF::AntiqueWhite };

	m_pGraphics->FillRectangle( Convert2fPixelRect(pixRect), COL_BACKGROUND );
	m_pGraphics->DisplayText( pixRect, wBuffer.str(), COLOR_TEXT, m_pTextFormat );
}

bool Measurement::IsClose2LeftLimit ( fPixel const fPix ) const 
{ 
	return (m_fPixLeftLimit <= fPix) && (fPix <= m_fPixLeftLimit + GRADIENT_WIDTH);
}

bool Measurement::IsClose2RightLimit( fPixel const fPix ) const 
{ 
	return (m_fPixRightLimit - GRADIENT_WIDTH <= fPix) && (fPix <= m_fPixRightLimit);
}

void Measurement::DisplayDynamicScale( fMicroSecs const fMicroSecsPerPixel ) const
{
	if ( m_bActive )
	{
		emphasizedLineLeft( m_fPixLeftLimit );
		verticalLine( m_fPixLeftLimit );
		measuringArea();
		emphasizedLineRight( m_fPixRightLimit );
		verticalLine( m_fPixRightLimit );
		textArea( fMicroSecsPerPixel );
	}
}
