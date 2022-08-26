// WrapGlobalParameter.ixx
//
// ModelIO

module;

#include <iostream>
#include "NNetModelWriterInterface.h"
#include "NNetModelReaderInterface.h"
#include "NNetModelIO.h"

export module WrapGlobalParameter;

import SaveCast;
import ParamType;
import NNetWrapperBase;
import Script;

using std::wostream;
using std::endl;

export class WrapGlobalParameter : public NNetWrapperBase
{
public:
    using NNetWrapperBase::NNetWrapperBase;

    void operator() (Script& script) const final
    {
        auto const param(static_cast<ParamType::Value>(script.ScrReadUint()));
        script.ScrReadSpecial(L'=');
        float const fValue{ Cast2Float(script.ScrReadFloat()) };
        m_modelIO.GetImportNMWI().SetParam(param, fValue);
    }

    void Write(wostream & out) const final
    {
        ParamType::Apply2GlobalParameters
        (
            [this, &out](ParamType::Value const& par)
            {
                out << L"GlobalParameter" << par << L" = "
                    << m_modelIO.GetExportNMRI().GetParameter(par)
                    << endl;
            }
        );
    };
};
