// NNetModelStorageRead.cpp
//
// NNetModel

#include "stdafx.h"
#include <filesystem>
#include "assert.h"
#include "ModelDescription.h"
#include "NNetError.h"
#include "NNetParameters.h"
#include "InputNeuron.h"
#include "win32_script.h"
#include "win32_thread.h"
#include "NNetModelStorage.h"

using std::filesystem::exists;

class WrapProtocol : public Script_Functor
{
public:
    virtual void operator() ( Script & script ) const 
    {
        script.ScrReadString( L"version" );
        double dVersion = script.ScrReadFloat();
    }
};

class WrapDescription : public Script_Functor
{
public:
    WrapDescription( wstring * const pwstrDescription ) :
        m_pwstrDescription( pwstrDescription )
    { };

    virtual void operator() ( Script & script ) const 
    {
        wstring const wstrDescription { script.ScrReadString( ) };
        * m_pwstrDescription += wstrDescription + L"\r\n";
    }

private:
    wstring * m_pwstrDescription { nullptr };
};

class WrapMarkShape : public Script_Functor
{
public:
    WrapMarkShape( NNetModelWriterInterface * const pNNetModel ) :
        m_pModelWriterInterface( pNNetModel )
    { };

    virtual void operator() ( Script & script ) const
    {   
        ShapeId const idFromScript{ script.ScrReadLong() };
        return m_pModelWriterInterface->MarkShape( idFromScript, tBoolOp::opTrue );
    }

private:

    NNetModelWriterInterface * m_pModelWriterInterface;
};

class WrapCreateShape : public Script_Functor
{
public:
    WrapCreateShape( NNetModelWriterInterface * const pNNetModel ) :
        m_pModelWriterInterface( pNNetModel )
    { };

    virtual void operator() ( Script & script ) const
    {   
        ShapeId   const idFromScript{ script.ScrReadLong() };
        ShapeType const shapeType   { static_cast<ShapeType::Value>(script.ScrReadInt( )) };
        Shape         * pShape      { nullptr };

        script.ScrReadSpecial( L'(' );
        if ( shapeType.IsPipeType() )
        {
            ShapeId const idStart { script.ScrReadLong() };
            script.ScrReadSpecial( L'-' );
            script.ScrReadSpecial( L'>' );
            ShapeId const idEnd { script.ScrReadLong() };
            if ( idStart == idEnd )
            {
                wcout << "+++ Pipe has identical start and end point" << endl;
                wcout << "+++ " << idFromScript << L": " << idStart << L" -> " << idEnd << endl;
                wcout << "+++ Pipe ignored" << endl;
            }
            else
            { 
                BaseKnot * const pKnotStart { m_pModelWriterInterface->GetShapePtr<BaseKnot *>( idStart ) };
                BaseKnot * const pKnotEnd   { m_pModelWriterInterface->GetShapePtr<BaseKnot *>( idEnd   ) };
                Pipe     * const pPipe      { new Pipe( pKnotStart, pKnotEnd ) };
                pKnotStart->m_connections.AddOutgoing( pPipe );
                pKnotEnd  ->m_connections.AddIncoming( pPipe );
                pPipe->SetId( idFromScript );
                pShape = pPipe;
            }
        }
        else 
        {
            MicroMeter      const xCoord { CastToFloat( script.ScrReadFloat() ) };
            script.ScrReadSpecial( L'|' );
            MicroMeter      const yCoord { CastToFloat( script.ScrReadFloat() ) };
            MicroMeterPoint const umPosition( xCoord, yCoord );
            switch ( shapeType.GetValue() )
            {
            case ShapeType::Value::inputNeuron:
                pShape = new InputNeuron( umPosition );
                break;

            case ShapeType::Value::neuron:
                pShape = new Neuron( umPosition );
                break;

            case ShapeType::Value::knot:
                pShape = new Knot( umPosition );
                break;

            default:
                assert( false );
                break;
            }
        }
        script.ScrReadSpecial( L')' );

        if ( pShape )
            m_pModelWriterInterface->InsertAtModelSlot( pShape, idFromScript );
    }

private:

    NNetModelWriterInterface * m_pModelWriterInterface;
};

class WrapGlobalParameter : public Script_Functor
{
public:
    WrapGlobalParameter( NNetModelWriterInterface * const pNNetModel ) :
        m_pModelWriterInterface( pNNetModel )
    { };

    virtual void operator() ( Script & script ) const 
    {
        tParameter const param( static_cast< tParameter >( script.ScrReadUint() ) );
        script.ScrReadSpecial( L'=' );
        float const fValue { CastToFloat( script.ScrReadFloat() ) };
        m_pModelWriterInterface->SetParam( param, fValue );
    }

private:
    NNetModelWriterInterface * m_pModelWriterInterface;
};

class WrapNrOfShapes : public Script_Functor
{
public:
    WrapNrOfShapes( NNetModelWriterInterface * const pNNetModel ) :
        m_pModelWriterInterface( pNNetModel )
    { };

    virtual void operator() ( Script & script ) const 
    {
        script.ScrReadSpecial( L'=' );
        m_pModelWriterInterface->SetNrOfShapes( script.ScrReadLong() );
    }

private:
    NNetModelWriterInterface * m_pModelWriterInterface;
};

