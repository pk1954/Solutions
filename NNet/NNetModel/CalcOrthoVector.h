// CalcOrthoVector.h
//
// NNetModel

#pragma once

#include "MoreTypes.h"
#include "LineType.h"
#include "BaseKnot.h"
#include "NobPtrList.h"

template <Nob_t T>
MicroMeterPoint const CalcOrthoVector
(
	MicroMeterLine  const & line,
	NobPtrList<T> const & list
)
{
	unsigned int uiLeftConnections  { 0 };
	unsigned int uiRightConnections { 0 };
	list.Apply2All
	(	
		[&](Nob const & nob)
		{ 
			if ( nob.IsBaseKnot() )
			{
				BaseKnot const & baseKnot { static_cast<BaseKnot const &>(nob) };
				baseKnot.m_connections.Apply2AllInPipes
				( 
					[&](Pipe & pipe) 
					{ 
						MicroMeterPoint pnt { pipe.GetStartPoint() };
						if ( PointToLine(line, pnt) < 0.0_MicroMeter )
							++uiLeftConnections;
						else
							++uiRightConnections;
					}
				);
				baseKnot.m_connections.Apply2AllOutPipes
				( 
					[&](Pipe & pipe) 
					{ 
						MicroMeterPoint pnt { pipe.GetEndPoint() };
						if ( PointToLine(line, pnt) < 0.0_MicroMeter )
							++uiRightConnections;
						else
							++uiLeftConnections;
					}
				);
			}
		}	
	);

	MicroMeterPoint orthoVector = line.OrthoVector();
	if ( uiRightConnections < uiLeftConnections )
		orthoVector = -orthoVector;
	return orthoVector;
}