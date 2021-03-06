// SoundInterface.h
//
// Toolbox\Utilities

#pragma once

#include <string>
#include "MoreTypes.h"
#include "Observable.h"

using std::wstring;

struct SoundDescr
{
	bool      m_bOn       { false };
	Hertz     m_frequency { 0_Hertz };      
	MilliSecs m_duration  { 0_MilliSecs };
};

class Sound
{
public:
	void Initialize( Observable * const pObservable )
	{
		m_pObservable = pObservable;
	}

	virtual void Play( wstring const ) const = 0;

	virtual void Beep( SoundDescr const & ) const = 0;

	void On () 
	{ 
		m_bActive = true;
		m_pObservable->NotifyAll( true );
	}

	void Off() 
	{ 
		m_bActive = false; 
		m_pObservable->NotifyAll( true );
	}

	bool IsOn() const { return m_bActive; }

private:

	inline static Observable * m_pObservable { nullptr };
	inline static bool         m_bActive     { true };
};
