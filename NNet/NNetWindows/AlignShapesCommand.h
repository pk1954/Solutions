// AlignShapesCommand.h
//
// NNetWindows

#pragma once

#include "ConnAnimationCommand.h"

class RootWindow;

class AlignShapesCommand : public ConnAnimationCommand
{
public:
    AlignShapesCommand
    (
        unique_ptr<ShapePtrList<ConnNeuron>> upShapesAnimated,
        RootWindow                   const * pWin,
        int                          const   iMsg,
        bool                         const   bBackwards
    )
        : ConnAnimationCommand(move(upShapesAnimated), pWin, iMsg, bBackwards)
    {}

private:
    virtual void DefineTarget()
    {
        m_umPntVectorTarget.Align(m_line);
    }
};