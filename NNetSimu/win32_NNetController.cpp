// win32_NNetController.cpp
//
// NNetSimu

#include "stdafx.h"
#include <unordered_map>
#include <chrono>
#include "Windowsx.h"
#include "Windows.h"
#include "Resource.h"
#include "BoolOp.h"
#include "LogarithmicTrackBar.h"
#include "SlowMotionRatio.h"
#include "win32_util.h"
#include "win32_simulationControl.h"
#include "win32_zoomControl.h"
#include "win32_aboutBox.h"
#include "win32_NNetAppWindow.h"
#include "win32_NNetWindow.h"
#include "win32_NNetEditor.h"
#include "win32_NNetWorkThreadInterface.h"
#include "win32_NNetController.h"

using std::unordered_map;

NNetController::NNetController
(
	NNetAppWindow           * const pAppWindow,
	NNetWindow              * const pNNetWindow,
	NNetEditorWindow        * const pNNetEditorWindow,
	WinManager              * const pWinManager,
	StatusBar               * const pStatusBar,
	NNetWorkThreadInterface * const pNNetWorkThreadInterface,
	SlowMotionRatio         * const pSlowMotionRatio
) 
  :	m_pAppWindow              ( nullptr ),
	m_pNNetWindow             ( pNNetWindow ),
	m_pNNetEditorWindow       ( pNNetEditorWindow ),
	m_pWinManager             ( pWinManager ),
	m_pStatusBar              ( pStatusBar ),
	m_pNNetWorkThreadInterface( pNNetWorkThreadInterface ),
	m_pSlowMotionRatio        ( pSlowMotionRatio ),
	m_hCrsrWait               ( LoadCursor( NULL, IDC_WAIT ) )
{
}

NNetController::~NNetController( )
{
	m_pNNetWorkThreadInterface = nullptr;
	m_pAppWindow               = nullptr;
	m_pWinManager              = nullptr;
	m_pSlowMotionRatio         = nullptr;
    m_pStatusBar               = nullptr;
}

bool NNetController::ProcessUIcommand( int const wmId, LPARAM const lParam )
{
	switch (wmId)
	{

	case IDM_CRSR_WINDOW:
		SendMessage( m_pWinManager->GetHWND( wmId ), WM_COMMAND, IDM_WINDOW_ON, 0 );
		break;

	case IDM_TRACKBAR:
		switch ( lParam )
		{
		case IDM_ZOOM_TRACKBAR:
		{
			LONG const lLogicalPos = m_pStatusBar->GetTrackBarPos( IDM_ZOOM_TRACKBAR );
			LONG const lValue      = LogarithmicTrackbar::Value2TrackbarL( CastToLong( MAXIMUM_PIXEL_SIZE.GetValue()) ) - lLogicalPos;
			LONG const lPos        = LogarithmicTrackbar::TrackBar2ValueL( lValue );
			ProcessUIcommand( IDM_ZOOM_TRACKBAR, lPos );
		}
		break;

		default:
			assert( false );
		}
		break;

	case IDM_ZOOM_TRACKBAR:  // comes from trackbar in statusBar
		m_pNNetWindow->SetPixelSize( MicroMeter((float &)lParam) );
		break;

	case IDM_FIT_ZOOM:
		//m_pNNetWindow->Fit2Rect( );
		//setSizeTrackBar( m_pNNetWindow->GetPixelSize() );
		break;

	case IDM_ZOOM_OUT:
	case IDM_ZOOM_IN:
		m_pNNetWindow->Zoom( wmId == IDM_ZOOM_IN );
		setSizeTrackBar( m_pNNetWindow->GetPixelSize() );
		break;

	case IDM_SET_ZOOM:
		m_pNNetWindow->SetPixelSize( MicroMeter((float &)lParam) );
		setSizeTrackBar( MicroMeter((float &)lParam));
		break;

	case IDM_REFRESH:
		m_pNNetWindow->Notify( lParam != 0 );
		break;

	case IDD_PULSE_RATE:
	case IDD_PULSE_SPEED:
	case IDD_PULSE_WIDTH:
	case IDD_DAMPING_FACTOR:
	case IDD_THRESHOLD:
	case IDD_PEAK_VOLTAGE:
	case IDD_REFRACTORY_PERIOD:
		{
			static unordered_map < int, tParameter const > mapParam
			{
				{ IDD_PULSE_RATE,        tParameter::pulseRate        },
				{ IDD_PULSE_SPEED,       tParameter::pulseSpeed       },
				{ IDD_PULSE_WIDTH,       tParameter::pulseWidth       },
				{ IDD_DAMPING_FACTOR,    tParameter::dampingFactor    },
				{ IDD_THRESHOLD,         tParameter::threshold        },
				{ IDD_PEAK_VOLTAGE,      tParameter::peakVoltage      },
				{ IDD_REFRACTORY_PERIOD, tParameter::refractoryPeriod }
			};				  

			tParameter param     { mapParam.at( wmId ) };
			float      fNewValue { m_pNNetWindow->ParameterDialog( param ) };
			m_pNNetWorkThreadInterface->PostSetParameter
			( 
				param, 
				fNewValue, 
				m_pNNetWindow->GetShapeSelected( ) 
			);
		}
		break;

	default:
		return FALSE; // command has not been processed
	}

	return TRUE;  // command has been processed
}

