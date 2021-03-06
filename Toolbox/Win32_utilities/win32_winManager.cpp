// win32_winManager.cpp
//
// Win32_utilities

#include "stdafx.h"
#include <fstream>
#include "SCRIPT.H"
#include "symtab.h"
#include "UtilityWrappers.h"
#include "win32_util.h"
#include "win32_winManager.h"

using std::endl;
using std::pair;
using std::wcout;
using std::wofstream;
using std::to_wstring;

using Util::operator==;
using Util::operator!=;
using Util::operator<<; 

class WrapMoveWindow : public Script_Functor
{
public:
    WrapMoveWindow( WinManager * pWinManager ) :
        m_pWinManager( pWinManager )
    { };

    virtual void operator() ( Script & script ) const
    {
	    UINT uiResId = script.ScrReadUint();

		if ( uiResId > 0 )
		{
			PixelRect pixRect = ScrReadPixelRect( script );
			if ( m_pWinManager->IsMoveable( uiResId ) )
			{
				HWND const hwnd = m_pWinManager->GetHWND( uiResId );
				if ( m_pWinManager->IsSizeable( uiResId ) )
				{
					bool bRes = Util::MoveWindowAbsolute( hwnd, pixRect, true ); 
					assert( bRes );
				}
				else
				{
     				bool bRes = Util::MoveWindowAbsolute( hwnd, pixRect.GetStartPoint(), true ); 
					DWORD dwErr = GetLastError();
					assert( bRes );
				}
			}
		}
    }

private:
    WinManager * m_pWinManager;
};

class WrapShowWindow : public Script_Functor
{
public:
    WrapShowWindow( WinManager * pWinManager ) :
        m_pWinManager( pWinManager )
    { };

    virtual void operator() ( Script & script ) const
    {
	    UINT const uiResId  = script.ScrReadUint();
		INT const  iCmdShow = script.ScrReadInt();  // WM_HIDE, WM_SHOW, ...
		if ( uiResId > 0 )
			ShowWindow( m_pWinManager->GetHWND( uiResId ), iCmdShow );
    }

private:
    WinManager * m_pWinManager;
};

//
// read monitor configuration from script
//

struct CHECK_MON_STRUCT  // communication between WrapMonitorInfos and CheckMonitorInfo 
{
    int                                 m_iMonCounter    { 0 };
    int                                 m_iMonFromScript { 0 };
    Script                            * m_pScript        { nullptr } ;
    bool                                m_bCheckResult   { true };
    ScriptErrorHandler::ScriptException m_errorInfo      { } ;
};

static MONITORINFO ScrReadMonitorInfo( Script & script )
{
    MONITORINFO monInfo;

    monInfo.cbSize    = sizeof( MONITORINFO );
    monInfo.rcMonitor = Util::ScrReadRECT( script );
    monInfo.rcWork    = Util::ScrReadRECT( script );
    monInfo.dwFlags   = script.ScrReadUlong();

    return monInfo;
}

static bool operator != ( MONITORINFO const & a, MONITORINFO const b ) 
{ 
    return ( a.rcMonitor != b.rcMonitor) || ( a.rcWork != b.rcWork ) || ( a.dwFlags != b.dwFlags );
};

