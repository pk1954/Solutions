// WrapProtocol.ixx
//
// ModelIO

module;

#include <iostream>
#include <iomanip>
#include <string>

export module WrapProtocol;

import NNetWrapperBase;
import NNetModelStorage;
import IoUtil;

using std::endl;
using std::wstring;
using std::wostream;
using std::setprecision;

export class WrapProtocol : public NNetWrapperBase
{
public:
    using NNetWrapperBase::NNetWrapperBase;

    inline static wstring const VERSION { L"version" };

    void operator() (Script& script) const final
    {
        script.ScrReadString(VERSION);
        double dVersion = script.ScrReadFloat();
        if (dVersion < NNetModelStorage::PROTOCOL_VERSION)
            throw ProtocollException(dVersion);
    }

    void Write(wostream& out) const final
    {
        WriteCmdName(out);
        out << VERSION << SPACE << setprecision(2) << NNetModelStorage::PROTOCOL_VERSION << endl;
        out << endl;
    };
};
