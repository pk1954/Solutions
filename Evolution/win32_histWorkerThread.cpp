// win32_histWorkerThread.cpp

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include "resource.h"
#include "EvolutionModelData.h"
#include "EvolutionCore.h"
#include "win32_status.h"
#include "win32_editor.h"
#include "win32_histWorkerThread.h"

using namespace std;

HistWorkThread::HistWorkThread
( 
    wostream           *       pTraceStream,
    EvolutionCore      * const pCore,
    EvolutionModelData * const pModel,
    EvoHistorySys      * const pHistorySys
) :
    WorkThread( pTraceStream ),
    m_genDemanded( 0 ),
    m_pEvoHistorySys( pHistorySys ),
    m_pCore( pCore ),
    m_pModelWork( pModel )
{ }

HistWorkThread::~HistWorkThread( )
{}

// GenerationStep - perform one history step towards demanded generation
//                - update editor state if neccessary

void HistWorkThread::GenerationStep( )
{
    if ( m_pEvoHistorySys->GetCurrentGeneration( ) != m_genDemanded )
    {
        m_pEvoHistorySys->EvoApproachHistGen( m_genDemanded );

        if ( m_pCore->EditorStateHasChanged( m_pModelWork ) )
        {
            m_pCore->SaveEditorState( m_pModelWork );
            m_pEditorWindow->UpdateControls( );
        }

        postMsg2WorkThread( THREAD_MSG_REFRESH, 0, 0 );
		WorkThread::PostNextGeneration(  );   // will call indirectly HistWorkThread::GenerationStep
    }
}

void HistWorkThread::GenerationRun( )
{
    m_genDemanded = m_pEvoHistorySys->GetCurrentGeneration( ) + 1;
    WorkThread::GenerationRun( );
}

void HistWorkThread::ApplyEditorCommand( tEvoCmd const evoCmd, short const sParam )
{
    if ( m_pEvoHistorySys->CreateEditorCommand( evoCmd, sParam ) )
        m_pCore->SaveEditorState( m_pModelWork );
}

void HistWorkThread::StopComputation()
{
	m_genDemanded = m_pEvoHistorySys->GetCurrentGeneration();
	WorkThread::StopComputation();
}

void HistWorkThread::DoEdit( GridPoint const gp )
{
	if ( m_pEvoHistorySys->CreateEditorCommand( tEvoCmd::editSetXvalue, gp.x ) )
		 m_pEvoHistorySys->CreateEditorCommand( tEvoCmd::editSetYvalue, gp.y );
}

void HistWorkThread::postGotoGeneration( HIST_GENERATION const gen )
{
	m_genDemanded = gen;
	WorkThread::PostNextGeneration(  );   // will call indirectly HistWorkThread::GenerationStep
}

void HistWorkThread::PostNextGeneration( )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << endl;

    wcout << __FUNCTION__ << L" " << m_pEvoHistorySys->GetCurrentGeneration( ) << endl;
	postGotoGeneration( m_pEvoHistorySys->GetCurrentGeneration( ) + 1 );
}

void HistWorkThread::PostPrevGeneration( )
{
    wcout << __FUNCTION__ << L" " << m_pEvoHistorySys->GetCurrentGeneration( ) << endl;
	if (m_pEvoHistorySys->GetCurrentGeneration() > 0)
		PostGotoGeneration( m_pEvoHistorySys->GetCurrentGeneration() - 1 );
	else
		(void)MessageBeep(MB_OK);  // first generation reached
}

void HistWorkThread::PostHistoryAction( UINT const uiID, GridPoint const gp )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << uiID << L" " << gp << endl;

	assert( m_pModelWork->IsAlive(gp) );
	assert( (uiID == IDM_GOTO_ORIGIN) || (uiID == IDM_GOTO_DEATH) );

	IndId           idTarget  = m_pModelWork->GetId(gp);
	HIST_GENERATION genTarget = ( uiID == IDM_GOTO_ORIGIN )
	                            ? m_pEvoHistorySys->GetFirstGenOfIndividual(idTarget)
		                        : m_pEvoHistorySys->GetLastGenOfIndividual(idTarget);
	
	postGotoGeneration( genTarget );
}

void HistWorkThread::PostGotoGeneration( HIST_GENERATION const gen )
{
    if ( m_bTrace )
        * m_pTraceStream << __func__ << L" " << gen << endl;

    assert( gen >= 0 );
    assert( gen <= m_pEvoHistorySys->GetHistorySystem( )->GetYoungestGeneration( ) );

	postGotoGeneration( gen );
}
