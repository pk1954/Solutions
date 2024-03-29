// Accelerators.ixx
//
// NNetSimu  

module;

#include <cassert>
#include <array>
#include <Windows.h>
#include "Resource.h"

export module Accelerators;

import Win32_Util_Resource;
import SaveCast;

using std::array;

export class Accelerators  // doesn't work
{
public:
    HACCEL Get()
    {
        ACCEL m_acc[14]
        {
            ACCEL(FVIRTKEY | FCONTROL, WORD('c'), IDM_COPY_SELECTION),
            ACCEL(FVIRTKEY | FCONTROL, WORD('z'), IDM_UNDO),
            ACCEL(FVIRTKEY | FCONTROL, WORD('y'), IDM_REDO),
            ACCEL(FVIRTKEY | FCONTROL, WORD('a'), IDM_SELECT_ALL),
            ACCEL(FVIRTKEY | FCONTROL, WORD('s'), IDM_SAVE_MODEL),
            ACCEL(FVIRTKEY | FALT,     VK_BACK,   IDM_UNDO),
            ACCEL(FVIRTKEY,            VK_DELETE, IDM_DELETE),
            ACCEL(FVIRTKEY,            VK_ESCAPE, IDM_ESCAPE),
            ACCEL(FALT,                WORD('c'), IDM_CRSR_WINDOW),
            ACCEL(FALT,                WORD('d'), IDM_DESC_WINDOW),
            ACCEL(FALT,                WORD('p'), IDM_PARAM_WINDOW),
            ACCEL(FALT,                WORD('m'), IDM_MINI_WINDOW),
            ACCEL(FALT,                WORD('?'), IDM_ABOUT),
            ACCEL(FALT,                WORD('n'), IDM_NEXT_SCRIPT_CMD)
        };

        HACCEL haccel { CreateAcceleratorTable(LPACCEL(m_acc), 14) }; // Cast2Int(m_acc.size()))
        assert(haccel);
        ACCEL a1 { m_acc[2] };
        ACCEL * pAcc = m_acc;

        int x = sizeof(ACCEL);
        int y = sizeof(m_acc);
        return haccel;
    }

private:

};
