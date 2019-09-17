// win32_NNetWorkThread.h
//
// NNetWindows

#pragma once

#include "HistoryGeneration.h"
#include "win32_WorkThread.h"
#include "NNetGenerationCmd.h"

class Delay;
class ActionTimer;
class RootWindow;
class WinManager;
class EventInterface;
class NNetHistorySysGlue;
class NNetWorkThreadInterface;

class NNetWorkThreadMessage
{
public:
	enum class Id : UINT
	{
		NNET_FIRST = WorkThreadMessage::FIRST_APP_MESSAGE,
		NNET_LAST,
		FIRST = NNET_FIRST,
		LAST = NNET_LAST
	};

	static BOOL IsValid( NNetWorkThreadMessage::Id msg )
	{
		return (NNetWorkThreadMessage::Id::FIRST <= msg) && (msg <= NNetWorkThreadMessage::Id::LAST);
	}
};

class NNetWorkThread: public WorkThread
{
public:
	NNetWorkThread
	( 
		HWND                      const,
		ActionTimer             * const,
		EventInterface          * const,
		Delay                   * const,
		ObserverInterface       * const,
		NNetHistorySysGlue      * const,
		NNetWorkThreadInterface * const
	);
	~NNetWorkThread( );

private:

	GenerationCmd NNetCmd( NNetGenerationCmd::Id const cmd, Int24 const param )
	{ 
		return GenerationCmd::ApplicationCmd( static_cast<GenerationCmd::Id>(cmd), param );  
	}  

	void editorCommand( NNetGenerationCmd::Id const cmd, WPARAM const wParam )
	{
		EditorCommand( static_cast<GenerationCmd::Id>(cmd), wParam );
	}

	virtual BOOL Dispatch( MSG const );
};