// gridCoord.h
//

#pragma once

#include "util.h"
#include "NamedType.h"

using GRID_COORD = NamedType< short, struct GRID_COORD_Parameter >;

inline bool IsEven( GRID_COORD const a ) { return a.GetValue() % 2 == 0; }
inline bool IsOdd ( GRID_COORD const a ) { return a.GetValue() % 2 != 0; }

constexpr GRID_COORD operator"" _GRID_COORD( unsigned long long ull )
{
	return GRID_COORD( CastToUnsignedShort( ull ) );
}

GRID_COORD const MAX_GRID_COORD = 255_GRID_COORD;

static long const GRID_WIDTH_  = 200;
static long const GRID_HEIGHT_ = 100;

static GRID_COORD const GRID_WIDTH  = 200_GRID_COORD;
static GRID_COORD const GRID_HEIGHT = 100_GRID_COORD;
static GRID_COORD const GRID_X_MIN  = 0_GRID_COORD;
static GRID_COORD const GRID_Y_MIN  = 0_GRID_COORD;
static GRID_COORD const GRID_X_MAX  = GRID_WIDTH  - 1_GRID_COORD;
static GRID_COORD const GRID_Y_MAX  = GRID_HEIGHT - 1_GRID_COORD;