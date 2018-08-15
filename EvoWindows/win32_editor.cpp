// win32_editor.cpp 
//

#include "stdafx.h"
#include <unordered_map>
#include "commctrl.h"
#include "Resource.h"
#include "config.h"
#include "EvolutionModelData.h"
#include "win32_util.h"
#include "win32_status.h"
#include "win32_workThreadInterface.h"
#include "win32_displayOptions.h"
#include "win32_editor.h"

using namespace std;

EditorWindow::EditorWindow( )
  : BaseDialog( ),
    m_pModelWork          ( nullptr ),
    m_pWorkThreadInterface( nullptr ),
    m_pDspOptWindow       ( nullptr ),
    m_pStatusBar          ( nullptr )
{ }

void EditorWindow::Start
(  
    HWND                  const hWndParent,
    WorkThreadInterface * const pWorkThreadInterface,
    EvolutionModelData  * const pModel,
    DspOptWindow        * const pDspOptWindow,
	StatusBar           * const pStatusBar
)
{
    StartBaseDialog( hWndParent, MAKEINTRESOURCE( IDD_EDITOR ) );

    m_pWorkThreadInterface = pWorkThreadInterface;
    m_pModelWork           = pModel;
    m_pDspOptWindow        = pDspOptWindow;
	m_pStatusBar           = pStatusBar;

    SetTrackBarRange( IDM_EDIT_SIZE,         1,  50 );
    SetTrackBarRange( IDM_EDIT_INTENSITY, -100, 100 );
    UpdateEditControls( );
}

EditorWindow::~EditorWindow( )
{
    m_pDspOptWindow        = nullptr;
    m_pWorkThreadInterface = nullptr;
    m_pModelWork           = nullptr;
    m_pStatusBar           = nullptr;
}

LRESULT EditorWindow::sendClick( int const item ) const
{
    HWND    const hwndOld = SetActiveWindow( GetWindowHandle( ) );
    LRESULT const res = SendDlgItemMessage( item, BM_CLICK, 0, 0 );
    (void)SetActiveWindow( hwndOld );
    return res;
}

void EditorWindow::UpdateEditControls( ) // Set state of all window widgets according to mode (edit/simu)
{
	static unordered_map < tBrushMode, WORD > mapModeTable =
	{
		{ tBrushMode::move,           IDM_MOVE            },
		{ tBrushMode::randomStrategy, IDM_RANDOM_STRATEGY },
		{ tBrushMode::cooperate,      IDM_COOPERATE       },
		{ tBrushMode::defect,         IDM_DEFECT          },
		{ tBrushMode::tit4tat,        IDM_TIT4TAT         },
		{ tBrushMode::noAnimals,      IDM_KILL_ANIMALS    },
		{ tBrushMode::mutRate,        IDM_MUT_RATE        },
		{ tBrushMode::fertility,      IDM_FERTILITY       },
		{ tBrushMode::food,           IDM_FOOD_STOCK      },
		{ tBrushMode::fertilizer,     IDM_FERTILIZER      }
	};
    
	CheckRadioButton( IDM_MOVE, IDM_FOOD_STOCK, mapModeTable.at( m_pModelWork->GetBrushMode () ) );

	static unordered_map < tShape, WORD > mapShapeTable =
	{
		{ tShape::Circle, IDM_EDIT_CIRCLE    },    
		{ tShape::Rect,   IDM_EDIT_RECTANGLE }
	};

	CheckRadioButton( IDM_EDIT_CIRCLE, IDM_EDIT_RECTANGLE, mapShapeTable.at( m_pModelWork->GetBrushShape() ) );

	SetTrackBarPos( IDM_EDIT_SIZE,      static_cast<long>( m_pModelWork->GetBrushSize( )) );
    SetTrackBarPos( IDM_EDIT_INTENSITY, static_cast<long>( m_pModelWork->GetBrushIntensity( )) );

	// adjust display options window

	m_pDspOptWindow->UpdateDspOptionsControls( m_pModelWork->GetBrushMode () );

	SetSimulationMode();
}

