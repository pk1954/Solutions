// NNetModelCommands.cpp
//
// NNetModel

#include "stdafx.h"
#include <source_location>
#include "AddNobsCommand.h"
#include "AddPipe2BaseKnotCmd.h"
#include "AddPipe2PipeCommand.h"
#include "AddSignalCommand.h"
#include "AnalyzeCommand.h"
#include "Analyzer.h"
#include "ArrowAnimation.h"
#include "ClearBeepersCommand.h"
#include "CommandStack.h"
#include "CommandFunctions.h"
#include "ConnAnimationCommand.h"
#include "Connect2BaseKnotCommand.h"
#include "Connect2PipeCommand.h"
#include "CoordAnimation.h"
#include "CopySelectedNobs.h"
#include "DeleteSelectionCommand.h"
#include "DeleteSigGenCmd.h"
#include "DeleteSignalCommand.h"
#include "DeleteTrackCommand.h"
#include "DeleteBaseKnotCmd.h"
#include "DiscIoConnectorCmd.h"
#include "InputConnector.h"
#include "InsertBaseKnotCommand.h"
#include "InsertTrackCommand.h"
#include "MoveSensorCmd.h"
#include "MoveSignalCmd.h"
#include "MoveNobCommand.h"
#include "MoveSelectionCommand.h"
#include "NewIoNeuronPairCmd.h"
#include "NewSigGenCmd.h"
#include "NNetModelImporter.h"
#include "Uniform2D.h"
#include "PlugIoConnectorAnimation.h"
#include "PlugIoNeuronAnimation.h"
#include "RenameSigGenCmd.h"
#include "RestrictSelectionCommand.h"
#include "RotateNobCommand.h"
#include "RotateModelCommand.h"
#include "RotateSelectionCommand.h"
#include "SelectAllBeepersCommand.h"
#include "SelectAllCommand.h"
#include "SelectionCommand.h"
#include "SelectNobCommand.h"
#include "SelectNobsInRectCommand.h"
#include "SelectSubtreeCommand.h"
#include "SetSigGenDataCmd.h"
#include "SetHighlightedSignalCmd.h"
#include "SelSigGenClientsCmd.h"
#include "SetParameterCommand.h"
#include "SetNobCommand.h"
#include "SetSigGenCmd.h"
#include "SetTriggerSoundCommand.h"
#include "SizeSelectionCmd.h"
#include "SizeSensorCmd.h"
#include "SplitNeuronCmd.h"
#include "ToggleEmphModeCmd.h"
#include "ToggleStopOnTriggerCommand.h"
#include "NNetModelWriterInterface.h"
#include "NNetParameters.h"
#include "NNetModelCommands.h"

using std::wcout;
using std::endl;
using std::move;
using std::make_unique;
using std::unique_ptr;
using std::source_location;

void NNetModelCommands::Initialize
(
	NNetModelImporter * const pModelImporter,
	Observable        * const pDynamicModelObservable,
	CommandStack      * const pCmdStack
) 
{ 
	m_pModelImporter          = pModelImporter;
	m_pDynamicModelObservable = pDynamicModelObservable;
	m_pCmdStack               = pCmdStack;
}

void NNetModelCommands::SetModelInterface
(
	NNetModelWriterInterface * const pNMWI
) 
{ 
	m_pNMWI = pNMWI;
}

void NNetModelCommands::UndoCommand()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	if (! m_pCmdStack->UndoCommand())
		MessageBeep(MB_ICONWARNING);
}

void NNetModelCommands::RedoCommand()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	if (! m_pCmdStack->RedoCommand())
		MessageBeep(MB_ICONWARNING);
}

void NNetModelCommands::PushCommand(unique_ptr<NNetCommand> upCmd)
{
	m_pCmdStack->PushCommand(move(upCmd));
}

void NNetModelCommands::ResetModel()
{ 
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pNMWI->ResetModel();
	m_pCmdStack->Clear();
	m_pDynamicModelObservable->NotifyAll(false);
}

void NNetModelCommands::AddIncoming2BaseKnot(NobId const id, MicroMeterPnt const & pos)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand(make_unique<AddPipe2BaseKnotCommand>(id, pos - STD_OFFSET, NobType::Value::inputNeuron));
}

void NNetModelCommands::AddIncoming2Pipe(NobId const id, MicroMeterPnt const & pos)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand(make_unique<AddPipe2PipeCommand>(id, pos, NobType::Value::inputNeuron));
}

