// Win32_SlowMotionControl.ixx
//
// NNetWindows

module;

#include <Windows.h>
#include "Resource.h"

export module SlowMotionControl;

import StatusBar;

export class SlowMotionControl
{
public:

	static void Add(StatusBar & statusBar)
	{ 
		statusBar.AddButton(L" Slower ", HMENU(IDM_SLOWER), BS_PUSHBUTTON); 
		statusBar.AddButton(L" Faster ", HMENU(IDM_FASTER), BS_PUSHBUTTON); 
	} 
};