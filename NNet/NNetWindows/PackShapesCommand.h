// PackShapesCommand.h
//
// NNetWindows

#pragma once

#include "win32_Commands.h"
#include "ConnAnimationCommand.h"

class RootWindow;

class PackShapesCommand : public ConnAnimationCommand
{
public:
    PackShapesCommand
    (
        RootWindow  * pWin,
        WinCommands & cmds,
        int   const   iMsg,
        bool  const   bBackwards
    )
        : ConnAnimationCommand(move(cmds.CreateShapeList()), pWin, cmds, iMsg, bBackwards)
    {}

private:
    virtual void DefineTarget()
    {
        m_umPntVectorTarget.Pack(NEURON_RADIUS * 1.8f);
    }
};