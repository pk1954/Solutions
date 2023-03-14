// NNetModelCommands.ixx
//
// Commands

module;

#include <vector>
#include <string>
#include <iostream>

export module NNetModelCommands;

import Observable;
import Types;
import BoolOp;
import Uniform2D;
import SoundInterface;
import Commands;
import NNetModelIO;
import NNetCommand;
import NNetCommandStack;
import NNetModel;

using std::wcout;
using std::wstring;
using std::wostream;
using std::vector;
using std::unique_ptr;

export class NNetModelCommands
{
public:
    void Initialize
    (
        NNetModelIO      * const,
        Sound            * const,
        NNetCommandStack * const
    );
    void SetModelInterface(NNetModelWriterInterface* const);

    void PushCommand(unique_ptr<NNetCommand>);
    void UndoCommand();
    void RedoCommand();

    void AddModel();
    void AddSensor(MicroMeterCircle const&, TrackNr const);
    void AnimateArrows(MicroMeter&, MicroMeter const);
    void AnimateCoord (Uniform2D<MicroMeter>&, Uniform2D<MicroMeter> const&);
    void Connect(NobId const, NobId const);
    void DeleteSelection();
    void DiscIoConnector(NobId const);
    bool MakeIoConnector(NobId const, NobId const);
    void MoveSensor(SensorId const, MicroMeterPnt const&);
    void SelectSigGenClients();
    void SetSigGenStaticData(SignalGenerator&, SigGenStaticData const&);
    void SetNob(NobId const, MicroMeterPosDir const);
    void StartStimulus();

    SensorId SetHighlightedSensor(MicroMeterPnt const&);

private:

    wostream& TraceStream() { return wcout; }

    bool                       m_bTrace                  { true };
    CommandStack             * m_pCmdStack               { nullptr };
    NNetModelWriterInterface * m_pNMWI                   { nullptr };
    NNetModelIO              * m_pModelIO                { nullptr };
    Sound                    * m_pSound                  { nullptr };
};