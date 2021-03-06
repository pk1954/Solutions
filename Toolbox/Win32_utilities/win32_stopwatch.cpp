// win32_stopwatch.cpp
//
// Toolbox/win32_utilities

#pragma once

#include "stdafx.h"
#include <sstream> 
#include <iostream>
#include <iomanip>
#include <assert.h>
#include "util.h"
#include "windows.h"
#include "win32_stopwatch.h"

using std::wcout;
using std::setw;
using std::fixed;
using std::endl;
using std::setprecision;
using std::left;
using std::right;

void Stopwatch::Start()
{
	++m_iLevel; 
	m_hrtimer.Start();
}

void Stopwatch::Stop( wstring const wstr )
{
	assert( m_iLevel > 0 );  // no Stop without Start

	m_hrtimer.Stop();
	microseconds microSecs = m_hrtimer.GetDuration();
	float        millisecs = Cast2Float( microSecs.count() ) / 1000.0f;
	--m_iLevel;
	for ( int i = 0; i < m_iLevel; ++i )
		std::wcout << L"      ";
	wcout << setw(30) << left << wstr;
	wcout << setw( 6) << right;
	wcout << fixed    << setprecision(2) << millisecs;
	wcout << L" ms" << endl;
}
