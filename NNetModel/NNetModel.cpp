// NNetModel.cpp 
//
// NNetModel

#include "stdafx.h"
#include "NNetTypes.h"
#include "NNetPoint.h"
#include "NNetModel.h"

void NNetModel::InitClass
( 
	int                 const iNrOfNeighbors,
	ObserverInterface * const pObservers,
	EventInterface    * const pEvent
)
{
}

NNetModel::NNetModel( )
  : m_timeStamp( microseconds( 0 ) ),
	m_neuron  ( NNetPoint(   20.0_MicroMeter,   20.0_MicroMeter ) ),
	m_knot    ( NNetPoint( 1000.0_MicroMeter, 1400.0_MicroMeter ) ),
	m_pipeline( ),
	m_iCounter( 0 )
{
	m_neuron.AddOutgoing( & m_pipeline );
	m_knot.AddIncomming( & m_pipeline );
}

NNetModel * NNetModel::CreateModel( )
{
	return new NNetModel( );
}

void NNetModel::DestroyCore( NNetModel * pCore )
{
	delete pCore;
}

void NNetModel::Compute( )
{
	int iFrequency = 500; // Hertz
	int iStepsBetweenTrigger = CastToInt( microseconds::period::den / ( TIME_RESOLUTION.count() * iFrequency ) );
	
	m_pipeline.Step( );

	if ( m_iCounter == 0 )
	{
		m_neuron.Trigger();
		m_iCounter = iStepsBetweenTrigger;
	}
	else
	{
		--m_iCounter;
	}

	m_timeStamp += TIME_RESOLUTION;
}

void NNetModel::ResetAll( )
{

}
