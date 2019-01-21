// gridPoint.h : 
//

#pragma once

#include <stdlib.h>   // abs
#include <algorithm>  // min/max templates
#include <iostream>
#include <limits.h>
#include <assert.h>
#include <functional>
#include "debug.h"
#include "util.h"
#include "Int24.h"
#include "EvolutionTypes.h"

class GridPoint
{
public:

    GridPoint( ) : x( GP_NULL.x ), y( GP_NULL.y ) {}

	GridPoint( GRID_COORD const _x, GRID_COORD const _y ) 
		: x(_x), y(_y)
	{}

    virtual ~GridPoint() {};

    GridPoint operator++ () { ++x; ++y; return * this; }
    GridPoint operator-- () { --x; --y; return * this; }

    bool      const operator== (GridPoint const a) const { return (a.x == x) && (a.y == y); }
    bool      const operator!= (GridPoint const a) const { return (a.x != x) || (a.y != y); }

    GridPoint const operator+= (GridPoint const a) { x += a.x; y += a.y; return * this; }
    GridPoint const operator-= (GridPoint const a) { x -= a.x; y -= a.y; return * this; }
    GridPoint const operator%= (GridPoint const a) { x %= a.x; y %= a.y; return * this; }

    GridPoint const operator+= (GRID_COORD const l) { x += l; y += l; return * this; }
    GridPoint const operator-= (GRID_COORD const l) { x -= l; y -= l; return * this; }

	GridPoint const operator%= (int const i) { x %= i; y %= i; return * this; }
    GridPoint const operator/= (int const i) { x /= i; y /= i; return * this; }

	bool IsEvenColumn( ) const { return x.IsEven(); }
	bool IsOddColumn ( ) const { return x.IsOdd(); }

    static int const GRID_AREA = GRID_WIDTH_ * GRID_HEIGHT_;

    static GridPoint const GRID_ORIGIN;
    static GridPoint const GRID_MAXIMUM;
    static GridPoint const GRID_CENTER;
    static GridPoint const GRID_SIZE;
    static GridPoint const GP_NULL;

    bool const IsInGrid( ) const { return (0 <= x.get()) && (0 <= y.get()) && (x < GRID_WIDTH) && (y < GRID_HEIGHT); };

    bool IsNull   ( ) const { return * this == GP_NULL; };
    bool IsNotNull( ) const { return * this != GP_NULL; };

    void Set2Null( ) { * this = GP_NULL; }

    GRID_COORD x;
    GRID_COORD y;
};

inline GridPoint const operator+ (GridPoint const a, GridPoint const b) { GridPoint res(a); res += b; return res; }
inline GridPoint const operator- (GridPoint const a, GridPoint const b) { GridPoint res(a); res -= b; return res; }
inline GridPoint const operator% (GridPoint const a, GridPoint const b) { GridPoint res(a); res %= b; return res; }

inline GridPoint const operator+ (GridPoint const a, GRID_COORD const l) { GridPoint res(a); res += l; return res; }
inline GridPoint const operator- (GridPoint const a, GRID_COORD const l) { GridPoint res(a); res -= l; return res; }

inline GridPoint const operator% (GridPoint const a, int const i) { GridPoint res(a); res %= i; return res; }
inline GridPoint const operator/ (GridPoint const a, int const i) { GridPoint res(a); res /= i; return res; }

inline GridPoint const abs(GridPoint const a ) { return GridPoint( abs(a.x), abs(a.y) ); }

inline GridPoint const Min(GridPoint const a, GridPoint const b) { return GridPoint( min(a.x, b.x), min(a.y, b.y) ); }
inline GridPoint const Max(GridPoint const a, GridPoint const b) { return GridPoint( max(a.x, b.x), max(a.y, b.y) ); }

inline bool const Neighbors( GridPoint const a, GridPoint const b )
{ 
    GridPoint gpDiff( abs(a - b) );
    return ( 
		      ((gpDiff.x <= GRID_COORD(1_GRID_COORD)) || (gpDiff.x == GRID_X_MAX)) && 
		      ((gpDiff.y <= GRID_COORD(1_GRID_COORD)) || (gpDiff.y == GRID_Y_MAX))
		   );
}

inline GridPoint const Wrap2Grid(GridPoint const gp) 
{ 
	return (gp + GridPoint::GRID_SIZE) % GridPoint::GRID_SIZE; 
}

typedef std::function<void (GridPoint const)> GridPointFunc;
typedef std::function<short(short const, short const)> ManipulatorFunc;

std::wostream & operator << ( std::wostream &, GridPoint const );
