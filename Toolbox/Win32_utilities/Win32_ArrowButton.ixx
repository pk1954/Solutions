// win32_arrowButton.ixx
//
// NNetWindows

module;

#include "Windows.h"
#include "d2d1helper.h"

export module ArrowButton;

import Direct2D;
import Win32_Controls;
import GraphicsWindow;

using D2D1::ColorF;

export class ArrowButton : public GraphicsWindow
{
public:

	ArrowButton
	(
		HWND const hwndParent,
		int  const id
	)
		: m_idCommand(id)
	{
		GraphicsWindow::Initialize(hwndParent, L"ArrowBotton", WS_CHILD | WS_VISIBLE);
	}

	void SetDirection(bool const bDir)
	{
		m_bArrowDirUp = bDir;
		Trigger();
	}

private:

	void PaintGraphics() final
	{
		fPixelRect rect { Convert2fPixelRect(GetClPixelRect()) };
		rect = rect.ScaleRect(-2._fPixel);
		m_upGraphics->UpDownArrow(m_bArrowDirUp, rect, D2D1::ColorF::Black);
		m_upGraphics->DrawRectangle(rect, D2D1::ColorF::Black, 1._fPixel);
	}

	void OnDrawItem(WPARAM const wParam, DRAWITEMSTRUCT const * const pDiStruct) final
	{
		PaintGraphics();
	}

	bool OnLButtonUp(WPARAM const wParam, LPARAM const lParam) final
	{ 
		SendCommand2Parent(m_idCommand, 0);
		return true; 
	}

	void AddColorCtlMenu(HMENU const) final {}

	bool m_bArrowDirUp { true };
	int  m_idCommand;
};