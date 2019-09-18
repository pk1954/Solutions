// win32_evoController.h
//

#pragma once

#include "win32_controller.h"

class AppWindow;
class Delay;
class StatusBar;
class EvoWorkThreadInterface;
class EvoHistorySysGlue;
class WinManager;
class GridWindow;
class EditorWindow;
class ColorManager;

class EvoController : public Controller
{
public:
	EvoController
	(
		WinManager        * const,
		EvoHistorySysGlue * const,
		Delay             * const,
		ColorManager      * const,
		StatusBar         * const,
		GridWindow        * const,
		EditorWindow      * const
	);

	virtual ~EvoController( );

	void EvoController::Initialize
	( 
		AppWindow              * const,
		EvoWorkThreadInterface * const
	);

	virtual void ProcessAppCommand( WPARAM const, LPARAM const = 0 );

private:
	void scriptDialog( );
	bool processUIcommand( int const, LPARAM const );

	AppWindow              * m_pAppWindow;
	EvoWorkThreadInterface * m_pEvoWorkThreadInterface;
	WinManager             * m_pWinManager;
	EvoHistorySysGlue      * m_pEvoHistGlue;
	Delay                  * m_pDelay;
	StatusBar              * m_pStatusBar;
	GridWindow             * m_pGridWindow;
	EditorWindow           * m_pEditorWindow;
	ColorManager           * m_pColorManager;
	HCURSOR                  m_hCrsrWait;
};
