// win32_histWorkerThread.h

#pragma once

#include "win32_worker_thread.h"
#include "EvoHistorySys.h"

class EvoHistorySys;
class EvolutionCore;
class EvolutionModelData;

class HistWorkThread : public WorkThread
{
public:
    HistWorkThread( wostream *, EvolutionCore * const, EvolutionModelData * const, EvoHistorySys * const );
    ~HistWorkThread( );

	void PostPrevGeneration();
	void PostGotoGeneration( HIST_GENERATION const );
	void PostHistoryAction( UINT const, GridPoint const );

	HIST_GENERATION GetGenDemanded( ) const { return m_genDemanded; }

	// overwrite methods from WorkThread

	virtual void PostNextGeneration();

private:

    virtual void GenerationStep( );
	virtual void GenerationRun( );
	virtual void StopComputation( );
	virtual void ApplyEditorCommand( tEvoCmd const, short const );
	virtual void DoEdit( GridPoint const );

	void postGotoGeneration( HIST_GENERATION const );

    HIST_GENERATION      m_genDemanded;
    EvoHistorySys      * m_pEvoHistorySys;
    EvolutionCore      * m_pCore;
    EvolutionModelData * m_pModelWork;
};
