// win32_NNetWorkThreadInterface.cpp
//
// NNetWindows

#include "stdafx.h"
#include "assert.h"
#include <unordered_map>
#include "Resource.h"
#include "PixelTypes.h"
#include "win32_util.h"
#include "win32_script.h"
#include "win32_NNetWorkThread.h"
#include "win32_NNetWorkThreadInterface.h"

using std::wostream;
using std::wcout;
using std::endl;
using std::unordered_map;

NNetWorkThreadInterface::NNetWorkThreadInterface( ) :
	m_pTraceStream( nullptr ),
	m_bTrace      ( TRUE ),
	m_pNNetWorkThread( nullptr ),
	m_pModel( nullptr )
{ }

NNetWorkThreadInterface::~NNetWorkThreadInterface( )
{
	m_pNNetWorkThread = nullptr;
	m_pTraceStream = nullptr;
}

void NNetWorkThreadInterface::Initialize( wostream * pTraceStream ) 
{ 
	m_pTraceStream = pTraceStream;
}

void NNetWorkThreadInterface::Start
( 
	HWND                 const hwndApplication,
	ActionTimer        * const pActionTimer,
    EventInterface     * const pEvent,
	ObserverInterface  * const pRedrawObserver,
	ObserverInterface  * const pChangeObserver,
	SlowMotionRatio    * const pSlowMotionRatio,
	NNetModel          * const pModel,
	Param              * const pParam,
	NNetModelStorage   * const pStorage,
	BOOL                 const bAsync
)
{
	m_pModel = pModel;
	m_pNNetWorkThread = new NNetWorkThread
	( 
		hwndApplication, 
		pActionTimer,
		pEvent, 
		pRedrawObserver,
		pChangeObserver,
		pSlowMotionRatio,
		this,
		pModel,
		pParam,
		pStorage,
		bAsync
	);
}

void NNetWorkThreadInterface::Stop( )
{
	m_pNNetWorkThread->Terminate( );
	delete m_pNNetWorkThread;
	m_pNNetWorkThread = nullptr;
}

void NNetWorkThreadInterface::PostResetTimer( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::RESET_TIMER ), 0, 0 );
}

void NNetWorkThreadInterface::PostConnect( ShapeId const idSrc, ShapeId const idDst )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << idSrc.GetValue() << L" " << idDst.GetValue() << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::CONNECT ), idSrc.GetValue(), idDst.GetValue() );
}

void NNetWorkThreadInterface::PostRemoveShape( ShapeId const id )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id.GetValue() << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::REMOVE_SHAPE ), id.GetValue(), 0 );
}

void NNetWorkThreadInterface::PostDisconnect( ShapeId const id )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id.GetValue() << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::DISCONNECT ), id.GetValue(), 0 );
}

void NNetWorkThreadInterface::PostConvert2Neuron( ShapeId const id )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id.GetValue() << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::CONVERT2NEURON ), id.GetValue(), 0 );
}

void NNetWorkThreadInterface::PostConvert2InputNeuron( ShapeId const id )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id.GetValue() << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::CONVERT2INPUT_NEURON ), id.GetValue(), 0 );
}

void NNetWorkThreadInterface::PostResetModel( )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::RESET_MODEL ), 0, 0 );
}

void NNetWorkThreadInterface::PostSetPulseRate( ShapeId const id, float const fNewValue )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id.GetValue() << L" " << fNewValue << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::PULSE_RATE ), id.GetValue(), (LPARAM &)fNewValue );
}

void NNetWorkThreadInterface::PostSetTriggerSound( ShapeId const id, bool const bActive, Hertz const freq, MilliSecs const ms )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id.GetValue() << L" " << bActive << L" " << freq << L" " << ms << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::TRIGGER_SOUND ), id.GetValue(), Util::Pack2UINT64( freq.GetValue(), ms.GetValue() ) );
}