void NNetModelCommands::AddModel()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() 
		              << L" \"" << m_pModelImporter->GetModelFileName() << L"\" " << endl;
	unique_ptr<NNetModel> upImportedModel { m_pModelImporter->GetImportedModel() };
	m_pCmdStack->PushCommand(make_unique<AddNobsCommand>(upImportedModel->GetUPNobs()));
}

void NNetModelCommands::AddOutgoing2BaseKnot(NobId const id, MicroMeterPnt const & pos)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand(make_unique<AddPipe2BaseKnotCommand>(id, pos + STD_OFFSET, NobType::Value::outputNeuron));
}

void NNetModelCommands::AddOutgoing2Pipe(NobId const id, MicroMeterPnt const & pos)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand(make_unique<AddPipe2PipeCommand>(id, pos, NobType::Value::outputNeuron));
}

void NNetModelCommands::AddSignal
(
	MicroMeterCircle const & umCircle,
	TrackNr          const   trackNr
)
{ 
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << umCircle << L" " << trackNr << endl;
	m_pCmdStack->PushCommand(make_unique<AddSignalCommand>(umCircle, trackNr));
}

void NNetModelCommands::AnalyzeAnomalies()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<AnalyzeCommand>(ModelAnalyzer::FindAnomaly(*m_pNMWI)));
}

void NNetModelCommands::AnalyzeLoops()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<AnalyzeCommand>(ModelAnalyzer::FindLoop(*m_pNMWI)));
}

void NNetModelCommands::AnimateCoord
(
	Uniform2D<MicroMeter>       & actual, 
	Uniform2D<MicroMeter> const & target
)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<CoordAnimation>(actual, target));
}

void NNetModelCommands::AnimateArrows
(
	MicroMeter     & umActual, 
	MicroMeter const umTarget
)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<ArrowAnimation>(umActual, umTarget));
}

void NNetModelCommands::ClearBeepers()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << endl;
	m_pCmdStack->PushCommand(make_unique<ClearBeepersCommand>());
}

void NNetModelCommands::Connect
(
	NobId const idSrc, 
	NobId const idDst
)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << idSrc << L" " << idDst << endl;

	unique_ptr<Command> upCmd;
	switch (m_pNMWI->GetNobType(idDst).GetValue())
	{
	using enum NobType::Value;
	case pipe:
		upCmd = make_unique<Connect2PipeCommand>(idSrc, idDst);
		break;
	case knot:
	case neuron:
		upCmd = make_unique<Connect2BaseKnotCommand>(idSrc, idDst);
		break;
	case inputNeuron:
		if (m_pNMWI->GetNobType(idSrc).IsKnotType())  // connect knot to output neuron
			upCmd = make_unique<Connect2BaseKnotCommand>(idSrc, idDst);
		else if (m_pNMWI->GetNobType(idSrc).IsOutputNeuronType())
		{
			if (m_pNMWI->HasIncoming(idSrc) && m_pNMWI->HasOutgoing(idDst))
				upCmd = make_unique<PlugIoNeuronAnimation>(idSrc, idDst);
			else
				upCmd = make_unique<Connect2BaseKnotCommand>(idSrc, idDst);
		}
		else
			assert(false);
		break;
	case outputNeuron:
		if (m_pNMWI->GetNobType(idSrc).IsKnotType())  // connect knot to output neuron
			upCmd = make_unique<Connect2BaseKnotCommand>(idSrc, idDst);
		else if (m_pNMWI->GetNobType(idSrc).IsOutputNeuronType())  // connect two output neurons
			upCmd = make_unique<Connect2BaseKnotCommand>(idSrc, idDst);
		else if (m_pNMWI->GetNobType(idSrc).IsInputNeuronType())
		{
			if (m_pNMWI->HasOutgoing(idSrc) && m_pNMWI->HasIncoming(idDst))
				upCmd = make_unique<PlugIoNeuronAnimation>(idSrc, idDst);
			else
				upCmd = make_unique<Connect2BaseKnotCommand>(idSrc, idDst);
		}
		else
			assert(false);
		break;
	case inputConnector:
	case outputConnector:
		upCmd = make_unique<PlugIoConnectorAnimation>(idSrc, idDst); 
		break;
	default:
		assert(false);
	}
	m_pCmdStack->PushCommand(move(upCmd));
}

void NNetModelCommands::CopySelection()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<AddNobsCommand>(CopySelectedNobs::Do(*m_pNMWI)));
}

