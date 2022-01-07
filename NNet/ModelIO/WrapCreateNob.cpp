// WrapCreateNob.cpp
//
// ModelIO

#include "stdafx.h"
#include "ERRHNDL.H"
#include "Knot.h"
#include "InputNeuron.h"
#include "OutputNeuron.h"
#include "IoConnector.h"
#include "InputConnector.h"
#include "OutputConnector.h"
#include "WrapCreateNob.h"

using std::wcout;

Nob * WrapCreateNob::createNob(Script & script) const
{   
    NobId   const idFromScript { ScrReadNobId(script) };
    NobType const nobType      { ScrReadNobType(script) };
    Nob         * pNob         { nullptr };
    UPNob         upNob        {};   
    { 
        switch (nobType.GetValue())
        {
        case NobType::Value::inputConnector:
        case NobType::Value::outputConnector:
            upNob = createIoConnector(script, nobType);
            break;
        case NobType::Value::inputNeuron:
        case NobType::Value::outputNeuron:
        case NobType::Value::neuron:
        case NobType::Value::knot:
            upNob = createBaseKnot(script, nobType);
            break;
        case NobType::Value::pipe:
            upNob = createPipe(script);
            break;
        default:
            assert(false);  // handled in ScrReadNobType
            break;
        }
    };
    if (upNob)
    {
        upNob->SetId(idFromScript);
        pNob = upNob.get();
        GetUPNobsRef().SetNob2Slot(idFromScript, move(upNob));
    }
    if (pNob && nobType.IsIoConnectorType())
    {
        static_cast<IoConnector *>(pNob)->SetParentPointers();
    }
    return pNob;
}

UPNob WrapCreateNob::createPipe(Script & script) const
{
    script.ScrReadSpecial(Pipe::OPEN_BRACKET);
    NobId const idStart { ScrReadNobId(script) };
    for (int i = 0; i < Pipe::SEPARATOR.length(); i++)
        script.ScrReadSpecial(Pipe::SEPARATOR[i]);        
    NobId const idEnd { ScrReadNobId(script) };
    script.ScrReadSpecial(Pipe::CLOSE_BRACKET);
    NNetModelImporter::CheckImportedNobId(script, GetUPNobsRef(), idStart);
    NNetModelImporter::CheckImportedNobId(script, GetUPNobsRef(), idEnd);
    if (idStart == idEnd)
    {
        wcout << "+++ Pipe has identical start and end point" << endl;
        wcout << "+++ " << L": " << idStart << L" -> " << idEnd << endl;
        wcout << "+++ Pipe ignored" << endl;
        throw ScriptErrorHandler::ScriptException(999, wstring(L"Error reading Pipe") );
        return nullptr;
    }
    else
    { 
        BaseKnot * const pKnotStart { GetWriterInterface().GetNobPtr<BaseKnot *>(idStart) };
        BaseKnot * const pKnotEnd   { GetWriterInterface().GetNobPtr<BaseKnot *>(idEnd  ) };
        unique_ptr<Pipe> upPipe { make_unique<Pipe>(pKnotStart, pKnotEnd) };
        pKnotStart->AddOutgoing(upPipe.get());
        pKnotEnd  ->AddIncoming(upPipe.get());
        return move(upPipe);
    }
}

UPNob WrapCreateNob::createBaseKnot(Script & script, NobType const nobType) const 
{
    MicroMeterPnt const umPosition(ScrReadMicroMeterPnt(script));
    switch (nobType.GetValue())
    {
    case NobType::Value::inputNeuron:
        return make_unique<InputNeuron>(umPosition);

    case NobType::Value::outputNeuron:
        return make_unique<OutputNeuron>(umPosition);

    case NobType::Value::neuron:
        return make_unique<Neuron>(umPosition);

    case NobType::Value::knot:
        return make_unique<Knot>(umPosition);

    default:
        assert(false);
        return nullptr;
    }
}

UPNob WrapCreateNob::createIoConnector(Script & script, NobType const nobType) const 
{
    vector<IoNeuron *> ioNeuronList;
    script.ScrReadSpecial(BaseKnot::OPEN_BRACKET);
    int const iNrOfElements { script.ScrReadInt() };
    script.ScrReadSpecial(BaseKnot::NR_SEPARATOR);
    for (int iElem { 0 };;)
    {
        NobId      const id        { ScrReadNobId(script) };
        IoNeuron * const pIoNeuron { GetWriterInterface().GetNobPtr<IoNeuron *>(id) };
        if (! pIoNeuron)
            throw ScriptErrorHandler::ScriptException(999, wstring(L"NobId not found"));
        ioNeuronList.push_back(pIoNeuron);
        if (++iElem == iNrOfElements)
            break;
        script.ScrReadSpecial(BaseKnot::ID_SEPARATOR);
    }
    script.ScrReadSpecial(BaseKnot::CLOSE_BRACKET);
    unique_ptr<IoConnector> upIoConnector;
    if (nobType.IsInputConnectorType())
        upIoConnector = make_unique<InputConnector> (move(ioNeuronList));
    else
        upIoConnector = make_unique<OutputConnector>(move(ioNeuronList));
    upIoConnector->AlignDirection();
    return move(upIoConnector);
}
