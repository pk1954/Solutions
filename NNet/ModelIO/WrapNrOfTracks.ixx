// WrapNrOfTracks.ixx
//
// ModelIO

module;

#include <iostream>

export module WrapNrOfTracks;

import NNetWrapperBase;
import Script;
import NNetModelIO;
import NNetModel;

using std::wostream;
using std::endl;

export class WrapNrOfTracks : public NNetWrapperBase
{
public:
    using NNetWrapperBase::NNetWrapperBase;

    void operator() (Script& script) const final
    {
        unsigned int const uiNrOfTracks{ script.ScrReadUint() };
        for (unsigned int ui = 0; ui < uiNrOfTracks; ++ui)
            m_modelIO.GetImportNMWI().GetMonitorData().InsertTrack(TrackNr(0));
    }

    void Write(wostream& out) const final
    {
        WriteCmdName(out);
        out << m_modelIO.GetExportNMRI().GetMonitorDataC().GetNrOfTracks() << endl;
    };
};
