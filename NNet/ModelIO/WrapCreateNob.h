// WrapCreateNob.h 
//
// ModelIO

#pragma once

#include "WrapperBase.h"

class Script;

class WrapCreateNob : public WrapperBase
{
public:
    using WrapperBase::WrapperBase;

    void operator() (Script & script) const final
    {   
        createNob(script);
    }

private:

    Nob * createNob        (Script &) const;
    UPNob createPipe       (Script &) const;
    UPNob createBaseKnot   (Script &, NobType const) const;
    UPNob createIoConnector(Script &, NobType const) const; 
};