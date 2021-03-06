// win32_crsrWindow.cpp :
//
// NNetWindows

#include "stdafx.h"
#include <iostream>
#include <sstream> 
#include <string> 
#include <iomanip>
#include "NNetModelReaderInterface.h"
#include "Knot.h"
#include "Neuron.h"
#include "InputNeuron.h"
#include "Pipe.h"
#include "win32_util.h"
#include "win32_textBuffer.h"
#include "win32_MainWindow.h"
#include "win32_crsrWindow.h"

using std::setprecision;

CrsrWindow::CrsrWindow() :
	TextWindow()
{}

CrsrWindow::~CrsrWindow()
{
	m_pMainWindow = nullptr;
}

void CrsrWindow::Start
(
	HWND                     const         hwndParent,
	MainWindow               const * const pNNetWindow,
	NNetModelReaderInterface const * const pModelInterface
) 
{
	m_pMainWindow           = pNNetWindow;
	m_pNMRI = pModelInterface;
	StartTextWindow
	(
		hwndParent, 
		PixelRect { 0_PIXEL, 300_PIXEL, 300_PIXEL, 500_PIXEL }, 
		L"CrsrWindow", 
		100, 
		true,
		nullptr
	);
//	::CreateWindowToolTip( GetWindowHandle(), L"Cursor window" );
}

void CrsrWindow::Stop()
{
	TextWindow::StopTextWindow();
	Show( false );
}

void CrsrWindow::printMicroMeter
( 
	TextBuffer     & textBuf,
	MicroMeter const um 
)
{
	wostringstream wBuffer;
	MicroMeter     umAbs { abs( um.GetValue() ) };
	wBuffer << std::fixed << setprecision(1);
	if ( umAbs < 1000.0_MicroMeter )
		wBuffer << um.GetValue() << L" um ";
	else if ( umAbs < 1000000.0_MicroMeter )
		wBuffer << um.GetValue() / 1000.0f << L" mm ";
	else
		wBuffer << um.GetValue() / 1000000.0f << L" m ";
	textBuf.printString( wBuffer.str() );
}

void CrsrWindow::printMilliSecs( TextBuffer & textBuf, MilliSecs const msec )
{
	wostringstream wBuffer;
	wBuffer << msec.GetValue() << L" msec";
	textBuf.printString( wBuffer.str() );
}

void CrsrWindow::DoPaint( TextBuffer & textBuf )
{
	MicroMeterPoint const umPoint { m_pMainWindow->GetCursorPos() };
	if ( umPoint == NP_ZERO )
	{
		textBuf.AlignLeft();
		textBuf.printString( L"Cursor not in model window" );
		return;
	}

	textBuf.printString( L"Position: " );
	printMicroMeter( textBuf, umPoint.GetX() );
	printMicroMeter( textBuf, umPoint.GetY() );
	textBuf.nextLine();

	ShapeId const id { m_pMainWindow->GetHighlightedShapeId() };

	if ( IsUndefined(id) )
		return;

	ShapeType const type { m_pNMRI->GetShapeType( id ) };

	textBuf.AlignRight(); textBuf.printString( L"Shape #" );
	textBuf.AlignLeft();  textBuf.printNumber( id.GetValue() );
	if ( m_pNMRI->IsSelected(id) )
		textBuf.printString( L" selected" );
	textBuf.nextLine();
	textBuf.AlignRight(); textBuf.printString( L"type:" ); 
	textBuf.AlignLeft();  textBuf.printString( ShapeType::GetName( type.GetValue() ) ); 
	textBuf.nextLine();

	mV potential { 0.0_mV };
	if ( type.IsPipeType() )
	{
		textBuf.AlignRight(); textBuf.printString( L"# segments:" );
		textBuf.AlignLeft();  textBuf.printNumber( m_pNMRI->GetNrOfSegments( id ) );
		potential = m_pNMRI->GetVoltage( id, umPoint );
	}
	else 
	{
		potential = m_pNMRI->GetVoltage( id );
	}

	textBuf.AlignRight(); textBuf.nextLine( L"potential " );
	textBuf.AlignLeft();  textBuf.printFloat( potential.GetValue() );
	                      textBuf.printString( L" mV" );
	textBuf.nextLine();

	if ( type.IsAnyNeuronType() )
	{
		SoundDescr sound { m_pNMRI->GetTriggerSound( id ) };
		if ( sound.m_bOn )
		{
			textBuf.AlignRight(); textBuf.nextLine( L"trigger sound:" );
			textBuf.AlignLeft();  printFrequency( textBuf, sound.m_frequency );
			                      printMilliSecs( textBuf, sound.m_duration );
			                      textBuf.printString( L" msec" );
			textBuf.nextLine();
		}
		if ( type.IsInputNeuronType() )
		{
			textBuf.AlignRight(); textBuf.printString( L"pulse freq: " );
			textBuf.AlignLeft();  printFrequency( textBuf, m_pNMRI->GetPulseFrequency( id ) );
			textBuf.nextLine();
		}
	}
}
