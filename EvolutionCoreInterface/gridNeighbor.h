// gridNeighbor.h : 
//

#pragma once

#include <array>
#include <vector>
#include "gridPoint.h"

using namespace std;

class Neighborhood
{
public:
    static void InitClass( int const );
	static bool Apply2All( GridPoint const, GridPoint_Functor & );

	static int  GetNrOfNeighbors( ) 
	{ 
		return m_iNrOfNeighbors; 
	}

	Neighborhood( )	: m_neighbors( )
	{
		m_neighbors.reserve( m_iNrOfNeighbors );
	}

    void AddToList( GridPoint const & gp ) 
	{ 
		m_neighbors.push_back( gp ); 
		assert( m_neighbors.size( ) <= m_iNrOfNeighbors );
	}

	void RemoveFromList( int const iIndex )
	{
		assert( m_neighbors.size( ) >= iIndex);
		m_neighbors.erase( m_neighbors.begin() + iIndex );
	}

    void ClearList( ) 
	{ 
		m_neighbors.clear( ); 
	}

	size_t GetLength( ) const 
	{ 
		return m_neighbors.size( ); 
	}
    
	GridPoint const & GetElement( unsigned int const uiIndex  ) const 
	{ 
		return m_neighbors[uiIndex]; 
	}

    GridPoint const & GetRandomElement( unsigned int const uiRandom ) const 
    {
        assert( m_neighbors.size( ) > 0 ); 
        //lint -e414   possible division by 0
        return m_neighbors[uiRandom % m_neighbors.size( )];
        //lint +e414 
    }

private:
	typedef vector< GridPoint >                            NEIGHBORS;
	typedef array < NEIGHBORS,    GridPoint::GRID_WIDTH  > NEIGHBOR_ROW;
	typedef array < NEIGHBOR_ROW, GridPoint::GRID_HEIGHT > NEIGHBOR_GRID;

	static int             m_iNrOfNeighbors;
	static NEIGHBOR_GRID * m_pGridNeighbors;

    NEIGHBORS m_neighbors;

	static NEIGHBORS & getNeighbors( GridPoint const gp )
	{
		return (* m_pGridNeighbors)[ gp.y ][ gp.x ];
	}
};
