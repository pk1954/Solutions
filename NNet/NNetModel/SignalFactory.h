// SignalFactory.h : 
//
// NNetModel

#pragma once

#include "NNetParameters.h"
#include "Observable.h"
#include "ShapeId.h"
#include "AnimationInterface.h"
#include "Signal.h"

using std::unique_ptr;
using std::make_unique;

class NNetModelReaderInterface;

class SignalFactory
{
public:

    virtual ~SignalFactory() {};

    static void Initialize
    ( 
        NNetModelReaderInterface const & modelReaderInterface,
        Param                    const & param,
        Observable                     & observable,
        AnimationInterface             & animationInterface
    )
    {
        m_pMRI                = & modelReaderInterface;
        m_pParams             = & param;
        m_pObservable         = & observable;
        m_pAnimationInterface = & animationInterface;
    }

    static unique_ptr<Signal>MakeSignal( Signal const & src )
    {
        return make_unique<Signal>( src );
    }

    static unique_ptr<Signal>MakeSignal( MicroMeterCircle const & umCircle )
    {
        return make_unique<Signal>( * m_pMRI, * m_pParams, * m_pObservable, * m_pAnimationInterface, umCircle );
    }

private:
    inline static NNetModelReaderInterface const * m_pMRI                { nullptr };
    inline static Param                    const * m_pParams             { nullptr };
    inline static Observable                     * m_pObservable         { nullptr };
    inline static AnimationInterface             * m_pAnimationInterface { nullptr };
};
