// SetScalesCmd.ixx
//
// NNetCommands

module;

#include <iostream>
#include "Resource.h"

export module NNetCommands:SetScalesCmd;

import IoConstants;
import Win32_Util_Resource;
import RootWindow;
import WinManager;
import BoolWrapper;
import WrapSetScales;
import Scale;
import :AnimationCmd;

using std::wstring;

using fPixelPointAnimationCmd = AnimationCmd<fPixelPoint>;

export class SetScalesCmd : public fPixelPointAnimationCmd
{
public:
    SetScalesCmd
    (
        RootWindow * const pWin,
        fPixelPoint      & fAnimated,
        fPixelPoint const& fTarget
    )
      : fPixelPointAnimationCmd(fAnimated, fTarget),
        m_pWin(pWin)
    {}

    virtual void UpdateUI()
    {
        fPixelPointAnimationCmd::UpdateUI();
        m_pWin->SendCommand(IDD_SCALES_UPDATE);
    }

    static void Push
    (
        RootWindow  &rootWin,
        fPixelPoint &umAnimated,
        bool  const  bActive,
        bool  const  bAnimation
    )
    {
        fPixelPoint const umTarget 
        { 
            bActive 
            ? Convert2fPixelPoint(PixelPoint(V_SCALE_WIDTH, H_SCALE_HEIGHT))
            : fPP_ZERO 
        };

        if (umTarget == umAnimated)
            return;

        if (IsTraceOn())
            WrapSetScales::WriteSetScales(TraceStream(), rootWin, bActive);

        if (bAnimation)
            PushCommand(make_unique<SetScalesCmd>(&rootWin, umAnimated, umTarget));
        else
        {
            umAnimated = umTarget;
            rootWin.SendCommand(IDD_SCALES_UPDATE);
        }
    }

private:

    RootWindow * m_pWin { nullptr };
};