void NNetWorkThreadInterface::PostSetParameter( tParameter const param, float const fNewValue )
{
	static unordered_map < tParameter, NNetWorkThreadMessage::Id const > mapParam =
	{
		{ tParameter::pulseSpeed,     NNetWorkThreadMessage::Id::PULSE_SPEED       },
		{ tParameter::pulseWidth,     NNetWorkThreadMessage::Id::PULSE_WIDTH       },
		{ tParameter::threshold,      NNetWorkThreadMessage::Id::THRESHOLD         },
		{ tParameter::peakVoltage,    NNetWorkThreadMessage::Id::PEAK_VOLTAGE      },
    	{ tParameter::refractPeriod,  NNetWorkThreadMessage::Id::REFRACTORY_PERIOD },
	    { tParameter::timeResolution, NNetWorkThreadMessage::Id::TIME_RESOLUTION   }
	};				  

	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << GetParameterName( param ) << L" " << fNewValue << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( mapParam.at( param ) ), 0, (LPARAM &)fNewValue );
}

void NNetWorkThreadInterface::PostMoveShape( ShapeId const id, MicroMeterPoint const & delta )
{
	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << id.GetValue() << L" " << delta << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::MOVE_SHAPE ), id.GetValue(), Util::Pack2UINT64(delta) );
}

void NNetWorkThreadInterface::PostSlowMotionChanged( )
{
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::SLOW_MOTION_CHANGED ), 0, 0 );
}

wchar_t const * NNetWorkThreadInterface::GetActionCommandName( int const iMsgId ) const 
{
	static unordered_map < int, wchar_t const * > mapMsgName =
	{
		{ IDM_ANALYZE_LOOPS,       L"ANALYZE_LOOPS"       },
		{ IDM_ANALYZE_ANOMALIES,   L"ANALYZE_ANOMALIES"   },
		{ IDM_REMOVE_SELECTION,    L"REMOVE_SELECTION"    },
		{ IDM_MARK_SELECTION,      L"MARK_SELECTION"      },
		{ IDM_COPY_SELECTION,      L"COPY_SELECTION"      },
		{ IDD_INSERT_NEURON,       L"INSERT_NEURON"       },
		{ IDD_NEW_NEURON,          L"NEW_NEURON"          },
		{ IDD_NEW_INPUT_NEURON,    L"NEW_INPUT_NEURON"    },
		{ IDD_APPEND_NEURON,       L"APPEND_NEURON"       },
		{ IDD_APPEND_INPUT_NEURON, L"APPEND_INPUT_NEURON" },
		{ IDD_ADD_OUTGOING2KNOT,   L"ADD_OUTGOING2KNOT"   },
		{ IDD_ADD_INCOMING2KNOT,   L"ADD_INCOMING2KNOT"   },
		{ IDD_ADD_OUTGOING2PIPE,   L"ADD_OUTGOING2PIPE"   },
		{ IDD_ADD_INCOMING2PIPE,   L"ADD_INCOMING2PIPE"   }
	};

	return mapMsgName.at( iMsgId );
}

int const NNetWorkThreadInterface::GetActionCommandFromName( wchar_t const * const name ) const 
{
	static unordered_map < wchar_t const *, int > mapMsg =
	{
		{ L"ANALYZE_LOOPS",       IDM_ANALYZE_LOOPS       },
		{ L"ANALYZE_ANOMALIES",   IDM_ANALYZE_ANOMALIES   },
		{ L"REMOVE_SELECTION",    IDM_REMOVE_SELECTION    },
		{ L"MARK_SELECTION",      IDM_MARK_SELECTION      },
		{ L"COPY_SELECTION",      IDM_COPY_SELECTION      },
		{ L"INSERT_NEURON",       IDD_INSERT_NEURON       },
		{ L"NEW_NEURON",          IDD_NEW_NEURON          },
		{ L"NEW_INPUT_NEURON",    IDD_NEW_INPUT_NEURON    },
		{ L"APPEND_NEURON",       IDD_APPEND_NEURON       },
		{ L"APPEND_INPUT_NEURON", IDD_APPEND_INPUT_NEURON },
		{ L"ADD_OUTGOING2KNOT",   IDD_ADD_OUTGOING2KNOT   },
		{ L"ADD_INCOMING2KNOT",   IDD_ADD_INCOMING2KNOT   },
		{ L"ADD_OUTGOING2PIPE",   IDD_ADD_OUTGOING2PIPE   },
		{ L"ADD_INCOMING2PIPE",   IDD_ADD_INCOMING2PIPE   }
	};

	return mapMsg.at( name );
}

