// NNetModel.h 
//
// NNetModel

#pragma once

#include "util.h"
#include "MoreTypes.h"
#include "observable.h"
#include "ParameterType.h"
#include "NNetParameters.h"
#include "ModelDescription.h"
#include "MonitorData.h"
#include "UPNobList.h"
#include "Pipe.h"
#include "BaseKnot.h"

using std::unique_ptr;
using std::move;

class NNetModel
{
public:

	// const functions

	bool operator==(NNetModel const &) const;

	template <Nob_t T>
	T GetNobConstPtr(NobId const id) const
	{
		Nob const * const pNob { GetConstNob(id) };
		return (pNob && HasType<T>(* pNob)) ? static_cast<T>(pNob) : nullptr;
	}

	void CheckModel() const;
	void DumpModel (char const * const, int const) const;

	Nob const * GetConstNob (NobId const) const;

	fMicroSecs GetSimulationTime () const { return m_timeStamp; }

	float GetParameter(ParamType::Value const p) const { return m_param.GetParameterValue(p); }

	MicroMeterPnt GetNobPos      (NobId const id) const { return GetNobConstPtr<Nob  const *>(id)->GetPos         (); }
	BaseKnot    * GetStartKnotPtr(NobId const id) const { return GetNobConstPtr<Pipe const *>(id)->GetStartKnotPtr(); }
	BaseKnot    * GetEndKnotPtr  (NobId const id) const { return GetNobConstPtr<Pipe const *>(id)->GetEndKnotPtr  (); }

	NobId GetStartKnotId(NobId const idPipe) const { return GetStartKnotPtr(idPipe)->GetId(); }
	NobId GetEndKnotId  (NobId const idPipe) const { return GetEndKnotPtr  (idPipe)->GetId(); }

	UPNobList   const & GetUPNobs()        const { return m_Nobs; }
	MonitorData const & GetMonitorData()   const { return m_monitorData; }
	Param       const & GetParams()        const { return m_param; }
	wstring             GetModelFilePath() const { return m_wstrModelFilePath; }
	size_t              Size()             const { return m_Nobs.Size(); }

	NobId FindNobAt(MicroMeterPnt const &, NobCrit const &) const;
	bool  GetDescriptionLine(int const, wstring &)          const;

	// non const functions

	bool Compute();

	void  RecalcAllNobs();
	void  ClearDynamicData();
	void  ResetModel();
	float SetParam(ParamType::Value const, float const);
	void  SelectSubtree(BaseKnot * const, bool const);
	void  Reconnect(NobId const) const;

	UPNobList   & GetUPNobs()      { return m_Nobs; }
	MonitorData & GetMonitorData() { return m_monitorData; }
	Param       & GetParams()      { return m_param; }

	void DeselectAllNobs     ()                     { m_Nobs.SelectAllNobs(false); }
	void SetModelFilePath    (wstring const & wstr) { m_wstrModelFilePath = wstr; }
	void AddDescriptionLine  (wstring const & wstr) { m_description.AddDescriptionLine(wstr); }
	void DescriptionComplete ()                     { m_description.DescriptionComplete(); }
	void SetDescriptionUI    (DescriptionUI & i)    { m_description.SetDescriptionUI(i); }
	void SetHighSigObservable(Observable * obs)     { m_monitorData.SetHighSigObservable(obs); }
	void SetSimulationTime   (fMicroSecs const newVal = 0._MicroSecs) { m_timeStamp = newVal; }

private:

	UPNobList        m_Nobs;
	ModelDescription m_description;
	MonitorData      m_monitorData;
	Param            m_param;
	fMicroSecs       m_timeStamp         { 0._MicroSecs };
	wstring          m_wstrModelFilePath { L"" };
};