static BOOL CALLBACK CheckMonitorInfo( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
{
    CHECK_MON_STRUCT * const pMonStruct = (CHECK_MON_STRUCT *)dwData;
    bool bRes = true;
    try 
    {  
        Script * pScript = pMonStruct->m_pScript;

        pMonStruct->m_iMonFromScript = pScript->ScrReadInt();

        if ( pMonStruct->m_iMonFromScript == 0 )
        {                                        // m_script describes less monitors than current configuration  
            pMonStruct->m_bCheckResult = false;  // this is not the right configuration
            bRes = false;                        // stop monitor enumeration
        }
        else 
        {
            ++( pMonStruct->m_iMonCounter );
    
            MONITORINFO monInfoScript = ScrReadMonitorInfo( * pScript );
            MONITORINFO monInfoSystem = Util::GetMonitorInfo( hMonitor );
            if (
                  ( monInfoScript != monInfoSystem ) ||
                  ( pMonStruct->m_iMonFromScript != pMonStruct->m_iMonCounter )
               )
            {                                        // some monitor infos don't fit
                pMonStruct->m_bCheckResult = false;  // this is not the right configuration
                bRes = false;                        // stop monitor enumeration
            }
        }
    }
    catch ( ScriptErrorHandler::ScriptException const & errInfo ) // exception handling cannot pass through C function
    {                                                 
        pMonStruct->m_errorInfo = errInfo;                        // have to pass it manually
        bRes = false;
    }

    return bRes;  // true: everything ok so far, continue with monitor enumeration, false: stop monitor enumeration
}

// Syntax of monitor configuration file
// <monitor configuration file> = (0..n)<monitor configuration>
// <monitor configuration> = "MonitorConfiguration" (0..n)<monitor description> "0" <file name>
// <monitor description> = <monitor number> <rectangle> <rectangle> <monitor type>
// <monitor number> = <1..n>
// <rectangle> = (4)<integer number>
// <monitor type> = "0" | "1"

class WrapMonitorConfiguration : public Script_Functor
{
public:
    WrapMonitorConfiguration( WinManager * pWinManager ) :
        m_pWinManager( pWinManager )
    { };
    
    virtual void operator() ( Script & script ) const     // process one monitor configuration
    {
        CHECK_MON_STRUCT monStruct;
        monStruct.m_pScript = & script;

        wstring wstrFileName = script.ScrReadString();             // read window configuration file name

        EnumDisplayMonitors( nullptr, nullptr, CheckMonitorInfo, (LPARAM)&monStruct );
        if ( monStruct.m_errorInfo.m_sErrNr != 0 )                  // exception handling not passed through C function
            throw monStruct.m_errorInfo;                            // have to pass it manually

        if ( monStruct.m_iMonFromScript != 0 )                      // could be 0, if m_script describes less monitors than current configuration
        {
            monStruct.m_iMonFromScript = script.ScrReadInt();      // read monitor number 0 indicating end of monitor list

            if ( monStruct.m_iMonFromScript != 0 )                  // wasn't 0: m_script describes more monitors than current configuration
            {                           
                do                                                  // skip remaining monitor descriptions in m_script
                {
                    (void)ScrReadMonitorInfo( script );
                } while ( script.ScrReadInt() != 0 );
                monStruct.m_bCheckResult = false;                   // this is not the right configuration
            }
        }

        if ( monStruct.m_bCheckResult )
            m_pWinManager->SetWindowConfigurationFile( wstrFileName );

        m_pWinManager->IncNrOfMonitorConfigurations();
    }

private:
    WinManager * m_pWinManager;
};

bool WinManager::GetWindowConfiguration()
{
    bool bRes { false };

    Script scriptWindowConfig;
	
	if ( ! scriptWindowConfig.ScrProcess( MONITOR_CONFIG_FILE ) )
	{
		wcout << L"+++ Monitor configuration file " << MONITOR_CONFIG_FILE << L" not found or bad" << endl;
	} 
	else if ( m_strWindowConfigurationFile.empty() )
	{
		wcout << L"+++ Monitor configuration unknown" << endl;
	}
    else
    {
	    if ( ! scriptWindowConfig.ScrProcess( m_strWindowConfigurationFile ) )
	    {
            wcout << L"+++ Window configuration file " 
                  << m_strWindowConfigurationFile
                  << L" missing or bad" << endl;
	    }
	    else
	    {
            wcout << L"*** Window configuration file " 
                  << m_strWindowConfigurationFile
                  << L" sucessfully processed" << endl;
            bRes = true;
	    }
    }

    if ( ! bRes )
        wcout << L"*** Using default window positions" << endl;

    return bRes;
}

//
// writing monitor information and window coordinates
//

struct DUMP_MON_STRUCT    // communication between DumpWindowCoordinates and DumpMonitorInfo
{
    int         m_iMonCounter;
    wofstream * m_postr;
};

static BOOL CALLBACK DumpMonitorInfo( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
{
    DUMP_MON_STRUCT * const pMonStruct = (DUMP_MON_STRUCT *)dwData;
    wofstream       * const postr      = pMonStruct->m_postr;
    MONITORINFO       const monInfo    = Util::GetMonitorInfo( hMonitor );

    ++( pMonStruct->m_iMonCounter );

    * postr << pMonStruct->m_iMonCounter << L"   # monitor number"      << endl;
    * postr << monInfo.rcMonitor         << L"   # monitor rectangle"   << endl;
    * postr << monInfo.rcWork            << L"   # work area rectangle" << endl;
    * postr << (
                ( monInfo.dwFlags == 0 ) 
                ? L"0    # secondary monitor" 
                : L"1    # primary monitor"
             ) << endl;

    return true;
}

void WinManager::dumpMonitorConfiguration() const
{
    wofstream ostr( MONITOR_CONFIG_FILE, wofstream::app );
	
	DUMP_MON_STRUCT monStruct = { 0, & ostr };

    ostr << L"MonitorConfiguration  \"" << m_strWindowConfigurationFile << L"\"" << endl;
    EnumDisplayMonitors( nullptr, nullptr, DumpMonitorInfo, (LPARAM)&monStruct );
    ostr << L"0   # end of MonitorConfiguration" << endl << endl;

    ostr.close();
}

void WinManager::dumpWindowCoordinates() const
{
    wofstream ostr( m_strWindowConfigurationFile, wofstream::out );
    
    for ( const auto & it : m_map )
	{
		if ( it.second.m_bTrackPosition )
		{
			HWND hwnd = it.second.m_hwnd;
			if ( hwnd != nullptr )
			{
				ostr << L"MoveWindow "
					 << it.second.m_wstr << L" "
					 << Util::GetWindowLeftPos( hwnd ).GetValue() << L" "
					 << Util::GetWindowTop    ( hwnd ).GetValue() << L" "
				     << Util::GetWindowWidth  ( hwnd ).GetValue() << L" "
				     << Util::GetWindowHeight ( hwnd ).GetValue() << endl;

				ostr << L"ShowWindow " 
					<< it.second.m_wstr << L" "
					<< ( IsWindowVisible( hwnd ) 
						? ( IsZoomed( hwnd ) ? L"SW_MAXIMIZE" : L"SW_SHOWNORMAL" )
						: L"SW_HIDE" ) << endl;
			}
		}
	}

    ostr.close();
}

void WinManager::StoreWindowConfiguration()
{
    if ( m_strWindowConfigurationFile.empty() )
    {
        m_strWindowConfigurationFile = WINDOW_CONFIG_FILE_STUB + to_wstring( ++m_iNrOfMonitorConfigurations ) + L".cnf";

        dumpMonitorConfiguration();
    }

    dumpWindowCoordinates();    // Write window configuration to window configuration file
}

WinManager::WinManager() :
    m_strWindowConfigurationFile( L"" ),
    m_iNrOfMonitorConfigurations( 0 )
{
    #define DEF_WINMAN_FUNC(name) SymbolTable::ScrDefConst( L#name, new Wrap##name##( this ) )

    DEF_WINMAN_FUNC( MoveWindow );
    DEF_WINMAN_FUNC( ShowWindow );
    DEF_WINMAN_FUNC( MonitorConfiguration );

    DEF_ULONG_CONST( SW_RESTORE );
    DEF_ULONG_CONST( SW_SHOWNORMAL );
    DEF_ULONG_CONST( SW_MAXIMIZE );
    DEF_ULONG_CONST( SW_SHOW );
    DEF_ULONG_CONST( SW_HIDE );

    #undef DEF_WINMAN_FUNC
}

void WinManager::addWindow
( 
    wstring    const         wstrName, 
    UINT       const         id, 
    HWND       const         hwnd, 
    BaseWindow const * const pBaseWindow,
    bool       const         bTrackPosition,
    bool       const         bTrackSize
)
{
    if ( id != 0 )
    {
        m_map.insert( pair< UINT, MAP_ELEMENT >( id, { wstrName, pBaseWindow, hwnd, bTrackPosition, bTrackSize } ) );
        SymbolTable::ScrDefConst( wstrName, static_cast<ULONG>(id) );
    }
}

void WinManager::AddWindow
( 
    wstring const wstrName, 
    UINT    const id, 
    HWND    const hwnd, 
	bool    const bTrackPosition,
	bool    const bTrackSize
)
{
	if ( hwnd  )
        addWindow( wstrName, id, hwnd, nullptr, bTrackPosition, bTrackSize );
}

void WinManager::AddWindow
( 
	wstring    const   wstrName, 
	UINT       const   id, 
	BaseWindow const & baseWindow,
	bool       const   bTrackPosition,
	bool       const   bTrackSize
)
{
    addWindow( wstrName, id, baseWindow.GetWindowHandle(), & baseWindow, bTrackPosition, bTrackSize );
}

void WinManager::AddWindow
( 
	wstring    const   wstrName, 
	UINT       const   id, 
	BaseDialog const & baseDialog,
	bool       const   bTrackPosition,
	bool       const   bTrackSize
)
{
    addWindow( wstrName, id, baseDialog.GetWindowHandle(), nullptr, bTrackPosition, bTrackSize );
}
