// win32_appTitle.h : 
//
// NNetWindows

#pragma once

#include "ObserverInterface.h"
#include "win32_util_resource.h"
#include "win32_util.h"
#include "NNetModelReaderInterface.h"

class AppTitle : public ObserverInterface
{
public:
	void Initialize
	( 
		HWND                             const hwndApp,
		NNetModelReaderInterface const * const pNMRI
	)
	{
		m_hwndApp  = hwndApp;
		m_pNMRI    = pNMRI;
	}

    virtual void Notify( bool const bImmediate ) 
    { 
		Util::SetApplicationTitle
		( 
			m_hwndApp, 
			IDS_APP_TITLE, 
			m_pNMRI->GetModelFilePath() + (m_pNMRI->AnyUnsavedChanges() ? L" * " : L"") 
		);
	}

private:
	HWND                             m_hwndApp  { nullptr };
	NNetModelReaderInterface const * m_pNMRI    { nullptr };
};
