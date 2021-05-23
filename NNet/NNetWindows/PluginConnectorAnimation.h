// PluginConnectorAnimation.h
//
// NNetWindows

#pragma once

#include "vector"
#include "MoreTypes.h"
#include "AnimatedCommand.h"
#include "Connector.h"
#include "ClosedConnector.h"
#include "NobPtrList.h"
#include "SingleNobAnimation.h"
#include "win32_callable.h"
#include "win32_animation.h"
#include "ConnectIoObjectsCommand.h"

using std::unique_ptr;
using std::vector;

using ConnectConnectorsCommand = ConnectIoObjectsCommand<Connector,ClosedConnector>;

class PluginConnectorAnimation : public AnimatedCommand
{
public:
    PluginConnectorAnimation
    (
        Connector   &,
        Connector   &,
        MainWindow  &,
        WinCommands &
    );
    virtual ~PluginConnectorAnimation() {};

private:

    Connector & m_nobAnimated;

    unique_ptr<ConnectConnectorsCommand> m_upConnectConnectors;
    unique_ptr<SingleNobAnimation>       m_upSingleNobAnimation;

    virtual void doPhase  ();
    virtual void undoPhase();
    virtual void updateUI ();
};