void NNetModelCommands::CreateInitialNobs()
{ 
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pNMWI->CreateInitialNobs();
	m_pDynamicModelObservable->NotifyAll(false);
}

void NNetModelCommands::DeleteSignal(SignalId const & id)
{ 
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<DeleteSignalCommand>(id));
}

void NNetModelCommands::DeleteSelection()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << endl;
	m_pCmdStack->PushCommand(make_unique<DeleteSelectionCommand>());
}

void NNetModelCommands::DeleteNob(NobId const id)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << endl;
	m_pCmdStack->PushCommand(MakeDeleteCommand(*m_pNMWI, *m_pNMWI->GetNob(id)));
}

void NNetModelCommands::DeleteTrack(TrackNr const nr)
{ 
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << nr << endl;
	m_pCmdStack->PushCommand(make_unique<DeleteTrackCommand>(nr));
}

void NNetModelCommands::DeleteBaseKnot(NobId const id)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << endl;
	Nob * pNob { m_pNMWI->GetNob(id) };
	if (! pNob)
		return;
	m_pCmdStack->PushCommand(make_unique<DeleteBaseKnotCmd>(*pNob));
}

void NNetModelCommands::DiscIoConnector(NobId const id)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << endl;
	Nob * pNob { m_pNMWI->GetNob(id) };
	if (! pNob)
		return;
	m_pCmdStack->PushCommand(make_unique<DiscIoConnectorCmd>(*pNob));
}

void NNetModelCommands::InsertTrack(TrackNr const nr)
{ 
	if (IsTraceOn())
	TraceStream() << source_location::current().function_name() << nr << endl;
	m_pCmdStack->PushCommand(make_unique<InsertTrackCommand>(nr));
}

bool NNetModelCommands::MakeIoConnector()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	unique_ptr<ConnAnimationCommand> upCmd = { make_unique<ConnAnimationCommand>() };
	bool bCmdOk = upCmd->IsCmdOk();
	m_pCmdStack->PushCommand(move(upCmd));
	return bCmdOk;
}

void NNetModelCommands::SplitNeuron(NobId const id)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << endl;
	m_pCmdStack->PushCommand(make_unique<SplitNeuronCmd>(id));
}

void NNetModelCommands::ToggleStopOnTrigger(NobId const id)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << endl;
	m_pCmdStack->PushCommand(make_unique<ToggleStopOnTriggerCommand>(id));
}

SignalId NNetModelCommands::SetHighlightedSignal(MicroMeterPnt const & umPos)
{
	return m_pNMWI->GetMonitorData().SetHighlightedSignal(umPos);
}

void NNetModelCommands::SetTriggerSound(NobId const id, SoundDescr const & sound)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << sound.m_bOn << L" " << sound.m_frequency << L" " << sound.m_duration << endl;
	m_pCmdStack->PushCommand(make_unique<SetTriggerSoundCommand>(id, sound));
}

void NNetModelCommands::SetParameter(ParamType::Value const param, float const fNewValue)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << ParamType::GetName(param) << L" " << fNewValue << endl;
	m_pCmdStack->PushCommand(make_unique<SetParameterCommand>(m_pNMWI->GetParams(), param, fNewValue));
}

void NNetModelCommands::SetSigGenData(SignalGenerator & dst, SigGenData const &data)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<SetSigGenDataCmd>(dst, data));
}

void NNetModelCommands::MoveNob(NobId const id, MicroMeterPnt const & delta)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << delta << endl;
	m_pCmdStack->PushCommand(make_unique<MoveNobCommand>(*m_pNMWI->GetNob(id), delta));
}

void NNetModelCommands::MoveSensor(SignalId const & id, MicroMeterPnt const & delta)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << delta << endl;
	m_pCmdStack->PushCommand(make_unique<MoveSensorCmd>(id, delta));
}

void NNetModelCommands::MoveSignal(SignalId const & id, TrackNr const trackNr)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << trackNr << endl;
	m_pCmdStack->PushCommand(make_unique<MoveSignalCmd>(id, trackNr));
}

void NNetModelCommands::NewSignalGenerator()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<NewSigGenCmd>());
}

void NNetModelCommands::SetSignalGenerator(SigGenId const id)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << id << endl;
	m_pCmdStack->PushCommand(make_unique<SetSigGenCmd>(id));
}

