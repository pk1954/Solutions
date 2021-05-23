// PluginAnimation.h
//
// NNetWindows

#pragma once

#include "MoreTypes.h"
#include "MicroMeterPntVector.h"
#include "Command.h"
#include "win32_mainWindow.h"
#include "win32_animation.h"
#include "win32_callable.h"
#include "win32_commands.h"
#include "SingleNobAnimation.h"

class NNetModelWriterInterface;

class PluginAnimation : public Command
{
public:
    PluginAnimation
    ( 
        Nob         & nobAnimated,
        MainWindow  & win,
        WinCommands & cmds
    )
    :   m_nobAnimated(nobAnimated),
        m_win(win),
        m_NMWI(cmds.GetNMWI()),
        m_callable(win.GetWindowHandle())
    {
        m_upSingleNobAnimation = make_unique<SingleNobAnimation>(win, nobAnimated);
        pushTarget(m_nobAnimated.GetPosDir());
    }

    virtual void Do( NNetModelWriterInterface& nmwi )
    {
        m_uiPhase = 0;
        doPhase();
    }

    virtual void Undo( NNetModelWriterInterface& nmwi )
    {
        m_uiPhase = Cast2Int(m_umPosDirTarget.size());
        undoPhase();
    }

protected:
    MicroMeterPosDir const calcOffsetTarget(Nob const & nobTarget, float fOffset)
    {
        if (nobTarget.GetIoMode() == NobIoMode::input )
            fOffset = -fOffset;
        Radian        const radianTarget { nobTarget.GetDir() };
        MicroMeterPnt const umDirVector  { Radian2Vector(radianTarget).ScaledTo(NEURON_RADIUS) };
        MicroMeterPnt const umPosOffset  { umDirVector * fOffset };
        MicroMeterPnt const umPosTarget  { nobTarget.GetPos() + umPosOffset };
        return MicroMeterPosDir(umPosTarget, radianTarget);
    }

    void pushTarget( MicroMeterPosDir const& umPosDir )
    {
        m_umPosDirTarget.push_back(umPosDir);
    }

    unique_ptr<Command> m_upConnectionCommand;

private:

    NNetModelWriterInterface     & m_NMWI;
    MainWindow                   & m_win;
    Callable                       m_callable;
    unique_ptr<SingleNobAnimation> m_upSingleNobAnimation;
    Nob                          & m_nobAnimated;
    vector<MicroMeterPosDir>       m_umPosDirTarget { };  
    unsigned int                   m_uiPhase        { 0 };


   void BlockUI()   { m_win.SendCommand2Application(IDM_BLOCK_UI, true);  };
   void UnblockUI() { m_win.SendCommand2Application(IDM_BLOCK_UI, false); };

    void doPhase() // runs in UI thread
    {
        switch (m_uiPhase++)
        {
        case 0:  BlockUI();
            doPhase();
            break;

        case 1:  m_upSingleNobAnimation->Start(getTarget(1), [&](){ doPhase(); });
            break;

        case 2:	 m_upSingleNobAnimation->Start(getTarget(2), [&](){ doPhase(); });
            break;

        case 3:  m_upConnectionCommand->Do(m_NMWI);
            m_win.Notify(false);
            UnblockUI();
            break; 

        default: break;        
        }
    }

    void undoPhase() // runs in UI thread
    {
        switch (m_uiPhase--)
        {
        case 3:  BlockUI();
            m_upConnectionCommand->Undo(m_NMWI);
            undoPhase();
            break;

        case 2:  m_upSingleNobAnimation->Start(getTarget(1), [&](){ undoPhase(); });
            break;

        case 1:	 m_upSingleNobAnimation->Start(getTarget(0), [&](){ undoPhase(); });
            break;

        case 0:  UnblockUI();
            break; 

        default: break;                // do not start animation
        }
    }

    MicroMeterPosDir const& getTarget( unsigned int const uiStep )
    {
        return m_umPosDirTarget.at(uiStep);
    }

};
