// gridRect.cpp :
//

#include "stdafx.h"
#include "limits.h"
#include <iostream>
#include <algorithm>  // min/max templates
#include "gridPoint.h"
#include "gridRect.h"

using namespace std;

GridPoint GridRect::clipStartPoint( ) const
{
	return GridPoint
	(
		max( m_lLeft, GRID_RECT_FULL().m_lLeft ),
		max( m_lTop,  GRID_RECT_FULL().m_lTop  )
	);
};

GridPoint GridRect::clipEndPoint( ) const
{
	return GridPoint
	(
		min( m_lRight,  GRID_RECT_FULL().m_lRight ),
		min( m_lBottom, GRID_RECT_FULL().m_lBottom )
	);
};

void Apply2Rect
( 
	GridPointFunc const & func,
	GridPoint             gpStart,
	GridPoint             gpEnd,
	bool          const   fWithBorders
)
{
	if (fWithBorders)
	{
		gpStart -= GridPoint( GRID_COORD(1_GRID_COORD), GRID_COORD(1_GRID_COORD) );
		gpEnd   += GridPoint( GRID_COORD(1_GRID_COORD), GRID_COORD(1_GRID_COORD) );
	}

    for ( short y = gpStart.GetYvalue(); y <= gpEnd.GetYvalue(); ++y )
    for ( short x = gpStart.GetXvalue(); x <= gpEnd.GetXvalue(); ++x )
	{
		func( GridPoint{ GRID_COORD(x), GRID_COORD(y) } );
	}
}

void Apply2Grid( GridPointFunc const & func, bool const fWithBorders )
{
	Apply2Rect( func, GRID_ORIGIN(), GRID_MAXIMUM(), fWithBorders );
}

std::wostream & operator << ( std::wostream & out, GridRect const & rect )
{
    out << rect.GetStartPoint() << L' ' << rect.GetEndPoint();
    return out;
}
