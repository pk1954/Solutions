// D2D_DrawContext.cpp
//
// NNetWindows

#include "stdafx.h"
#include "d2d1helper.h"
#include "D2D_DrawContext.h"

void D2D_DrawContext::Start( HWND const hwnd )
{
	DrawContext::Initialize( );
	m_graphics.Initialize( hwnd );
	SetStdFontSize( STD_FONT_SIZE );
	m_scale.Initialize( & m_graphics, L"m" );
}

void D2D_DrawContext::Stop( )
{
	m_graphics.ShutDown( );
}

void D2D_DrawContext::Resize( int const width, int const height )
{
	m_graphics.Resize( width, height );
	m_scale.SetClientRectSize( PIXEL(width), PIXEL(height) );
}

void D2D_DrawContext::SetStdFontSize( MicroMeter const & size )
{
	m_graphics.SetStdFontSize( m_coord.Transform2fPixel( size ).GetValue() );
}

void D2D_DrawContext::DrawLine
( 
	MicroMeterPoint const & umStartPoint, 
	MicroMeterPoint const & umEndPoint, 
	MicroMeter      const   umWidth, 
	D2D1::ColorF    const   col
) const
{
	if ( umStartPoint != umEndPoint )
	{
		m_graphics.DrawLine
		( 
			m_coord.Transform2fPixelPos( umStartPoint ),
			m_coord.Transform2fPixelPos( umEndPoint   ),
			m_coord.Transform2fPixel   ( umWidth      ),
			m_bNoColors ? D2D1::ColorF::Black : col 
		);
	}
}

void D2D_DrawContext::FillCircle
(
	MicroMeterCircle const & umCircle,
	D2D1::ColorF     const   col  
) const
{
	m_graphics.FillCircle
	( 
		m_coord.Transform2fPixelCircle( umCircle ), 
		m_bNoColors ? D2D1::ColorF::Black : col 
	);
}

void D2D_DrawContext::FillGradientCircle
(
	MicroMeterCircle const & umCircle,
	D2D1::ColorF     const   col1,  
	D2D1::ColorF     const   col2  
) const
{
	m_graphics.FillGradientCircle( m_coord.Transform2fPixelCircle( umCircle ), col1, col2 );
}

void D2D_DrawContext::DrawCircle
(
	MicroMeterCircle const & umCircle,
	D2D1::ColorF     const   col,
	MicroMeter       const   umWidth
) const
{
	m_graphics.DrawCircle
	( 
		m_coord.Transform2fPixelCircle( umCircle ), 
		m_bNoColors ? D2D1::ColorF::Black : col,
		m_coord.Transform2fPixel( umWidth )
	);
}

void D2D_DrawContext::FillEllipse
(
	MicroMeterEllipse const & umEllipse,
	D2D1::ColorF      const   col  
) const
{
	m_graphics.FillEllipse
	( 
		m_coord.Transform2fPixelEllipse( umEllipse ), 
		m_bNoColors ? D2D1::ColorF::Black : col 
	);
}

void D2D_DrawContext::DrawEllipse
(
	MicroMeterEllipse const & umEllipse,
	D2D1::ColorF     const   col,
	MicroMeter       const   umWidth
) const
{
	m_graphics.DrawEllipse
	( 
		m_coord.Transform2fPixelEllipse( umEllipse ), 
		m_bNoColors ? D2D1::ColorF::Black : col,
		m_coord.Transform2fPixel( umWidth )
	);
}

void D2D_DrawContext::FillArrow
( 
	MicroMeterPoint const & umPos, 
	MicroMeterPoint const & umVector, 
	MicroMeter      const   umSize, 
	MicroMeter      const   umWidth, 
	D2D1::ColorF    const   col
) const
{
	m_graphics.FillArrow
	( 
		m_coord.Transform2fPixelPos ( umPos ),
		m_coord.Transform2fPixelSize( umVector ), 
		m_coord.Transform2fPixel    ( umSize ),
		m_coord.Transform2fPixel    ( umWidth ),
		m_bNoColors ? D2D1::ColorF::Black : col 
	);
}

void D2D_DrawContext::FillRectangle( MicroMeterRect const & umRect, D2D1::ColorF col ) const 
{
	m_graphics.FillRectangle( m_coord.Transform2fPixelRect( umRect ), col );
}

void D2D_DrawContext::DrawTranspRect( MicroMeterRect const & umRect, D2D1::ColorF col ) const 
{
	if ( IsTooSmall( umRect ) )
	{
		m_graphics.FillRectangle
		( 
			fPixelRect
			( 
				fPP_ZERO,
				m_coord.Transform2fPixelPos(umRect.GetEndPoint()) 
			), 
			col 
		);
		m_graphics.FillRectangle
		( 
			fPixelRect
			( 
				m_coord.Transform2fPixelPos(umRect.GetStartPoint()), 
				m_graphics.GetClRectSize() 
			), 
			col 
		);
	}
	else
	{
		m_graphics.FillRectangle( m_coord.Transform2fPixelRect(umRect), col );
	}
}

void D2D_DrawContext::SetPixelSize( MicroMeter const s ) 
{
	DrawContext::SetPixelSize( s );
	m_scale.SetHorzPixelSize( s.GetValue() );
}

void D2D_DrawContext::ShowScale( PixelRectSize const & pixRectSize ) const 
{
	m_scale.DisplayStaticScale( );
}

void D2D_DrawContext::DisplayText
(
	MicroMeterRect      const & umRect,
	std::wstring        const & wstr,
	D2D1::ColorF        const   colF,
	IDWriteTextFormat * const   pTextFormat
) const
{
	m_graphics.DisplayText( m_coord.Transform2PixelRect( umRect ), wstr, colF, pTextFormat );
}