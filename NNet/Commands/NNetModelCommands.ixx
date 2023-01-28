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
        Observable       * const,
        Sound            * const,
        NNetCommandStack * const
    );
    void SetModelInterface(NNetModelWriterInterface* const);

    void PushCommand(unique_ptr<NNetCommand>);
    void UndoCommand();
    void RedoCommand();

    void AddModel();
    void AddSensor(MicroMeterCircle const&, TrackNr const);
    void AddSigGen2Monitor(TrackNr const);
    void AnimateArrows(MicroMeter&, MicroMeter const);
    void AnimateCoord (Uniform2D<MicroMeter>&, Uniform2D<MicroMeter> const&);
    void AttachSigGen2Conn(NobId const);
    void AttachSigGen2Line(NobId const);
    void Connect(NobId const, NobId const);
    void DeleteNob(NobId const);
    void DeleteSelection();
    void DeleteSensor(SensorId const&);
    void DeleteSignal(SignalId const&);
    void DeleteTrack(TrackNr const);
    void DiscIoConnector(NobId const);
    void InsertKnot  (NobId const, MicroMeterPnt const&);
    void InsertNeuron(NobId const, MicroMeterPnt const&);
    bool MakeIoConnector(NobId const, NobId const);
    void MoveSensor(SensorId const, MicroMeterPnt const&);
    void NewSignalGenerator();
    void Rotate(NobId const, MicroMeterPnt const&, MicroMeterPnt const&);
    void RotateModel(MicroMeterPnt const&, MicroMeterPnt const&);
    void RotateSelection(MicroMeterPnt const&, MicroMeterPnt const&);
    void SelectSigGenClients();
    void SetActiveSignalGenerator(SigGenId const);
    void SetParameter(ParamType::Value const, float const);
    void SetSigGenStaticData(SignalGenerator&, SigGenStaticData const&);
    void SetNob(NobId const, MicroMeterPosDir const);
    void SizeSelection(float const);
    void SizeSensor(SensorId const, float const);
    void ToggleEmphMode(NobId const);
    void ToggleStopOnTrigger(NobId const);
    void StartStimulus();

    SensorId SetHighlightedSensor(MicroMeterPnt const&);

private:

    wostream& TraceStream() { return wcout; }

    bool                       m_bTrace                  { true };
    CommandStack             * m_pCmdStack               { nullptr };
    NNetModelWriterInterface * m_pNMWI                   { nullptr };
    NNetModelIO              * m_pModelIO                { nullptr };
    Observable               * m_pDynamicModelObservable { nullptr };
    Sound                    * m_pSound                  { nullptr };
};