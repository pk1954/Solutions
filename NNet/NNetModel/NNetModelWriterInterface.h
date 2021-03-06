// NNetModelWriterInterface.h
//
// NNetModel

#pragma once

#include <string>
#include "boolOp.h"
#include "ParameterType.h"
#include "MoreTypes.h"
#include "ConnectionNeuron.h"
#include "ShapeId.h"
#include "NNetModel.h"

class Pipe;
class BaseKnot;
class ShapeErrorHandler;
class MicroMeterPointVector;

using std::unique_ptr;
using std::move;

class NNetModelWriterInterface
{
public:
	void          Start(NNetModel * const);
	void          Stop(); 
    void          CreateInitialShapes();
    void          RemoveOrphans();
    void          SelectBeepers();
    void          SelectShape(ShapeId const, bool const);
    void          ToggleStopOnTrigger(ShapeId const);
    Shape * const GetShape(ShapeId const);

    UPShapeList & GetUPShapes()    { return m_pModel->GetUPShapes(); }
    Param       & GetParams()      { return m_pModel->GetParams(); }
    MonitorData & GetMonitorData() { return m_pModel->GetMonitorData(); }

    void CheckModel() { m_pModel->CheckModel(); }
    void ResetModel() { m_pModel->ResetModel(); }
    void ClearModel() { m_pModel->GetUPShapes().Apply2All([&](Shape & s) { s.Clear(); }); }

    void DumpModel() const { m_pModel->DumpModel(); }

    void  SelectSubtree(BaseKnot  * const p, bool  const b) { m_pModel->SelectSubtree(p, b); }
    float SetParam(ParamType::Value const p, float const f) { return m_pModel->SetParam(p, f); }
    void  SetModelFilePath  ( wstring const wstr ) { m_pModel->SetModelFilePath  ( wstr ); }
    void  AddDescriptionLine( wstring const wstr ) { m_pModel->AddDescriptionLine( wstr ); }
    void  DescriptionComplete()                    { m_pModel->DescriptionComplete(); }

    wstring const GetModelFilePath() { return m_pModel->GetModelFilePath(); }

    bool const IsConnector( ShapeId const id )
    {
        Shape * pShape { GetShapePtr<Shape *>( id ) };
        return pShape && pShape->IsConnector();
    }

    bool const IsPipe( ShapeId const id )
    {
        Shape * pShape { GetShapePtr<Shape *>( id ) };
        return pShape && pShape->IsPipe();
    }

    bool const IsKnot( ShapeId const id )
    {
        Shape * pShape { GetShapePtr<Shape *>( id ) };
        return pShape && pShape->IsKnot();
    }

    template <Shape_t T>
    T GetShapePtr( ShapeId const id ) 
    {
        Shape * const pShape { GetShape( id ) };
        return (pShape && HasType<T>( * pShape )) ? static_cast<T>( pShape ) : nullptr;
    }

    template <Shape_t NEW, Shape_t OLD>
    unique_ptr<OLD> ReplaceInModel( unique_ptr<NEW> up ) 
    {
        ShapeId id     { up.get()->GetId() };
        Shape * pShape { m_pModel->GetUPShapes().ReplaceShape( id, move(up) ) }; 
        return move( unique_ptr<OLD>( static_cast<OLD*>(pShape) ) );
    }

    template <Shape_t OLD>
    unique_ptr<OLD> RemoveFromModel( Shape const & shape ) 
    { 
        return RemoveFromModel<OLD>(shape.GetId());
    }

    template <Shape_t OLD>
    unique_ptr<OLD> RemoveFromModel( ShapeId const id ) 
    { 
        UPShape upShape { m_pModel->GetUPShapes().ExtractShape(id) }; 
        auto    pShape  { upShape.release() }; 
        return move( unique_ptr<OLD>( static_cast<OLD*>(pShape) ) );
    }

    MicroMeterPoint const OrthoVector( ShapeId const idPipe ) const
    {
        MicroMeterPoint vector { m_pModel->GetShapeConstPtr<Pipe const *>(idPipe)->GetVector() };
        return vector.OrthoVector().ScaledTo(NEURON_RADIUS*2.f);
    }

    void SetConnNeurons(MicroMeterPointVector &, ShapeIdList const &);
    void SetConnNeurons(MicroMeterPointVector const &, ShapePtrList<ConnNeuron> const &);

#ifdef _DEBUG
    NNetModel const & GetModel()  const { return * m_pModel; }  // TODO: find better solution
#endif
private:

    NNetModel * m_pModel { nullptr };
}; 
