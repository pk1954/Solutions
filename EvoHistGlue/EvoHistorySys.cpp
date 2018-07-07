// EvoHistorySys.cpp
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <sstream> 
#include "config.h"
#include "win32_util.h"
#include "win32_status.h"
#include "HistAllocThread.h"
#include "HistoryGeneration.h"
#include "HistorySystem.h"
#include "EvoModelFactory.h"
#include "EvoHistorySys.h"

class WorkThread;

EvoHistorySys::EvoHistorySys( ) :
    m_pEvoModelWork   ( nullptr ),
	m_pEvoModelFactory( nullptr ),
    m_pHistorySystem  ( nullptr ),
	m_pHistAllocThread( nullptr ),
    m_pStatusBar      ( nullptr ),
	m_bAskHistoryCut  ( false )
{}

void EvoHistorySys::Start
(
	EvolutionModelData * const pEvolutionModelData,
	WorkThread         * const pWorkThread,
    StatusBar          * const pStatusBar,
	unsigned long        const ulModelSize,
	bool                 const bAskHistoryCut // true: ask user for history cut, false: cut without asking
)
{
    LONG const lMaxHistSize         = Util::GetMaxNrOfSlots( ulModelSize );
    LONG const lHistEntriesDemanded = Config::GetConfigValue( Config::tId::nrOfHistorySlots );
	LONG const lHistEntries         = min( lHistEntriesDemanded, lMaxHistSize * 80 / 100 );  // use only 80% of available memory
	
	assert( lHistEntries < SHRT_MAX );
    short sNrOfSlots = static_cast<short>( lHistEntries );

    HIST_GENERATION const genMaxNrOfGens = Config::GetConfigValue( Config::tId::maxGeneration );

    m_pEvoModelWork    = new EvoModelData ( pEvolutionModelData, pWorkThread );
	m_pEvoModelFactory = new EvoModelFactory( pWorkThread );

    m_pHistorySystem = HistorySystem::CreateHistorySystem( );

	m_pHistorySystem->InitHistorySystem
    (
        sNrOfSlots,
        genMaxNrOfGens,
        m_pEvoModelWork,
        m_pEvoModelFactory,
		static_cast< short >( tEvoCmd::reset ),
		0
    );

	m_pStatusBar = pStatusBar;
	m_bAskHistoryCut = bAskHistoryCut;

	m_pHistAllocThread = new HistAllocThread( m_pHistorySystem, TRUE );   // delegate allocation of history slots to a work thread
}

EvoHistorySys::~EvoHistorySys( ) 
{
    shutDownHistoryCache( );
	delete m_pHistAllocThread;
	delete m_pEvoModelFactory;
	delete m_pHistorySystem;
};

class FindGridPointFunctor : public GenerationProperty
{
public:

    FindGridPointFunctor( IndId const id ) : m_id( id ) {}

    virtual bool operator() ( ModelData const * pModelData ) const
    {
        EvoModelData const * pEvoModelData = static_cast< EvoModelData const * >( pModelData );
        return ( pEvoModelData->FindGridPoint( m_id ).IsNotNull( ) );  // id is alive
    }

private:
    IndId const m_id;
};

HIST_GENERATION EvoHistorySys::GetFirstGenOfIndividual( IndId const & id ) const  
{ 
    return id.IsDefined( ) ? m_pHistorySystem->FindFirstGenerationWithProperty( FindGridPointFunctor( id ) ) : -1; 
}

HIST_GENERATION EvoHistorySys::GetLastGenOfIndividual ( IndId const & id ) const  
{ 
    return id.IsDefined( ) ? m_pHistorySystem->FindLastGenerationWithProperty ( FindGridPointFunctor( id ) ) : -1; 
}

bool EvoHistorySys::EvoCreateEditorCommand( tEvoCmd cmd, unsigned short usParam ) 
{ 
	if ( 
		  m_pHistorySystem->IsInHistoryMode( ) &&  // If in history mode,
          m_bAskHistoryCut &&                      // and asking user is requested
	      ! askHistoryCut( m_pHistorySystem )      // ask user, if all future generations should be erased
	   )
		return false;  // user answered no, do not erase

	m_pHistorySystem->ClearHistory( GetCurrentGeneration( ) );  // if in history mode: cut off future generations
	m_pHistorySystem->CreateAppCommand( static_cast< short >( cmd ), usParam ); 
	return true;
}

bool EvoHistorySys::IsEditorCommand( HIST_GENERATION const gen ) const
{
	unsigned short const usCmd  = m_pHistorySystem->GetGenerationCmd( gen );
	tEvoCmd        const evoCmd = static_cast<tEvoCmd>( usCmd );
	return ::IsEditorCommand( evoCmd );
}

bool EvoHistorySys::askHistoryCut( HistorySystem * pHistSys ) const
{
    std::wostringstream wBuffer;
    HIST_GENERATION  genCurrent  = pHistSys->GetCurrentGeneration( );
    HIST_GENERATION  genYoungest = pHistSys->GetYoungestGeneration( );
    assert( genCurrent < genYoungest );
    wBuffer << L"Gen " << ( genCurrent + 1 ) << L" - " << genYoungest << L" will be deleted.";
    return IDOK == MessageBox( nullptr, L"Cut off history?", wBuffer.str( ).c_str( ), MB_OKCANCEL | MB_SYSTEMMODAL );
}

void EvoHistorySys::shutDownHistoryCache( )
{
    int iMax = GetNrOfHistCacheSlots( ) - 1;
    int iPercentLast = 0;
    for ( int iRun = iMax; iRun >= 0; --iRun )
    {
        int iPercent = ( iRun * 100 ) / iMax;
        if ( iPercent != iPercentLast )
        {
            std::wstring wstrLine = L"... deleting history buffer: " + to_wstring( iPercent ) + L"%";
            m_pStatusBar->DisplayStatusLine( wstrLine );
            iPercentLast = iPercent;
        }
        m_pHistorySystem->ShutDownHistCacheSlot( iRun );
    }
}
