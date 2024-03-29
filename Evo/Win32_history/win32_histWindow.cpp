// win32_histWindow.cpp
//
// Win32_history


#include "assert.h"
#include "Windowsx.h"
#include "strsafe.h"
#include "HistorySystem.h"
#include "historyIterator.h"
#include "win32_util.h"
#include "win32_tooltip.h"
#include "win32_WorkThreadInterface.h"
#include "win32_genDisplayWindow.h"
#include "win32_histWindow.h"

HistWindow::HistWindow( ) :
    BaseWindow( ),
    m_pHistSys( nullptr ),
    m_pGenDisplay( nullptr ),
	m_pWorkThreadInterface( nullptr ),
    m_trackStruct( { sizeof( TRACKMOUSEEVENT ), TME_LEAVE, nullptr, 0L } ),
    m_genSelected( )
{ }

void HistWindow::Start
(
    HWND                  const hwndParent,
    HistorySystem       *       pHistSys,
	WorkThreadInterface * const pWorkThreadInterface
)
{
    m_pHistSys             = pHistSys;
	m_pWorkThreadInterface = pWorkThreadInterface;
	m_pGenDisplay          = new GenDisplayWindow( );

    HWND const hwndHistory = StartBaseWindow
    (
        hwndParent,
        CS_OWNDC | CS_DBLCLKS,
        L"ClassHistWindow",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		nullptr,
		[&]() { return ! m_pWorkThreadInterface->IsRunning(); }
    );

    Util::AddWindowStyle( hwndHistory, WS_EX_STATICEDGE );
    m_trackStruct.hwndTrack = hwndHistory;

    m_pGenDisplay->StartGenDisplayWindow( GetWindowHandle( ) );
	m_pHistSys->RegisterObserver( this );  // Trigger me, if something happens in history system
}

void HistWindow::Stop( )
{
	DestroyWindow( );
	m_pHistSys = nullptr;
	m_pWorkThreadInterface = nullptr;
}

HistWindow::~HistWindow( )
{
    delete m_pGenDisplay;
    m_pHistSys = nullptr;
    m_pGenDisplay = nullptr;
}

void HistWindow::doPaint( HDC const hDC )
{
	if ( m_pWorkThreadInterface )
	{
		paintAllGenerations( hDC );
		paintHighlightGenerations( hDC, m_pWorkThreadInterface->GetGenDemanded( ) );
	}
}

void HistWindow::gotoGeneration( HIST_GENERATION const gen ) 
{ 
	m_pWorkThreadInterface->PostGotoGeneration( gen ); 
}

PixelRect HistWindow::getGenerationRect( HIST_GENERATION const gen ) const
{
    return getGenerationRect( gen, gen + 1 );
}

PixelRect HistWindow::getGenerationRect  // position is relative to client area
(
    HIST_GENERATION const genLo,
    HIST_GENERATION const genHi
) const
{
	PixelRectSize   const rectSize    { GetClRectSize() };
	PIXEL           const pixSize     { rectSize.GetXvalue() };
	HIST_GENERATION const genNrOfGens { m_pHistSys->GetNrOfGenerations( ) };
    assert( genNrOfGens  > 0 );

	PIXEL pixLeft { PIXEL( MulDiv( genLo.GetLong( ), pixSize.GetValue(), genNrOfGens.GetLong() ) ) };
	PIXEL pixRight{ PIXEL( MulDiv( genHi.GetLong( ), pixSize.GetValue(), genNrOfGens.GetLong() ) ) };

	if ( pixLeft == pixRight )
		++pixRight;

    return PixelRect{ pixLeft, 0_PIXEL, pixRight, rectSize.GetY() };
}

HIST_GENERATION HistWindow::getGenFromXpos( LPARAM const lParam ) const
{
    PIXEL           const pixXpos  { PIXEL(GET_X_LPARAM( lParam )) };
    PIXEL           const pixWidth { GetClientWindowWidth( ) };
    long            const lNrOfGens{ m_pHistSys->GetNrOfGenerations( ).GetLong( ) };
    HIST_GENERATION const genMax   { lNrOfGens - 1 };
	HIST_GENERATION       genRes   { ( max( 0, pixXpos.GetValue() ) * lNrOfGens ) / pixWidth.GetValue() };

	if ( genRes > genMax )
		genRes = genMax;

    return genRes;
}