void NNetWorkThreadInterface::PostActionCommand( int const idMsg, ShapeId const idShape, MicroMeterPoint const & umPos )
{
	static unordered_map < int, NNetWorkThreadMessage::Id const > mapMsg =
	{
		{ IDM_ANALYZE_LOOPS,       NNetWorkThreadMessage::Id::ANALYZE_LOOPS       },
		{ IDM_ANALYZE_ANOMALIES,   NNetWorkThreadMessage::Id::ANALYZE_ANOMALIES   },
		{ IDM_REMOVE_SELECTION,    NNetWorkThreadMessage::Id::REMOVE_SELECTION    },
		{ IDM_MARK_SELECTION,      NNetWorkThreadMessage::Id::MARK_SELECTION      },
		{ IDM_COPY_SELECTION,      NNetWorkThreadMessage::Id::COPY_SELECTION      },
		{ IDD_INSERT_NEURON,       NNetWorkThreadMessage::Id::INSERT_NEURON       },
		{ IDD_NEW_NEURON,          NNetWorkThreadMessage::Id::NEW_NEURON          },
		{ IDD_NEW_INPUT_NEURON,    NNetWorkThreadMessage::Id::NEW_INPUT_NEURON    },
		{ IDD_APPEND_NEURON,       NNetWorkThreadMessage::Id::APPEND_NEURON       },
		{ IDD_APPEND_INPUT_NEURON, NNetWorkThreadMessage::Id::APPEND_INPUT_NEURON },
		{ IDD_ADD_OUTGOING2KNOT,   NNetWorkThreadMessage::Id::ADD_OUTGOING2KNOT   },
		{ IDD_ADD_INCOMING2KNOT,   NNetWorkThreadMessage::Id::ADD_INCOMING2KNOT   },
		{ IDD_ADD_OUTGOING2PIPE,   NNetWorkThreadMessage::Id::ADD_OUTGOING2PIPE   },
		{ IDD_ADD_INCOMING2PIPE,   NNetWorkThreadMessage::Id::ADD_INCOMING2PIPE   }
	};				  

	if ( IsTraceOn( ) )
		TraceStream( ) << __func__ << L" " << GetActionCommandName( idMsg ) << L" " << idShape.GetValue( ) << umPos << endl;
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( mapMsg.at( idMsg ) ), idShape.GetValue( ), Util::Pack2UINT64( umPos ) );
}

void NNetWorkThreadInterface::PostGenerationStep( )
{
	m_pNNetWorkThread->Continue( );     // trigger worker thread if waiting on POI event
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::NEXT_GENERATION ), 0, 0 );
}

void NNetWorkThreadInterface::PostRunGenerations( bool const bFirst )
{
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::GENERATION_RUN ), 0, bFirst );
}

void NNetWorkThreadInterface::PostRepeatGenerationStep( )
{
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::REPEAT_NEXT_GENERATION ), 0, 0 );
}

void NNetWorkThreadInterface::PostStopComputation( )
{
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::STOP ), 0, 0 );
}

void NNetWorkThreadInterface::PostSendBack( int const iMsg )
{
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::SEND_BACK ), iMsg, 0 );
}

void NNetWorkThreadInterface::PostDeleteSelection()
{
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::REMOVE_SELECTION), 0, 0 );
}

void NNetWorkThreadInterface::PostMarkSelection( tBoolOp const op )
{
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::MARK_SELECTION), 0, static_cast<LPARAM>(op) );
}

void NNetWorkThreadInterface::PostCopySelection()
{
	m_pNNetWorkThread->PostThreadMsg( static_cast<UINT>( NNetWorkThreadMessage::Id::COPY_SELECTION), 0, 0 );
}
