// EvolutionCore.cpp
//

#include "strategy.h"
#include "genome.h"
#include "gridPOI.h"
#include "grid_model.h"
#include "gridNeighbor.h"
#include "EvolutionCore.h"
#include "EvolutionCoreImpl.h"
#include "EvolutionCoreWrappers.h"

void EvolutionCore::InitClass
( 
	int                 const iNrOfNeighbors,
	ObserverInterface * const pObservers,
	EventInterface    * const pEvent
)
{
    Neighborhood::InitClass( iNrOfNeighbors );
    Grid::InitClass( pObservers, pEvent );
}

EvolutionCore * EvolutionCore::CreateCore( )
{
    return new EvolutionCoreImpl( );
}

void EvolutionCore::DestroyModel( EvolutionCore * pCore )
{
	delete pCore;
}

BYTES const EvolutionCore::GetCoreSize() 
{ 
	return BYTES(sizeof(EvolutionCoreImpl)) + GetGridHeapSize(); 
};

unsigned int EvolutionCore::GetMaxPartnerMemory( )
{ 
	return StrategyData::GetMaxPartnerMemory( ).GetValue( ); 
}

unsigned int EvolutionCore::GetNrInteractionsWithKnownCulprit( ) 
{ 
	return StrategyData::GetNrInteractionsWithKnownCulprit( ); 
}

unsigned int EvolutionCore::GetNrInteractionsWithUnknownCulprit( ) 
{ 
	return StrategyData::GetNrInteractionsWithUnknownCulprit( ); 
}

bool EvolutionCore::IsEnabled( Action::Id const action )
{
	return Genome::IsEnabled( action );
}

std::wostringstream* EvolutionCore::GetProtocolData( )
{
	return Grid::GetProtocolData( );
}

void EvolutionCore::ClearProtocolData( )
{
	Grid::ClearProtocolData( );
}
