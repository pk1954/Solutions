// win32_NNetSimuMenu.h
//
// NNetSimuMenu

#pragma once

#include "win32_appMenu.h"

class NNetSimuMenu : public AppMenu
{
public:
	NNetSimuMenu() :
		m_hMenu( nullptr ),
		m_pWorkThreadInterface( nullptr ),
		m_pWinManager( nullptr )
	{}

	virtual ~NNetSimuMenu() {}

	virtual void Initialize( HWND const, WorkThreadInterface const * const, WinManager const * const );
	virtual void AdjustVisibility( );
	virtual void Start( );
	virtual void Stop( );

private:
	void enableMenues( UINT const );

	HMENU                       m_hMenu;
	WorkThreadInterface const * m_pWorkThreadInterface;
	WinManager          const * m_pWinManager;
};