// AlignPositionsCommand.h
//
// NNetWindows

#pragma once

#include "win32_Commands.h"
#include "ConnAnimationCommand.h"

class RootWindow;

class AlignPositionsCommand : public ConnAnimationCommand
{
public:
    AlignPositionsCommand
    (
        RootWindow  & win,
        WinCommands & cmds,
        int     const iMsg,
        bool    const bBackwards
    )
        : ConnAnimationCommand(win, cmds, iMsg, bBackwards)
    {}

private:
    virtual void DefineTarget()
    {
        m_umPntVectorTarget.Align(m_line);
    }
};