void HistWindow::gotoNewGeneration( LPARAM const lParam )
{
	HIST_GENERATION genSelNew { getGenFromXpos( lParam ) };
	if (genSelNew != m_genSelected)     // can be triggered several times with same value
	{
		m_genSelected = genSelNew;
		gotoGeneration( m_genSelected );
	}
}

void HistWindow::dispGenerationWindow( ) const
{
	PIXEL      const pixGenDispWidth { 50_PIXEL };
    PIXEL      const pixGenDispHeight{ 20_PIXEL };
    PIXEL      const pixClientWidth  { GetClientWindowWidth( ) };
    PixelRect  const pixRectGen      { getGenerationRect( m_genSelected ) };   // position is relative to client area
	PixelPoint const ptClientPos     { Client2Screen( PP_ZERO ) };             // position of client area origin in screen coordinates
    PIXEL      const pixYpos         { ptClientPos.GetY() - pixGenDispHeight };
    PIXEL            pixXpos         { ( pixRectGen.GetLeft() + pixRectGen.GetRight() - pixGenDispWidth ) / 2 + ptClientPos.GetX() };

    pixXpos = max( pixXpos, ptClientPos.GetX() );                                     // do not leave client area on left
    pixXpos = min( pixXpos, ptClientPos.GetX() + pixClientWidth - pixGenDispWidth );  // or right side

    m_pGenDisplay->Move( pixXpos, pixYpos, pixGenDispWidth, pixGenDispHeight, FALSE );
    m_pGenDisplay->DisplayGenerationBelowCursor( m_genSelected );
    m_pGenDisplay->Show( TRUE );
}

void HistWindow::paintGeneration( HDC const hDC, HIST_GENERATION const gen, COLORREF const col ) const
{
    assert( gen >= 0 );
    SetBkColor( hDC, col );
    Util::FastFill( hDC, getGenerationRect( gen ) );
}

void HistWindow::paintPixelPos( HDC const hDC, PIXEL const pixPosX, HistoryIterator & histIter ) const
{
    PIXEL           const pixClientWidth = GetClientWindowWidth( );
    HIST_GENERATION const genNrOfGens    = m_pHistSys->GetNrOfGenerations( );
    HIST_GENERATION const genMin = MulDiv( pixPosX.GetValue(),     genNrOfGens.GetLong( ), pixClientWidth.GetValue() ) + 1;
    HIST_GENERATION       genMax = MulDiv( pixPosX.GetValue() + 1, genNrOfGens.GetLong( ), pixClientWidth.GetValue() );
    BOOL                  bFoundPos = FALSE;  // Found a pixel pos representing an existing history entry
    BOOL                  bFoundNeg = FALSE;  // Found a pixel pos representing a missing history entry

    if ( genMax < genMin )
        genMax = genMin;

    while ( histIter.GetCurrentGeneration( ) >= genMin )  // move backwards before genMin
        ( void )histIter.Set2Senior( );

    do
    {
        HIST_GENERATION gen = histIter.GetCurrentGeneration( );

        if ( ( genMin <= gen ) && ( gen <= genMax ) )  // gen maps to lPixPos
            bFoundPos = TRUE;

        if ( histIter.Set2Junior( ).IsNull() )
            break;

        if ( histIter.GetCurrentGeneration( ) > gen + 1 )
            bFoundNeg = TRUE;       // gap found

    } while ( histIter.GetCurrentGeneration( ) <= genMax );  // move forwards after genMax

    SetBkColor
    (
        hDC,
        bFoundPos
        ? ( bFoundNeg ? CLR_MIX : CLR_DARK )
        : CLR_BACK
    );

    Util::FastFill( hDC, PixelRect{ pixPosX, 0_PIXEL, pixPosX + 1_PIXEL, GetClientWindowHeight( ) } );
}

