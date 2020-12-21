// NNetModelCommands.cpp
//
// NNetModel

#include "stdafx.h"
#include "CommandStack.h"
#include "AddIncoming2KnotCommand.h"
#include "AddIncoming2PipeCommand.h"
#include "AddOutgoing2KnotCommand.h"
#include "AddOutgoing2PipeCommand.h"
#include "AnalyzeAnomaliesCommand.h"
#include "AnalyzeLoopsCommand.h"
#include "AppendNeuronCommand.h"
#include "AppendInputNeuronCommand.h"
#include "ClearBeepersCommand.h"
#include "Connect2BaseKnotCommand.h"
#include "Connect2PipeCommand.h"
#include "CopySelectionCommand.h"
#include "DeletePipeCommand.h"
#include "DisconnectBaseKnotCommand.h"
#include "InsertBaseKnotCommand.h"
#include "MoveBaseKnotCommand.h"
#include "MovePipeCommand.h"
#include "MoveSelectionCommand.h"
#include "NewInputNeuronCommand.h"
#include "NewNeuronCommand.h"
#include "NNetModelStorage.h"
#include "SelectAllBeepersCommand.h"
#include "SelectAllCommand.h"
#include "SelectionCommand.h"
#include "SelectShapeCommand.h"
#include "SelectShapesInRectCommand.h"
#include "SelectSubtreeCommand.h"
#include "SetParameterCommand.h"
#include "SetPulseRateCommand.h"
#include "SetTriggerSoundCommand.h"
#include "ToggleStopOnTriggerCommand.h"
#include "NNetModelWriterInterface.h"
#include "NNetParameters.h"
#include "NNetModelCommands.h"

using std::wcout;
using std::endl;
using std::move;
using std::make_unique;
using std::unique_ptr;

void NNetModelCommands::Initialize
( 
	NNetModelWriterInterface * const pWriterInterface,
	Param                    * const pParam,
	CommandStack             * const pCmdStack,
	NNetModelStorage         * const pStorage,
	Observable               * const pDynamicModelObservable
) 
{ 
	m_pMWI                    = pWriterInterface;
	m_pCmdStack               = pCmdStack;
	m_pStorage                = pStorage;
	m_pParam                  = pParam;
	m_pDynamicModelObservable = pDynamicModelObservable;
}

void NNetModelCommands::UndoCommand( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << endl;
	if ( ! m_pCmdStack->UndoCommand() )
		MessageBeep( MB_ICONWARNING );
}

void NNetModelCommands::RedoCommand( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << endl;
	if ( ! m_pCmdStack->RedoCommand() )
		MessageBeep( MB_ICONWARNING );
}

void NNetModelCommands::ResetModel( )
{ 
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << endl;
	m_pMWI->ResetModel( );
	m_pCmdStack->Clear();
	m_pStorage->ResetModelPath( );
	m_pMWI->CreateInitialShapes();
}

void NNetModelCommands::ReadModel( bool const bAsync, bool const bReset, wstring const wstrPath )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << bAsync << L" " << wstrPath << endl;
	m_pStorage->Read( wstrPath, bAsync, bReset );
	m_pCmdStack->Clear();
}

///////////////////// xxxxxx commands /////////////////////////////

void NNetModelCommands::DeleteShape( ShapeId const id )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << endl;
	deleteShape( id );
}

void NNetModelCommands::DeleteSelection( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << endl;
	m_pCmdStack->OpenSeries();
	{
		//m_pMWI->Apply2All<Shape>                   
		//(                                                        
		//	[&]( Shape const & s )
		//	{
		//		if ( s.IsSelected() )
		//			deleteShape( s.GetId() );
		//	} 
		//); 


		vector<ShapeId> list;                         // detour with secondary list is neccessary!
		m_pMWI->Apply2All<Shape>                      // cannot delete shapes directly in Apply2All
		(                                                        
			[&]( Shape const & s )                    // first construct list
			{
				if ( s.IsSelected() )
					list.push_back( s.GetId() );
			} 
		); 
		for ( ShapeId const id : list )               // then run through list 
		{
			deleteShape( id );                        // and delete shapes in model
		}                                             // using ids from list
	}
	m_pCmdStack->CloseSeries();
}

///////////////////// undoable commands /////////////////////////////

void NNetModelCommands::Connect( ShapeId const idSrc, ShapeId const idDst )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << idSrc << L" " << idDst << endl;
	unique_ptr<Command> pCmd;
	if ( m_pMWI->IsPipe( idDst ) ) 
		pCmd = make_unique<Connect2PipeCommand>( idSrc, idDst );
	else
		pCmd = make_unique<Connect2BaseKnotCommand>( idSrc, idDst );
	m_pCmdStack->PushCommand( move( pCmd ) );
}

void NNetModelCommands::deleteShape( ShapeId const id )
{
	unique_ptr<Command> pCmd;
	if ( m_pMWI->IsPipe( id )  ) 
		pCmd = make_unique<DeletePipeCommand>( id ) ;
	else 
		pCmd = make_unique<DisconnectBaseKnotCommand>( id, true );
	m_pCmdStack->PushCommand( move( pCmd ) );
}

void NNetModelCommands::Disconnect( ShapeId const id )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << endl;
	m_pCmdStack->PushCommand( make_unique<DisconnectBaseKnotCommand>( id, false ) );
}

