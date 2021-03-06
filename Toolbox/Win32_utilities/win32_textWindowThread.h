// win32_textWindowThread.h 
//
// win32_utilities

#pragma once

#include "win32_textWindow.h"
#include "win32_textBuffer.h"
#include "win32_thread.h"

using std::unique_ptr;
using std::make_unique;

class TextWindowThread: public Util::Thread
{
public:
	TextWindowThread
	(
		HDC             hDC_Memory,
		PixelRectSize & pixSize,
		TextWindow    & textWindow,
		wstring const & strName,
		bool            bAsync
	) :
		m_textWindow( textWindow ),
		m_hDC( hDC_Memory )
	{ 
	    m_pTextBuffer = make_unique<Win32_TextBuffer>( hDC_Memory, pixSize );
		if ( bAsync )
			StartThread( strName, bAsync );
		PostThreadMsg( anyMessageWillDo );
	}

	virtual ~TextWindowThread()	{ }

	virtual void Trigger()
	{
		PostThreadMsg( anyMessageWillDo );
	}

	virtual void ThreadMsgDispatcher( MSG const msg )
	{
        m_pTextBuffer->StartPainting();
		m_textWindow.DoPaint( * m_pTextBuffer );
		m_textWindow.Invalidate( false );
	}

private:
	const unsigned int anyMessageWillDo = 42;

	TextWindow           & m_textWindow;
	unique_ptr<TextBuffer> m_pTextBuffer;
    HDC                    m_hDC;
};
