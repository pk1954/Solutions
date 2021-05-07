// NNetModelWriterInterface.h
//
// NNetModel

#pragma once

#include <string>
#include "boolOp.h"
#include "ParameterType.h"
#include "MoreTypes.h"
#include "ConnectionNeuron.h"
#include "NobId.h"
#include "NNetModel.h"

class Pipe;
class BaseKnot;
class NobErrorHandler;
class MicroMeterPointVector;

using std::unique_ptr;
using std::move;

class NNetModelWriterInterface
{
public:
	void          Start(NNetModel * const);
	void          Stop(); 
    void          CreateInitialNobs();
    void          RemoveOrphans();
    void          SelectBeepers();
    void          SelectNob(NobId const, bool const);
    void          ToggleStopOnTrigger(NobId const);
    Nob * const GetNob(NobId const);

    UPNobList       & GetUPNobs()    { return m_pModel->GetUPNobs(); }
    Param             & GetParams()      { return m_pModel->GetParams(); }
    MonitorData       & GetMonitorData() { return m_pModel->GetMonitorData(); }
    NobPtrList<Nob> GetSelection()   { return GetUPNobs().GetAllSelected<Nob>(); }

    void CheckModel() { m_pModel->CheckModel(); }
    void ResetModel() { m_pModel->ResetModel(); }
    void ClearModel() { m_pModel->GetUPNobs().Apply2All([&](Nob & s) { s.Clear(); }); }

    void DumpModel() const { m_pModel->DumpModel(); }

    void  SelectSubtree(BaseKnot  * const p, bool  const b) { m_pModel->SelectSubtree(p, b); }
    float SetParam(ParamType::Value const p, float const f) { return m_pModel->SetParam(p, f); }
    void  SetModelFilePath  ( wstring const wstr ) { m_pModel->SetModelFilePath  ( wstr ); }
    void  AddDescriptionLine( wstring const wstr ) { m_pModel->AddDescriptionLine( wstr ); }
    void  DescriptionComplete()                    { m_pModel->DescriptionComplete(); }

    wstring const GetModelFilePath() { return m_pModel->GetModelFilePath(); }

    bool const IsConnector( NobId const id )
    {
        Nob * pNob { GetNobPtr<Nob *>( id ) };
        return pNob && pNob->IsConnector();
    }

    bool const IsPipe( NobId const id )
    {
        Nob * pNob { GetNobPtr<Nob *>( id ) };
        return pNob && pNob->IsPipe();
    }

    bool const IsKnot( NobId const id )
    {
        Nob * pNob { GetNobPtr<Nob *>( id ) };
        return pNob && pNob->IsKnot();
    }

    template <Nob_t T>
    T GetNobPtr( NobId const id ) 
    {
        Nob * const pNob { GetNob( id ) };
        return (pNob && HasType<T>( * pNob )) ? static_cast<T>( pNob ) : nullptr;
    }

    template <Nob_t NEW, Nob_t OLD>
    unique_ptr<OLD> ReplaceInModel( unique_ptr<NEW> up ) 
    {
        NobId id     { up.get()->GetId() };
        Nob * pNob { m_pModel->GetUPNobs().ReplaceNob( id, move(up) ) }; 
        return move( unique_ptr<OLD>( static_cast<OLD*>(pNob) ) );
    }

    template <Nob_t OLD>
    unique_ptr<OLD> RemoveFromModel( Nob const & nob ) 
    { 
        return RemoveFromModel<OLD>(nob.GetId());
    }

    template <Nob_t OLD>
    unique_ptr<OLD> RemoveFromModel( NobId const id ) 
    { 
        UPNob upNob { m_pModel->GetUPNobs().ExtractNob(id) }; 
        auto    pNob  { upNob.release() }; 
        return move( unique_ptr<OLD>( static_cast<OLD*>(pNob) ) );
    }

    MicroMeterPoint const OrthoVector( NobId const idPipe ) const
    {
        MicroMeterPoint vector { m_pModel->GetNobConstPtr<Pipe const *>(idPipe)->GetVector() };
        return vector.OrthoVector().ScaledTo(NEURON_RADIUS*2.f);
    }

    void SetConnNeurons(MicroMeterPointVector       &, NobIdList              const &);
    void SetConnNeurons(MicroMeterPointVector const &, NobPtrList<ConnNeuron> const &);

#ifdef _DEBUG
    NNetModel const & GetModel()  const { return * m_pModel; }  // TODO: find better solution
#endif
private:

    NNetModel * m_pModel { nullptr };
}; 