class WrapShapeParameter : public Script_Functor
{
public:
    WrapShapeParameter( NNetModelWriterInterface * const pModel ) :
        m_pModelWriterInterface( pModel )
    { };

    virtual void operator() ( Script & script ) const 
    {
        script.ScrReadString( L"InputNeuron" );
        ShapeId const id ( script.ScrReadUint() );
        tParameter const param( static_cast< tParameter >( script.ScrReadUint() ) );
        assert( param == tParameter::pulseRate );
        script.ScrReadSpecial( L'=' );
        float const fValue { CastToFloat( script.ScrReadFloat() ) };
        m_pModelWriterInterface->GetShapePtr<InputNeuron *>( id )->SetPulseFrequency( fHertz( fValue ) );
    }

private:
    NNetModelWriterInterface * m_pModelWriterInterface;
};

class WrapTriggerSound : public Script_Functor
{
public:
    WrapTriggerSound( NNetModelWriterInterface * const pNNetModel ) :
        m_pModelWriterInterface( pNNetModel )
    { };

    virtual void operator() ( Script & script ) const 
    {
        ShapeId const id      { script.ScrReadLong () };
        Neuron      * pNeuron { m_pModelWriterInterface->GetShapePtr<Neuron *>( id ) };
        Hertz   const freq    { script.ScrReadUlong() };
        script.ScrReadString( L"Hertz" );
        MilliSecs const msec { script.ScrReadUlong() };
        script.ScrReadString( L"msec" );
        SoundDescr oldValue { pNeuron->SetTriggerSound( SoundDescr{ true, freq, msec } ) };
    }

private:
    NNetModelWriterInterface * m_pModelWriterInterface;
};

void NNetModelStorage::prepareForReading( )
{
    SymbolTable::ScrDefConst( L"Description", new WrapDescription( & m_wstrDescription ) );
    SymbolTable::ScrDefConst( L"Protocol"   , new WrapProtocol(  ) );

#define DEF_NNET_FUNC(name) SymbolTable::ScrDefConst( L#name, new Wrap##name##( m_pModelWriterInterface ) )
    DEF_NNET_FUNC( GlobalParameter );
    DEF_NNET_FUNC( NrOfShapes );
    DEF_NNET_FUNC( CreateShape );
    DEF_NNET_FUNC( ShapeParameter );
    DEF_NNET_FUNC( MarkShape );
    DEF_NNET_FUNC( TriggerSound );
#undef DEF_NET_FUNC

    ShapeType::Apply2All
    ( 
        [&]( ShapeType const & type ) 
        {
            SymbolTable::ScrDefConst
            ( 
                ShapeType::GetName( type.GetValue() ), 
                static_cast<unsigned long>(type.GetValue()) 
            );
        }
    );

    ///// Legacy /////
    SymbolTable::ScrDefConst( L"pipeline", static_cast<unsigned long>(ShapeType::Value::pipe) );  // support older protocol version
                                                                                                  ///// end Legacy /////

    Apply2AllParameters
    ( 
        [&]( tParameter const & param ) 
        {
            SymbolTable::ScrDefConst
            ( 
                GetParameterName( param ), 
                static_cast<unsigned long>( param ) 
            );
        }
    );

#undef DEF_NNET_FUNC

    m_bPreparedForReading = true;
}

bool NNetModelStorage::readModel( ) 
{
    if ( ProcessNNetScript( m_pScript, m_pModelWriterInterface, m_wstrPathOfNewModel ) )
    {
        m_pDescription->SetDescription( m_wstrDescription );
        m_wstrPathOfOpenModel = m_wstrPathOfNewModel;
        m_pModelWriterInterface->StaticModelChanged();
        setUnsavedChanges( false );
        m_pResult->Reaction( ReadModelResult::tResult::ok );
        return true;
    }
    else
    {
        m_pResult->Reaction( ReadModelResult::tResult::errorInFile );
        return false;
    }
}

static unsigned int __stdcall readModelThreadProc( void * data ) 
{
    SetThreadDescription( GetCurrentThread(), L"ReadModel" );
    reinterpret_cast<NNetModelStorage *>( data )->readModel( );
    return 0;
}

bool NNetModelStorage::Read( bool bConcurrently, wstring const wstrPath )
{
    bool bRes { true };
    if ( ! m_bPreparedForReading )
        prepareForReading( );

    m_wstrPathOfNewModel = ( wstrPath == L"" ) ? m_wstrPathOfOpenModel : wstrPath;
    if ( exists( m_wstrPathOfNewModel ) )
    {
        m_pModelWriterInterface->ResetModel();
        m_wstrDescription.clear();
        wcout << L"*** Reading file " << m_wstrPathOfNewModel << endl;
        if ( bConcurrently )
        {
            UINT threadId { 0 };
            Util::RunAsAsyncThread( readModelThreadProc, static_cast<void *>(this), & threadId );
        }
        else
        {
            bRes = readModel( );
        }
    }
    else
    {
        m_pResult->Reaction( ReadModelResult::tResult::fileNotFound, wstrPath );
        bRes = false;
    }
    return bRes;
}

void NNetModelStorage::ReadAsync( wstring const wstrPath )
{
    Read( true, wstrPath );
}
