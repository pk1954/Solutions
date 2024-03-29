// NNetSimu.cpp
//
// NNetSimu

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <Windows.h>
#include "Resource.h"
#include "CommCtrl.h"

import Win32_Util_Resource;
import Win32_Util;
import Util;
import IoConstants;
import Trace;
import Stopwatch;
import MessagePump;
import Script;
import Scanner;
import Accelerators;
import NNetWin32;

using std::make_unique;
using std::unique_ptr;
using std::wstring;
using std::wcout;
using std::endl;

unique_ptr<NNetAppWindow> upApp;

int APIENTRY wWinMain
(
	_In_     HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     LPWSTR    lpCmdLine,
	_In_     int       nCmdShow
)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

	Stopwatch stopwatch;
	stopwatch.Start();

	SetThreadAffinityMask(GetCurrentThread(), 0x0001);

	INITCOMMONCONTROLSEX icex // load common control's DLL 
	{
		sizeof(INITCOMMONCONTROLSEX),
		ICC_STANDARD_CLASSES | 
		ICC_BAR_CLASSES | 
		ICC_TAB_CLASSES | 
		ICC_TREEVIEW_CLASSES  // for tooltips
	};

	SwitchWcoutTo(L"main_trace.out");

	wcout << COMMENT_START << L"Application start at " << GetCurrentDateAndTime();
	wcout << COMMENT_START << L"Version:       "       << AppTitle::PRODUCT_NAME  << endl;
	wcout << COMMENT_START << L"Build date:    "       << COMPILE_TIMESTAMP       << endl;
	wcout << COMMENT_START << L"Computer name: "       << ::GetComputerName()     << endl;
	wcout << COMMENT_START << L"User name:     "       << ::GetUserName()         << endl;

	upApp = make_unique<NNetAppWindow>(AppTitle::PRODUCT_NAME);

//	Accelerators acc; // doesn't work

	MessagePump pump;
	pump.SetAccelTable(hInstance, IDC_NNET_SIMU_MAIN);
//	pump.SetAccelTable(acc.Get());
	upApp->Start(pump);
	pump.RegisterWindow(upApp->GetWindowHandle(), false);

	stopwatch.Stop(COMMENT_START + L"App.Start");

	int iRetVal = pump.Run();
	return iRetVal;
}
