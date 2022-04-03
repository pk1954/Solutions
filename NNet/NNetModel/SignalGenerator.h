// SignalGenerator.h
//
// NNetModel

#pragma once

#include "MoreTypes.h"
#include "observable.h"
#include "observerInterface.h"
#include "ParameterType.h"
#include "SigGenData.h"
#include "Stimulus.h"

class Param;
class UPSigGenList;
class SignalGenerator;

using std::wstring;
using std::unique_ptr;

using UPSigGen = unique_ptr<SignalGenerator>;

class SignalGenerator
{
public:

	explicit SignalGenerator(UPSigGenList &);

	void Tick();
	void TriggerStimulus() { m_stimulus.TriggerStimulus(); }
	void StopTrigger    () { m_stimulus.StopTrigger(); }

	fHertz GetFrequency(fMicroSecs const uSecs) const { return m_data.GetFrequency(uSecs); }
	mV     GetAmplitude(fMicroSecs const uSecs) const {	return m_data.GetAmplitude(uSecs); }
	fHertz GetActFrequency()                    const { return GetFrequency(TimeTilTrigger()); }
	mV     GetAmplitude   ()                    const {	return GetAmplitude(TimeTilTrigger());}

	fMicroSecs        const & TimePeak () const { return m_data.GetPeakTime(); }
	BASE_PEAK<fHertz> const & Frequency() const { return m_data.GetFreq();     }
	BASE_PEAK<mV>     const & Amplitude() const { return m_data.GetAmplit();   }

	void SetParam(ParamType::Value const, float const);

	void SetData(SigGenData const &);
	SigGenData GetData() const;

	void SetName(wstring const & name) { m_name = name; }
	wstring const & GetName() const { return m_name; }

	void SetFreqBase(fHertz const);
	void SetFreqPeak(fHertz const);
	void SetBaseVolt(mV const);
	void SetPeakVolt(mV const);
	void SetTimePeak(fMicroSecs const);
	void Register  (ObserverInterface &);
	void Unregister(ObserverInterface &);

	bool       IsTriggerActive() const { return m_stimulus.IsTriggerActive(); }
	fMicroSecs TimeTilTrigger () const { return m_stimulus.TimeTilTrigger(); }

	Param       & GetParams ();
	Param const & GetParamsC() const;

private:

	inline static int m_iCounter { 0 };

	UPSigGenList & m_list;
	Stimulus       m_stimulus;
	SigGenData     m_data;
	wstring        m_name;
};
