// DeleteIoConnectorCmd.h
//
// Commands

#pragma once

#include "NNetModelWriterInterface.h"
#include "CommandFunctions.h"
#include "NNetCommand.h"
#include "CommandStack.h"
#include "Nob.h"
#include "IoConnector.h"

using std::unique_ptr;

class DeleteIoConnectorCmd : public NNetCommand
{
public:
    explicit DeleteIoConnectorCmd(Nob & nob)
      : m_connector(*Cast2IoConnector(&nob))
    {
        m_cmdStack.Initialize(m_pNMWI, nullptr);
        m_connector.Apply2All
        (
            [this](Neuron & n) 
            { 
                if (unique_ptr<NNetCommand> upCmd { MakeDeleteCommand(*m_pNMWI, n) })
                    m_cmdStack.Push(move(upCmd)); 
            }
        );
    }

    void Do() final
    {
        m_upIoConnector = m_pNMWI->RemoveFromModel<IoConnector>(m_connector);
        m_upIoConnector->ClearParentPointers();
        m_cmdStack.DoAll();
    }

    void Undo() final
    {
        m_upIoConnector->SetParentPointers();
        m_pNMWI->Restore2Model(move(m_upIoConnector));
        m_cmdStack.UndoAll();
    }

private:

    IoConnector     const & m_connector;
    CommandStack            m_cmdStack      {};
    unique_ptr<IoConnector> m_upIoConnector {};  
};
