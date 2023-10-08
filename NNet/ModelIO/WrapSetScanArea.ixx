// WrapSetScanArea.ixx
//
// ModelIO

module;

#include <iostream>
#include <string>

export module WrapSetScanArea;

import IoUtil;
import NNetModelIO;
import Wrapper;

using std::wostream;
using std::wstring;
using std::endl;

export class WrapSetScanArea : public Wrapper
{
public:
    using Wrapper::Wrapper;

    void operator() (Script& script) const final
    {
        NNetModelIO::GetImportNMWI().SetScanArea(ScrReadMicroMeterRect(script));
    }

    void Write(wostream& out) const final
    {
        WriteCmdName(out);
        out << NNetModelIO::GetExportNMRI().GetScanAreaRect() << endl;
    }
};