void NNetModelCommands::DeleteSigGen()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<DeleteSigGenCmd>());
}

void NNetModelCommands::RenameSigGen()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<RenameSigGenCmd>());
}

void NNetModelCommands::SelectSigGenClients()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<SelSigGenClientsCmd>());
}

void NNetModelCommands::SizeSelection(float const fFactor)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << fFactor << endl;
	m_pCmdStack->PushCommand(make_unique<SizeSelectionCmd>(fFactor));
}

void NNetModelCommands::SizeSensor(SignalId const & id, float const fFactor)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << fFactor << endl;
	m_pCmdStack->PushCommand(make_unique<SizeSensorCmd>(id, fFactor));
}

void NNetModelCommands::Rotate
(
	NobId         const   id, 
	MicroMeterPnt const & umPntOld, 
	MicroMeterPnt const & umPntNew 
)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << umPntOld << L" " << umPntNew << endl;
	m_pCmdStack->PushCommand(make_unique<RotateNobCommand>(*m_pNMWI->GetNob(id), umPntOld, umPntNew));
}

void NNetModelCommands::SetNob(NobId const id, MicroMeterPosDir const posDir)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << posDir << endl;
	m_pCmdStack->PushCommand(make_unique<SetNobCommand>(*m_pNMWI->GetNob(id), posDir));
}

void NNetModelCommands::MoveSelection(MicroMeterPnt const & delta)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << delta << endl;
	m_pCmdStack->PushCommand(make_unique<MoveSelectionCommand>(delta));
}

void NNetModelCommands::RotateSelection(MicroMeterPnt const & umPntOld, MicroMeterPnt const & umPntNew)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << umPntOld << umPntNew << endl;
	m_pCmdStack->PushCommand(make_unique<RotateSelectionCommand>(umPntOld, umPntNew));
}

void NNetModelCommands::RotateModel(MicroMeterPnt const & umPntOld, MicroMeterPnt const & umPntNew)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << umPntOld << umPntNew << endl;
	m_pCmdStack->PushCommand(make_unique<RotateModelCommand>(umPntOld, umPntNew));
}

void NNetModelCommands::InsertKnot(NobId const id, MicroMeterPnt const & pos)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand(make_unique<InsertBaseKnotCommand<Knot>>(id, pos));
}

void NNetModelCommands::InsertNeuron(NobId const id, MicroMeterPnt const & pos)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << pos << endl;
	m_pCmdStack->PushCommand(make_unique<InsertBaseKnotCommand<Neuron>>(id, pos));
}

void NNetModelCommands::NewIoNeuronPair(MicroMeterPnt const & pos)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << pos << endl;
	m_pCmdStack->PushCommand(make_unique<NewIoNeuronPairCmd>(* m_pNMWI, pos));
}

void NNetModelCommands::RestrictSelection(NobType::Value const val)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << val << endl;
	m_pCmdStack->PushCommand(make_unique<RestrictSelectionCommand>(val));
}

void NNetModelCommands::SelectAllBeepers()
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<SelectAllBeepersCommand>());
}

void NNetModelCommands::SelectNob(NobId const id, tBoolOp const op)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << op << endl;
	if (IsDefined(id))
		m_pCmdStack->PushCommand(make_unique<SelectNobCommand>(id, op));
}

void NNetModelCommands::SelectAll(bool const bOn)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << bOn << endl;
	m_pCmdStack->PushCommand(make_unique<SelectAllCommand>(bOn));
}

void NNetModelCommands::SelectSubtree(NobId const id, bool  const bOn)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << id << L" " << bOn << endl;
	m_pCmdStack->PushCommand(make_unique<SelectSubtreeCommand>(id, bOn));
}

void NNetModelCommands::SelectNobsInRect(MicroMeterRect const & rect)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << L" " << rect << endl;
	m_pCmdStack->PushCommand(make_unique<SelectNobsInRectCommand>(rect));
}

void NNetModelCommands::ToggleEmphMode(NobId const id)
{
	if (IsTraceOn())
		TraceStream() << source_location::current().function_name() << endl;
	m_pCmdStack->PushCommand(make_unique<ToggleEmphModeCmd>(id));
}

void NNetModelCommands::TriggerStimulus(NobId const id)  //TODO
{  
	//if (IsTraceOn())
	//	TraceStream() << source_location::current().function_name() << endl;
	//m_pNMWI->GetNobPtr<InputConnector *>(id)->TriggerStimulus();
}