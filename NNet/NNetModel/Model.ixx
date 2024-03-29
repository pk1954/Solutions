// Model.ixx
//
// NNetModel

module;

#include <memory>
#include <string>

export module NNetModel:Model;

import Observable;
import Util;
import Signals;
import DescriptionUI;
import :ParamType;
import :ModelDescription;
import :UPNobList;
import :UPSigGenList;
import :UPSensorList;
import :PosNob;

using std::unique_ptr;
using std::wstring;
using std::move;

export class Model
{
public:

	Model();

	// const functions

	template <Nob_t T>
	T GetNobConstPtr(NobId const id) const
	{
		Nob const* const pNob { GetConstNob(id) };
		return (pNob && HasType<T>(*pNob)) ? static_cast<T>(pNob) : nullptr;
	}

	void CheckModel() const;
	void CheckId(NobId const) const;
	void DumpModel(char const* const, int const) const;

	float GetParameter(ParamType::Value const p) const { return m_upParam->GetParameterValue(p); }

	Nob    const * GetConstNob   (NobId const) const;
	PosNob const * GetStartNobPtr(NobId const) const;
	PosNob const * GetEndNobPtr  (NobId const) const;

	NobId GetStartKnotId(NobId const) const;
	NobId GetEndKnotId  (NobId const) const;

	wstring GetModelFilePath() const { return m_wstrModelFilePath; }
	size_t  Size()             const { return m_upNobs->Size(); }

	bool  GetDescriptionLine(int const, wstring&) const;

	NobId FindNobAt(MicroMeterPnt const& umPoint, auto const& crit) const // TODO: Template!
	{
		NobId idRes{ NO_NOB };

		idRes = m_upNobs->FindNobAt(umPoint, [&crit](Nob const& s) { return s.IsIoConnector() && crit(s); });
		if (IsDefined(idRes))
			return idRes;

		idRes = m_upNobs->FindNobAt(umPoint, [&crit](Nob const& s) { return s.IsNeuron() && crit(s); });
		if (IsDefined(idRes))
			return idRes;

		idRes = m_upNobs->FindNobAt(umPoint, [&crit](Nob const& s) { return s.IsIoLine() && (!s.HasParentNob()) && crit(s); });
		if (IsDefined(idRes))
			return idRes;

		idRes = m_upNobs->FindNobAt(umPoint, [&crit](Nob const& s) { return s.IsKnot() && crit(s); });
		if (IsDefined(idRes))
			return idRes;

		idRes = m_upNobs->FindNobAt(umPoint, [&crit](Nob const& s) { return s.IsSynapse() && crit(s); });
		if (IsDefined(idRes))
			return idRes;

		idRes = m_upNobs->FindNobAt(umPoint, [&crit](Nob const& s) { return s.IsFork() && crit(s); });
		if (IsDefined(idRes))
			return idRes;

		idRes = m_upNobs->FindNobAt(umPoint, [&crit](Nob const& s) { return s.IsPipe() && crit(s); });
		if (IsDefined(idRes))
			return idRes;

		return NO_NOB;
	}

	// functions affecting Nob data, not model data

	void ClearDynamicData();

	// access functions to members 

	UPSigGenList     const & GetSigGenList  () const { return *m_upSigGenList.get(); }
	UPSigGenList           & GetSigGenList  ()       { return *m_upSigGenList.get(); }
	UPSensorList     const & GetSensorList  () const { return m_sensorList; }
	UPSensorList           & GetSensorList  ()       { return m_sensorList; }
	UPNobList        const & GetUPNobs      () const { return *m_upNobs.get(); }
	UPNobList              & GetUPNobs      ()       { return *m_upNobs.get(); }
	unique_ptr<UPNobList>    MoveUPNobs     ()       { return move(m_upNobs); }
	MonitorData      const & GetMonitorData () const { return m_monitorData; }
	MonitorData            & GetMonitorData ()       { return m_monitorData; }
	NNetParameters   const & GetParams      () const { return *m_upParam.get(); }
	NNetParameters         & GetParams      ()       { return *m_upParam.get(); }
	SignalParameters const & GetSignalParams() const { return m_signalParams; }
	SignalParameters       & GetSignalParams()       { return m_signalParams; }

	SignalGenerator const* GetSigGen(SigGenId const sigGenId) const
	{									  
		return m_upSigGenList->GetSigGen(sigGenId);
	}									  
	
	// non const functions

	Nob * GetNob(NobId const);

	bool Compute();
	void ResetModel();
	void SetParam(ParamType::Value const, float const);
	void Reconnect(NobId const);

	void DeselectAllNobs          ()               const { m_upNobs->DeselectAllNobs(); }
	void SetModelFilePath         (wstring const & wstr) { m_wstrModelFilePath = wstr; }
	void AddDescriptionLine       (wstring const & wstr) { m_description.AddDescriptionLine(wstr); }
	void DescriptionComplete      ()                     { m_description.DescriptionComplete(); }
	void SetDescriptionUI         (DescriptionUI &i)     { m_description.SetDescriptionUI(i); }
	void SetHighSigObservable     (Observable    &o)     { m_monitorData.SetHighSigObservable(o); }
	void SetActiveSigGenObservable(Observable    &o)     { m_upSigGenList->SetActiveSigGenObservable(o); }

private:
	unique_ptr<UPNobList>      m_upNobs;
	unique_ptr<UPSigGenList>   m_upSigGenList;
	unique_ptr<NNetParameters> m_upParam;
	SignalParameters           m_signalParams;
	UPSensorList               m_sensorList;
	ModelDescription           m_description;
	MonitorData                m_monitorData;
	wstring                    m_wstrModelFilePath { L"" };
};						    
