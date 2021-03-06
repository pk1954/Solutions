// Analyzer.cpp
//
// NNetModel

#include "stdafx.h"
#include "MoreTypes.h"
#include "RectType.h"
#include "win32_util.h"
#include "NNetModelReaderInterface.h"
#include "Knot.h"
#include "Analyzer.h"

using std::endl;
using std::to_wstring;

ShapeStack const ModelAnalyzer::FindLoop( NNetModelReaderInterface const & nmri )
{
	int const iNrOfShapes { Cast2Int(nmri.GetSizeOfShapeList()) };

	for ( int iMaxLoopSize = 5; iMaxLoopSize <= iNrOfShapes + 1; iMaxLoopSize += 2 )
	{
		int iCounter { iMaxLoopSize };
		m_iRecDepth = iMaxLoopSize;
		m_bStop     = false;
		statusDisplay( wstring( L"Looking for loop of size " ) + to_wstring( iMaxLoopSize ) + L". Press ESC to stop." );
		m_shapeStack.Clear();
		if ( nmri.GetUPShapes().Apply2AllB<BaseKnot>([&](BaseKnot const & b) { return findLoop( b ); } ) )
		{
			if ( m_bStop )  
			{
				m_shapeStack.Clear();
				statusDisplay( L"analysis aborted by user" );
				return m_shapeStack;
			}
			else 
			{
				statusDisplay( L"loop found" );
				return m_shapeStack;
			}
		}
	}

	statusDisplay( L"no loop found" );
	return m_shapeStack;
}

// findLoop - try to find a loop in model
//
// returns true, if loop found (m_bStop == false) or aborted by user (m_bStop == true). 
//         false, if analysis completed and no loop found

bool ModelAnalyzer::findLoop( Shape const & shape )
{
	if ( ( * m_pEscFunc )() )
		m_bStop = true;

	if ( m_bStop )
		return true;

	if ( m_shapeStack.Size() == m_iRecDepth )
		return false;  // maximum search depth reached

	assert( shape.IsDefined() );
	m_shapeStack.Add( & shape );

	bool bResult { false };

	if ( 
		  ( m_shapeStack.Size() > 1 ) &&                        // we are beyond the initial shape
	      ( shape.GetId() == m_shapeStack.GetFirst().GetId() )  // and found the same shape again
	   )
	{
		bResult = true;  // loop found. Do not pop_back stack!
	}
	else if ( shape.IsPipe() )
	{
		bResult = findLoop( * static_cast<Pipe const &>(shape).GetEndKnotPtr() );  // recursion
	}
	else if ( shape.IsBaseKnot() )
	{
		bResult = static_cast<BaseKnot const &>(shape).m_connections.Apply2AllOutPipesB
		( 
			[&]( Pipe const & pipe ) 
			{ 
				return findLoop( pipe ); 
			} 
		);
	}
	else
	{
		assert( false );  // shape is neither Pipe nor baseknot
	}

	if ( ! bResult )
		m_shapeStack.RemoveLast(); // no loop in this branch

	return bResult;
}

bool ModelAnalyzer::hasAnomaly( Knot const & knot )
{
	bool bFoundAnomaly { false };

	if ( ! knot.m_connections.HasIncoming() )
	{
		knot.m_connections.Apply2AllOutPipes( [&]( Pipe & pipe ) { m_shapeStack.Add( & pipe ); } );
		bFoundAnomaly = true;
	}
	else if ( ! knot.m_connections.HasOutgoing() )
	{
		knot.m_connections.Apply2AllInPipes( [&]( Pipe & pipe ) { m_shapeStack.Add( & pipe ); } );
		bFoundAnomaly = true;
	}

	if ( bFoundAnomaly )
		m_shapeStack.Add( & knot );

	return bFoundAnomaly; 
}

ShapeStack const ModelAnalyzer::FindAnomaly( NNetModelReaderInterface const & nmri)
{
	m_shapeStack.Clear();
	bool const bFound { nmri.GetUPShapes().Apply2AllB<Knot>( [&](Knot const & k) { return hasAnomaly(k); } ) };
	if ( ! bFound )
		statusDisplay( L"no anomalies found" );
	return m_shapeStack;
}