void EditorWindow::SetSimulationMode()  	// adjust window configuration according to simulation or edit mode
{
	bool bSimulationMode = m_pModelWork->GetSimulationMode( );

	m_pStatusBar->SetSimuMode( bSimulationMode );

	if ( bSimulationMode )
		sendClick( IDM_MOVE );

	Show( ! bSimulationMode );

	PostCommand2Application( IDM_SHOW_PERF_WINDOW, static_cast<LPARAM>(bSimulationMode) );
}

void EditorWindow::setBrushMode( WORD const wId ) const
{
	static unordered_map < WORD, tBrushMode > mapModeTable =
	{
		{ IDM_MOVE,            tBrushMode::move            },
		{ IDM_RANDOM_STRATEGY, tBrushMode::randomStrategy  },
		{ IDM_COOPERATE,       tBrushMode::cooperate       },
		{ IDM_DEFECT,          tBrushMode::defect          },
		{ IDM_TIT4TAT,         tBrushMode::tit4tat         },
		{ IDM_KILL_ANIMALS,    tBrushMode::noAnimals       },
		{ IDM_MUT_RATE,        tBrushMode::mutRate         },
		{ IDM_FERTILITY,       tBrushMode::fertility       },
		{ IDM_FOOD_STOCK,      tBrushMode::food            },
		{ IDM_FERTILIZER,      tBrushMode::fertilizer      }
	};

	tBrushMode const brushMode = mapModeTable.at( wId ) ;
	m_pWorkThreadInterface->PostSetBrushMode( brushMode );
	m_pDspOptWindow->UpdateDspOptionsControls( brushMode );
}

void EditorWindow::setBrushShape( WORD const wId ) const
{
	static unordered_map < WORD, tShape > mapShapeTable =
	{
		{ IDM_EDIT_CIRCLE,    tShape::Circle },    
		{ IDM_EDIT_RECTANGLE, tShape::Rect   }
	};

	tShape const brushShape = mapShapeTable.at( wId );
	m_pWorkThreadInterface->PostSetBrushShape( brushShape );
}

INT_PTR EditorWindow::UserProc( UINT const message, WPARAM const wParam, LPARAM const lParam )
{
    switch (message)
    {
    case WM_HSCROLL:
		{
			HWND const hwndTrackBar = (HWND)lParam;
			int  const iCtrlId      = GetDlgCtrlID( hwndTrackBar );
			LONG const lLogicalPos  = GetTrackBarPos( iCtrlId );
			switch ( iCtrlId )
			{
			case IDM_EDIT_INTENSITY:
				m_pWorkThreadInterface->PostSetBrushIntensity( lLogicalPos );
				break;
			case IDM_EDIT_SIZE:
				m_pWorkThreadInterface->PostSetBrushSize( lLogicalPos );
				break;
			default:
				assert( false );
			}
		}
        return TRUE;

    case WM_COMMAND:
        {
            WORD const wId = LOWORD( wParam );

            switch ( wId )
            {
            case IDM_MOVE:
            case IDM_RANDOM_STRATEGY:
            case IDM_COOPERATE:
            case IDM_DEFECT:
            case IDM_TIT4TAT:
            case IDM_KILL_ANIMALS:
            case IDM_MUT_RATE:
            case IDM_FERTILITY:
            case IDM_FOOD_STOCK:
				setBrushMode( wId );
                break;

            case IDM_EDIT_CIRCLE:
            case IDM_EDIT_RECTANGLE:
				setBrushShape( wId );
                break;

            default:
				assert( false );
                break;
            }
        }
        break;

    case WM_CLOSE:
        Show( FALSE );
        return TRUE;

    case WM_DESTROY:
        DestroyWindow( GetWindowHandle( ) );
        break;

    default:
        break;
    }

    return FALSE;
}
