// EvolutionCore.h
//

#pragma once

#include <functional>
#include "EvolutionTypes.h"

class DisplayFunctor;
class EvolutionModelData;

class EvolutionCore
{
public:
    virtual ~EvolutionCore( ) { };

// debugging functions

    virtual void DumpGridPointList( EvolutionModelData * const ) const = 0;

// manipulating functions

    virtual void SetGridDisplayFunctor( DisplayFunctor const * const ) = 0;
    virtual void Compute   ( EvolutionModelData * const ) = 0;

    // static functions
    
    static void            InitClass( );
    static EvolutionCore * CreateCore( );
    static void            DeleteCore( );
    static unsigned long   GetModelSize( );
    static unsigned int    GetNrInteractionsWithKnownCulprit( );
    static unsigned int    GetNrInteractionsWithUnknownCulprit( );
    static unsigned int    GetMaxPartnerMemory( );
	static bool            IsEnabled( tAction const );
	static bool            IsEnabled( tGeneType const );
};
