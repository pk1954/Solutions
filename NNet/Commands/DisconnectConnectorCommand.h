// DisconnectConnectorCommand.h
//
// Commands

#pragma once

#include <vector>
#include "NNetModelWriterInterface.h"
#include "DisconnectBaseKnotCommand.h"
#include "Command.h"
#include "CommandStack.h"
#include "NobId.h"
#include "UpNobList.h"
#include "Connector.h"

using std::unique_ptr;

class DisconnectConnectorCommand : public Command
{
public:
    DisconnectConnectorCommand
    (
        NobId const idConnector,
        bool    const bDelete 
    )
      : m_idConnector(idConnector),
        m_bDelete(bDelete)
    {}

    ~DisconnectConnectorCommand() {}

    virtual void Do( NNetModelWriterInterface & nmwi )
    {
        if ( ! m_bInitialized )
        {
            init(nmwi);
            m_bInitialized = true;
        }
        m_upConnector = nmwi.RemoveFromModel<Connector>(m_idConnector);
        m_upConnector->ClearParentPointers();
        if (m_bDelete)
            m_cmdStack.DoAll();
    }

    virtual void Undo( NNetModelWriterInterface & nmwi )
    {
        m_upConnector->SetParentPointers();
        nmwi.GetUPNobs().SetNob2Slot( move(m_upConnector) );
        if (m_bDelete)
            m_cmdStack.UndoAll();
    }

private:

    void init(NNetModelWriterInterface & nmwi) 
    { 
        m_cmdStack.Initialize(&nmwi, nullptr);
        if (m_bDelete)
        {
            nmwi.GetNobPtr<Connector *>(m_idConnector)->Apply2All
            (
                [&](Nob const & s) 
                { 
                    m_cmdStack.Push(move(make_unique<DisconnectBaseKnotCommand>(s.GetId(), true)));
                }
            );
        }
    }

    NobId         const m_idConnector;
    bool            const m_bDelete;     // true: delete Connector, false: disconnect only
    CommandStack          m_cmdStack {};
    unique_ptr<Connector> m_upConnector  {};  
    bool                  m_bInitialized { false };
};
