// action.cpp : 
//
// EvolutionCore

#include "stdafx.h"
#include "Genome.h"
#include "action.h"

void Action::Apply2AllEnabledActions( std::function<void(Id const &)> const & func )
{
    for ( int index = 0; index < static_cast<int>( Id::count ); ++index )
	{
		Id const action { static_cast<Id>(index) };
	 	if ( Genome::IsEnabled( action ) )
		    func( static_cast<Id>(index) );   
	}
}