bool NNetController::ProcessModelCommand( int const wmId, LPARAM const lParam )
{
	switch ( wmId )
	{
	case IDM_RUN:
		m_pNNetWorkThreadInterface->PostResetTimer( );
		return true;

	case IDD_TRIGGER:
		m_pNNetWorkThreadInterface->PostTrigger( ShapeId( CastToLong( lParam ) ) );
		break;

	case IDD_CONNECT:
		m_pNNetWorkThreadInterface->PostConnect( );
		break;

	case IDM_HIGHLIGHT:
		m_pNNetWorkThreadInterface->PostHighlight( ShapeId( CastToLong( lParam ) ) );
		break;

	case IDM_SUPER_HIGHLIGHT:
		m_pNNetWorkThreadInterface->PostSuperHighlight( ShapeId( CastToLong( lParam ) ) );
		break;

	case IDM_SLOWER:
		if ( m_pSlowMotionRatio->IncRatio( ) )
			m_pNNetWorkThreadInterface->PostSlowMotionChanged( );
		else
			MessageBeep( MB_ICONWARNING );
		break;

	case IDM_FASTER:
		if ( m_pSlowMotionRatio->DecRatio( ) )
			m_pNNetWorkThreadInterface->PostSlowMotionChanged( );
		else
			MessageBeep( MB_ICONWARNING );
		break;

	case IDD_CREATE_NEW_BRANCH:
		m_pNNetWorkThreadInterface->PostCreateNewBranch( m_pNNetWindow->GetShapeSelected() );
		break;

	case IDD_CREATE_NEW_NEURON:
	{
		MicroMeterPoint pnt = Util::Unpack2MicroMeterPoint( lParam );
		m_pNNetWorkThreadInterface->PostCreateNewNeuron( pnt );
	}
	break;

	case IDD_SPLIT_PIPELINE:
	{
		MicroMeterPoint pnt = Util::Unpack2MicroMeterPoint( lParam );
		m_pNNetWorkThreadInterface->PostSplitPipeline( m_pNNetWindow->GetShapeSelected(), pnt );
	}
	break;

	case IDD_CREATE_NEW_INPUT_NEURON:
	{
		MicroMeterPoint pnt = Util::Unpack2MicroMeterPoint( lParam );
		m_pNNetWorkThreadInterface->PostCreateNewInputNeuron( pnt );
	}
	break;

	case IDD_CREATE_NEW_OUTPUT_NEURON:
	{
		MicroMeterPoint pnt = Util::Unpack2MicroMeterPoint( lParam );
		m_pNNetWorkThreadInterface->PostCreateNewOutputNeuron( pnt );
	}
	break;

	default:
		return true;
	}

	return false;
}

void NNetController::setSizeTrackBar( MicroMeter const pixelSize )
{ 
	m_pStatusBar->SetTrackBarPos( IDM_ZOOM_TRACKBAR, CastToLong( LogarithmicTrackbar::Value2TrackbarD( pixelSize.GetValue() ) ) ); 
}