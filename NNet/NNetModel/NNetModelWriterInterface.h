// NNetModelWriterInterface.h
//
// NNetModel

#pragma once

#include <string>
#include <cassert>
#include <xutility>
#include "BaseKnot.h"
#include "Nob.h"
#include "Pipe.h"
#include "NNetModelReaderInterface.h"
#include "SignalGenerator.h"
#include "UPNobList.h"

import BoolOp;
import Types;
import ParamType;
import SigGenId;
import UPSensorList;
import NobId;
import DescriptionUI;
import NNetParameters;
import IoLine;
import MicroMeterPosDir;
import NNetModel;
import MonitorData;

using std::unique_ptr;
using std::wstring;
using std::move;

class NNetModelWriterInterface : public NNetModelReaderInterface
{
public:
    void  CreateInitialNobs();
    void  RemoveOrphans();
    void  SelectNob(NobId const, bool const);
    void  ToggleStopOnTrigger(NobId const);
    Nob * GetNob(NobId const);

    UPNobList               & GetUPNobs()      { return m_pModel->GetUPNobs(); }
    Param                   & GetParams()      { return m_pModel->GetParams(); }
    MonitorData             & GetMonitorData() { return m_pModel->GetMonitorData(); }
    unique_ptr<vector<Nob *>> GetSelection()   { return GetUPNobs().GetAllSelected(); }

    void ResetModel      ()       { m_pModel->ResetModel(); }
    void ClearDynamicData() const { m_pModel->ClearDynamicData(); }

    SigGenId          FindSigGen       (wstring const & name)   const { return m_pModel->GetSigGenList().FindSigGen(name); }
    bool              IsValid          (SigGenId const id)      const { return m_pModel->GetSigGenList().IsValid(id); }
    SignalGenerator * StdSigGen        ()                             { return m_pModel->GetSigGenList().StdSigGen(); }
    UPSigGen          NewSigGen        ()                             { return m_pModel->GetSigGenList().NewSigGen(); }
    UPSigGen          NewSigGen        (wstring const & name)         { return m_pModel->GetSigGenList().NewSigGen(name); }
    SigGenId          PushSigGen       (UPSigGen upSigGen)            { return m_pModel->GetSigGenList().PushSigGen(move(upSigGen)); }
    SigGenId          SetSigGenActive  (SigGenId const id)            { return m_pModel->GetSigGenList().SetActive(id); }
    void              InsertSigGen     (UPSigGen u, SigGenId const i) { return m_pModel->GetSigGenList().InsertSigGen(move(u), i); }
    SignalGenerator * GetSigGenSelected()                             { return m_pModel->GetSigGenList().GetSigGenSelected(); }
    SignalGenerator * GetSigGen        (SigGenId const id)            { return m_pModel->GetSigGenList().GetSigGen(id); }
    SignalGenerator * GetSigGen        (wstring const & name)         { return m_pModel->GetSigGenList().GetSigGen(name); }
    UPSigGen          RemoveSigGen     (SigGenId const id)            { return m_pModel->GetSigGenList().RemoveSigGen(id); }
    UPSigGen          PopSigGen        ()                             { return m_pModel->GetSigGenList().PopSigGen(); }

    UPSensorList & GetSensorList() { return m_pModel->GetSensorList(); }

    void  Reconnect(NobId const id) const { m_pModel->Reconnect(id); }

    void  SelectSubtree(BaseKnot & baseKnot, bool  const b) { m_pModel->SelectSubtree(baseKnot, b); }
    float SetParam(ParamType::Value const p, float const f) { return m_pModel->SetParam(p, f); }

    void  SetDescriptionUI   (DescriptionUI  & ui  ) { m_pModel->SetDescriptionUI   (ui); }
    void  SetModelFilePath   (wstring  const & wstr) { m_pModel->SetModelFilePath   (wstr); }
    void  AddDescriptionLine (wstring  const & wstr) { m_pModel->AddDescriptionLine (wstr); }
    void  DescriptionComplete()                      { m_pModel->DescriptionComplete(); }
    void  DeselectAllNobs    () const                { m_pModel->DeselectAllNobs    (); }
    void  SetTargetNobId     (NobId const id)        { m_pModel->SetTargetNobId     (id); }
    void  SetHighlightedNobId(NobId const id)        { m_pModel->SetHighlightedNobId(id); }

    void AddOutgoing   (NobId const id, Pipe & pipe) { GetBaseKnot(id).AddOutgoing   (pipe); }
    void AddIncoming   (NobId const id, Pipe & pipe) { GetBaseKnot(id).AddIncoming   (pipe); }
    void RemoveIncoming(NobId const id, Pipe & pipe) { GetBaseKnot(id).RemoveIncoming(pipe); }
    void RemoveOutgoing(NobId const id, Pipe & pipe) { GetBaseKnot(id).RemoveOutgoing(pipe); }

    BaseKnot & GetBaseKnot(NobId const id)
    {
        BaseKnot * pBaseKnot { GetNobPtr<BaseKnot *>(id) };
        assert(pBaseKnot);
        return * pBaseKnot;
    }

    template <Nob_t T>
    T GetNobPtr(NobId const id) 
    {
        Nob * const pNob { GetNob(id) };
        return (pNob && HasType<T>(*pNob)) ? static_cast<T>(pNob) : nullptr;
    }

    template <Nob_t T>  
    void Apply2All(auto const& func)
    {
        GetUPNobs().Apply2All<T>(func);
    }

    /// RemoveFromModel - ReplaceInModel/Restore2Model: Slot remains

    template <Nob_t OLD>
    unique_ptr<OLD> RemoveFromModel(Nob const & nob) 
    { 
        return RemoveFromModel<OLD>(nob.GetId());
    }

    template <Nob_t OLD>
    unique_ptr<OLD> RemoveFromModel(NobId const id) 
    { 
        UPNob upNob { GetUPNobs().ExtractNob(id) }; 
        auto  pNob  { upNob.release() };
        return move(unique_ptr<OLD>(static_cast<OLD*>(pNob)));
    }

    void Restore2Model(unique_ptr<Nob> up) 
    {
        if (up)
            GetUPNobs().ReplaceNob(move(up));
    }

    template <Nob_t OLD>
    unique_ptr<OLD> ReplaceInModel(unique_ptr<Nob> upNew) 
    {
        NobId id      { upNew.get()->GetId() };
        Nob * pNobOld { GetUPNobs().ReplaceNob(move(upNew)) };
        Reconnect(id);
        return move(unique_ptr<OLD>(static_cast<OLD*>(pNobOld)));
    }

    NobId Push2Model(UPNob upNob)
    {
        NobId const id { GetUPNobs().Push(move(upNob)) };
        Reconnect(id);
        return id;
    }

    template <Nob_t T>
    unique_ptr<T> PopFromModel() 
    { 
        return move(GetUPNobs().Pop<T>());
    }

    unique_ptr<BaseKnot> FixBaseKnot(NobId const);

    ///////////////////////////////////////////////////////////

    void IncreaseSize(long const nr) { GetUPNobs().IncreaseSize(nr); }
    void ReduceSize  (long const nr) { GetUPNobs().ReduceSize(nr); }

    bool Compute() { return m_pModel->Compute(); }

    void SetPosDir(NobId const, MicroMeterPosDir const &);
};

void ConnectIncoming(Pipe &, BaseKnot &);
void ConnectOutgoing(Pipe &, BaseKnot &);
