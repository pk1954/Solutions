// PixelTypes.h
//
// Utilities

#pragma once

#include "util.h"
#include "NamedType.h"
#include "MoreTypes.h"
#include "PointType.h"
#include "RectType.h"

/////////////////// PIXEL ///////////////////////////////////

using PIXEL         = NamedType< long, struct PIXEL_Parameter >;
using PixelPoint    = PosType  < PIXEL >;
using PixelRectSize = SizeType < PIXEL >;
using PixelRect     = RectType < PIXEL >;

static PixelPoint const PP_NULL( PixelPoint::NULL_VAL() );   // compiler generates call!
static PixelPoint const PP_ZERO( PixelPoint::ZERO_VAL() );   // compiler generates call!

constexpr PIXEL operator"" _PIXEL( unsigned long long ull )
{
	return PIXEL( CastToShort( ull ) );
}

/////////////////// fPIXEL ///////////////////////////////////

using fPIXEL         = NamedType< float, struct fPIXEL_Parameter >;
using fPixelPoint    = PosType  < fPIXEL >;
using fPixelRectSize = SizeType < fPIXEL >;
using fPixelRect     = RectType < fPIXEL >;

static fPixelPoint const fPP_NULL( fPixelPoint::NULL_VAL() );   // compiler generates call!
static fPixelPoint const fPP_ZERO( fPixelPoint::ZERO_VAL() );   // compiler generates call!

constexpr fPIXEL operator"" _fPIXEL( long double ld )
{
	return fPIXEL( CastToFloat( ld ) );
}

inline static fPIXEL Hypot( fPixelPoint const pt ) 
{ 
	return fPIXEL( std::hypotf(pt.GetXvalue(), pt.GetYvalue() ) );
};

inline static fPixelPoint OrthoVector( fPixelPoint const & vect, fPIXEL const width )
{
	fPIXEL fHypot = Hypot( vect );
	assert( ! IsCloseToZero( fHypot.GetValue() ) );
	return fPixelPoint( vect.GetY(), - vect.GetX() ) * (width / fHypot);
}

inline bool IsCloseToZero( fPixelPoint const vect )
{
	return IsCloseToZero( vect.GetXvalue() ) && IsCloseToZero( vect.GetYvalue() );
}

/////////////////// conversions ///////////////////////////////////

static PIXEL convert2PIXEL( fPIXEL const fPixel )
{
	return PIXEL( CastToLong( fPixel.GetValue() ) );
}

static PixelPoint convert2PixelPoint( fPixelPoint const fPixPoint )
{
	return PixelPoint( convert2PIXEL( fPixPoint.GetX() ), convert2PIXEL( fPixPoint.GetY() ) );
}

static PixelRectSize convert2PixelRectSize( fPixelRectSize const fRectSize )
{
	return PixelRectSize( convert2PIXEL( fRectSize.GetX() ), convert2PIXEL( fRectSize.GetY() ) );
}

static fPIXEL convert2fPIXEL( PIXEL const pixel )
{
	return fPIXEL( CastToFloat( pixel.GetValue() ) );
}

static fPixelPoint convert2fPixelPoint( PixelPoint const pixPoint )
{
	return fPixelPoint( convert2fPIXEL( pixPoint.GetX() ), convert2fPIXEL( pixPoint.GetY() ) );
}

static fPixelRectSize convert2fPixelRectSize( PixelRectSize const rectSize )
{
	return fPixelRectSize( convert2fPIXEL( rectSize.GetX() ), convert2fPIXEL( rectSize.GetY() ) );
}