// CommandStack.cpp
//
// Commands

#include "stdafx.h"
#include "assert.h"
#include "NNetModelWriterInterface.h"
#include "CommandStack.h"

using std::wcout;
using std::endl;

void CommandStack::Initialize
( 
    NNetModelWriterInterface * const pModel, 
    Observable               * const pStaticModelObservable 
)
{
    m_pNMWI = pModel;
    m_pStaticModelObservable = pStaticModelObservable;
}

void CommandStack::Clear()
{
    m_iIndex = 0;
    clearRedoStack();
    notify();
}

void CommandStack::clearRedoStack()
{
    m_CommandStack.erase( m_CommandStack.begin() + m_iIndex, m_CommandStack.end() );
    assert( RedoStackEmpty() );
}

void CommandStack::Push( unique_ptr<Command> pCmd )
{
    if ( UndoStackEmpty() || ! previousCmd().Combine(*pCmd) )
    {
        m_CommandStack.push_back( move(pCmd) );
        set2YoungerCmd();
    }
}

void CommandStack::PushCommand( unique_ptr<Command> pCmd )
{
#ifdef _DEBUG
    //NNetModel const & model { m_pNMWI->GetModel() };
    //m_pNMWI->CheckModel();
    //NNetModel modelSave1( model );
    //m_pNMWI->CheckModel();
#endif
    clearRedoStack();
    assert( * pCmd );
    pCmd->Do( * m_pNMWI );
    m_pNMWI->CheckModel();
    Push( move(pCmd) );
    notify();

#ifdef _DEBUG
    //NNetModel modelSave2( model );
    //modelSave2.CheckModel();
    //m_pNMWI->CheckModel();
    //UndoCommand();
    //m_pNMWI->CheckModel();
    //if ( !(model == modelSave1) )
    //{
    //    int x = 42;
    //}
    //m_pNMWI->CheckModel();
    //RedoCommand();
    //m_pNMWI->CheckModel();
    //if ( !(model == modelSave2) )
    //{
    //    int x = 42;
    //}
#endif
}

bool CommandStack::UndoCommand()
{
    if ( UndoStackEmpty() )
       return false;
    set2OlderCmd();
    currentCmd().Undo(*m_pNMWI);
    m_pNMWI->CheckModel();
    notify();
    return true;
}

bool CommandStack::RedoCommand()
{
    if ( RedoStackEmpty() ) 
        return false;
    currentCmd().Do(*m_pNMWI);
    set2YoungerCmd();
    notify();
    return true;
}

void CommandStack::DoAll( )
{
    for ( size_t i = 0; i < m_CommandStack.size(); ++i )
        m_CommandStack[i]->Do(*m_pNMWI);
}

void CommandStack::UndoAll( )
{
    for ( size_t i = m_CommandStack.size(); i --> 0; )
        m_CommandStack[i]->Undo(*m_pNMWI);
}
