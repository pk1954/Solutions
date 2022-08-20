// DeleteIoConnectorCmd.ixx
//
// Commands

module;

#include "NNetModelWriterInterface.h"
#include "NNetCommand.h"
#include "CommandStack.h"
#include "Nob.h"
#include "IoConnector.h"

export module DeleteIoConnectorCmd;

import CommandFunctions;

using std::unique_ptr;

export class DeleteIoConnectorCmd : public NNetCommand
{
public:
    explicit DeleteIoConnectorCmd(Nob& nob)
        : m_connector(*Cast2IoConnector(&nob))
    {
        m_cmdStack.Initialize(nullptr);
        m_cmdStack.SetModelInterface(m_pNMWI);
        m_connector.Apply2All
        (
            [this](IoLine& n)
            {
                if (unique_ptr<NNetCommand> upCmd{ MakeDeleteCommand(*m_pNMWI, n) })
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

    IoConnector& m_connector;
    CommandStack            m_cmdStack{};
    unique_ptr<IoConnector> m_upIoConnector{};
};