void NNetModelCommands::ToggleStopOnTrigger( ShapeId const id )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << endl;
	m_pCmdStack->PushCommand( make_unique<ToggleStopOnTriggerCommand>( id ) );
}

void NNetModelCommands::SetPulseRate( ShapeId const id, fHertz const fNewValue )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << fNewValue << endl;
	m_pCmdStack->PushCommand( make_unique<SetPulseRateCommand>( id, fNewValue ) );
}

void NNetModelCommands::SetTriggerSound( ShapeId const id, SoundDescr const & sound )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << sound.m_bOn << L" " << sound.m_frequency << L" " << sound.m_duration << endl;
	m_pCmdStack->PushCommand( make_unique<SetTriggerSoundCommand>( id, sound ) );
}

void NNetModelCommands::SetParameter( tParameter const param, float const fNewValue )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << GetParameterName( param ) << L" " << fNewValue << endl;
	m_pCmdStack->PushCommand( make_unique<SetParameterCommand>( SetParameterCommand( m_pParam, param, fNewValue ) ) );
}

void NNetModelCommands::MoveShape( ShapeId const id, MicroMeterPoint const & delta )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << delta << endl;
	unique_ptr<Command> pCmd;
	if ( m_pMWI->IsPipe( id ) ) 
		pCmd = make_unique<MovePipeCommand>( id, delta );
	else 
		pCmd = make_unique<MoveBaseKnotCommand>( id, delta );
	m_pCmdStack->PushCommand( move( pCmd ) );
}

void NNetModelCommands::MoveSelection( MicroMeterPoint const & delta )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << delta << endl;
	m_pCmdStack->PushCommand( make_unique<MoveSelectionCommand>( delta ) );
}

void NNetModelCommands::AddOutgoing2Knot( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand( make_unique<AddOutgoing2KnotCommand>( id, pos + STD_OFFSET ) );
}

void NNetModelCommands::AddIncoming2Knot( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand( make_unique<AddIncoming2KnotCommand>( id, pos - STD_OFFSET ) );
}

void NNetModelCommands::AddOutgoing2Pipe( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand( make_unique<AddOutgoing2PipeCommand>( id, pos ) );
}

void NNetModelCommands::AddIncoming2Pipe( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand( make_unique<AddIncoming2PipeCommand>( id, pos ) );
}

void NNetModelCommands::InsertKnot( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand( make_unique<InsertBaseKnotCommand<Knot>>( id, pos ) );
}

void NNetModelCommands::InsertNeuron( ShapeId const id, MicroMeterPoint const & pos )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand( make_unique<InsertBaseKnotCommand<Neuron>>( id, pos ) );
}

void NNetModelCommands::NewNeuron( MicroMeterPoint const & pos )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << pos << endl;
	m_pCmdStack->PushCommand( make_unique<NewNeuronCommand>( pos ) );
}

void NNetModelCommands::NewInputNeuron( MicroMeterPoint const & pos )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << pos << endl;
	m_pCmdStack->PushCommand( make_unique<NewInputNeuronCommand>( pos ) );
}

void NNetModelCommands::AppendNeuron( ShapeId const id )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << endl;
	m_pCmdStack->PushCommand( make_unique<AppendNeuronCommand>( id ) );
}

void NNetModelCommands::AppendInputNeuron( ShapeId const id )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << endl;
	m_pCmdStack->PushCommand( make_unique<AppendInputNeuronCommand>( id ) );
}

void NNetModelCommands::ClearBeepers( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << endl;
	m_pCmdStack->PushCommand( make_unique<ClearBeepersCommand>( ) );
}

///////////////////// selection commands /////////////////////////////

void NNetModelCommands::CopySelection( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << endl;
	m_pCmdStack->PushCommand( make_unique<CopySelectionCommand>( ) );
}

void NNetModelCommands::SelectAllBeepers( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << endl;
	m_pCmdStack->PushCommand( make_unique<SelectAllBeepersCommand>( ) );
}

void NNetModelCommands::SelectShape( ShapeId const id, tBoolOp const op )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << op << endl;
	m_pCmdStack->PushCommand( make_unique<SelectShapeCommand>( id, op ) );
}

void NNetModelCommands::SelectAll( tBoolOp const op )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << op << endl;
	m_pCmdStack->PushCommand( make_unique<SelectAllCommand>( op  ) );
}

void NNetModelCommands::SelectSubtree( ShapeId const id, tBoolOp const op )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id << L" " << op << endl;
	m_pCmdStack->PushCommand( make_unique<SelectSubtreeCommand>( id, op ) );
}

void NNetModelCommands::SelectShapesInRect( MicroMeterRect const & rect )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << rect << endl;
	m_pCmdStack->PushCommand( make_unique<SelectShapesInRectCommand>( rect ) );
}

void NNetModelCommands::AnalyzeLoops( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << endl;
	m_pCmdStack->PushCommand( make_unique<AnalyzeLoopsCommand>( ) );
}

void NNetModelCommands::AnalyzeAnomalies( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << endl;
	m_pCmdStack->PushCommand( make_unique<AnalyzeAnomaliesCommand>( ) );
}
