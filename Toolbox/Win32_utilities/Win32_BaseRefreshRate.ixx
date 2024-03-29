// Win32_BaseRefreshRate.ixx
//
// Win32_utilities

module;

#include <string>
#include <chrono>
#include <Windows.h>

export module BaseRefreshRate;

import Win32_Util;

using std::wstring;
using std::chrono::milliseconds;
using namespace std::chrono;

export class BaseRefreshRate
{
public:

	BaseRefreshRate();
	virtual ~BaseRefreshRate();

	virtual void Trigger() = 0;

	void SetRefreshRate(milliseconds const);
	milliseconds GetRefreshRate() const;

	void Notify(bool const bImmediately)
	{
		m_bDirty = true;
		if (bImmediately || (m_msRefreshRate == 0ms))
			trigger();
	}

	void RefreshRateDialog(HWND const);

private:
	HANDLE       m_hTimer        { nullptr };
	milliseconds m_msRefreshRate { 0ms };
	bool         m_bDirty        { true };
	float        m_fValue        { };    // the value to be edited in the dialog
	wstring      m_wstrValue     { };    // m_fValue as text
	wstring      m_wstrUnit      { };    // the text to be displayed right of the edit box
	wstring      m_wstrTitle     { };    // the title bar text of the dialog

	void startTimer(milliseconds const);
	void deleteTimer();
	void trigger()
	{
		Trigger();
		m_bDirty = false;
	}

	static void CALLBACK TimerProc(void * const, bool const);
};