void HistWindow::paintAllGenerations( HDC const hDC )
{
    PIXEL const pixSizeX = GetClientWindowWidth( );

    if ( m_pHistSys->GetNrOfGenerations( ) <= 1 )
        return;

	if ( m_pHistSys->GetNrOfGenerations( ) < pixSizeX.GetValue() )
    {
		HistoryIterator histIter( m_pHistSys->GetHistoryCache() ); 
        
		HistSlotNr slotNr = histIter.Set2Oldest( );

        for ( HIST_GENERATION genRun = 0; slotNr.IsNotNull(); ++genRun )
        {
            BOOL bGenRunInHistory = (genRun >= histIter.GetCurrentGeneration( ));
            paintGeneration( hDC, genRun, bGenRunInHistory ? CLR_DARK : CLR_BACK );
            if ( bGenRunInHistory )
                slotNr = histIter.Set2Junior( );
        }
    }
    else // more generations than pixels
    {
		HistoryIterator histIter( m_pHistSys->GetHistoryCache() ); 

		(void)histIter.Set2Oldest( );

        for ( PIXEL pixX = 0_PIXEL; pixX < pixSizeX; ++pixX )
        {
            paintPixelPos( hDC, pixX, histIter );
        }
    }
}

void HistWindow::paintHighlightGenerations( HDC const hDC, HIST_GENERATION const genDemanded ) const
{
	HIST_GENERATION const genActive = m_pHistSys->GetCurrentGeneration( );

    paintGeneration( hDC, genActive, CLR_RED );

    if ( ( m_genSelected >= 0L ) && ( m_genSelected != genActive ) )
    {
        paintGeneration( hDC, m_genSelected, CLR_YELLOW );
    }

    if ( ( genActive != genDemanded ) && ( m_pHistSys->IsInHistoryMode( ) ) && !genDemanded.IsInfinite( ) )
        paintGeneration( hDC, genDemanded, CLR_GREEN );
}

void HistWindow::paintLifeLine( HDC const hDC, HIST_GENERATION const genBirth, HIST_GENERATION const genDeath ) const
{
    PixelRect   pixRect = getGenerationRect( genBirth, max( genBirth, genDeath ) );
    PIXEL const height4 = GetClientWindowHeight( ) / 4;
    SetBkColor( hDC, CLR_POI );
	Util::FastFill
	( 
		hDC, 
		PixelRect
		{ 
			pixRect.GetLeft(), 
			pixRect.GetTop() + height4, 
			pixRect.GetRight(), 
			pixRect.GetBottom() - height4
		} 
	);
}

LRESULT HistWindow::UserProc( UINT const message, WPARAM const wParam, LPARAM const lParam )
{
    switch ( message )
    {

	case WM_COMMAND:
		PostCommand2Application( wParam, lParam ); // not handled here, delegate to application
		return FALSE;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint( &ps );
        doPaint( hDC );
        (void)EndPaint( &ps );

        if ( ( CrsrInClientRect( ) || IsCaptured( ) ) && ( m_genSelected >= 0 ) )
            dispGenerationWindow( );
        else
            m_pGenDisplay->Show( FALSE );

        return FALSE;
    }

    case WM_MOUSEMOVE:
        (void)TrackMouseEvent( &m_trackStruct );
        if ( wParam & MK_LBUTTON )                // Left mouse button
			gotoNewGeneration( lParam );
        return FALSE;

    case WM_MOUSELEAVE:
        m_pGenDisplay->Show( FALSE );
        m_genSelected = -1L;
        return FALSE;

    case WM_LBUTTONDOWN:
		gotoNewGeneration( lParam );
        (void)SetCapture( );
        (void)SetFocus( );
        Invalidate( FALSE );   // Redraw, do not erase background
        return FALSE;

    case WM_LBUTTONUP:
        (void)ReleaseCapture( );
        return FALSE;

	case WM_SIZE:
		CreateWindowToolTip( L"The history window allows to step back to earlier versions of the model. Use left mouse button. The past is on the left side." );
		break;

	default:
        break;
    }

    return DefWindowProc( message, wParam, lParam );
}
