// NNetWrapperHelpers.cpp : wrapper functions for unit tests and scripting of application
//
// NNetWindows

#include "stdafx.h"
#include "ERRHNDL.H"
#include "SCRIPT.H"
#include "NobId.h"
#include "NNetWrapperHelpers.h"

MicroMeter const ScrReadMicroMeter(Script & script)
{
	float const fValue = Cast2Float(script.ScrReadFloat());
	if (fabs(fValue) > MAX_MICRO_METER.GetValue())
		throw ScriptErrorHandler::ScriptException(777, L"MicroMeter value too big");
	return MicroMeter(fValue);
}

MicroMeterPnt const ScrReadMicroMeterPnt(Script & script)
{
	script.ScrReadSpecial(MicroMeterPnt::OPEN_BRACKET);
	MicroMeter const x(ScrReadMicroMeter(script));
	script.ScrReadSpecial(MicroMeterPnt::SEPARATOR);
	MicroMeter const y(ScrReadMicroMeter(script));
	script.ScrReadSpecial(MicroMeterPnt::CLOSE_BRACKET);
	return MicroMeterPnt(x, y);
}

MicroMeterCircle const ScrReadMicroMeterCircle(Script & script)
{
	script.ScrReadSpecial(MicroMeterCircle::OPEN_BRACKET);
	MicroMeterPnt umCenter { ScrReadMicroMeterPnt(script) };
	script.ScrReadSpecial(MicroMeterCircle::SEPARATOR);
	MicroMeter      umRadius { ScrReadMicroMeter(script) };
	script.ScrReadSpecial(MicroMeterCircle::CLOSE_BRACKET);
	return MicroMeterCircle(umCenter, umRadius);
}

MicroMeterPosDir const ScrReadMicroMeterPosDir(Script & script)
{
	script.ScrReadSpecial(MicroMeterPosDir::OPEN_BRACKET);
	MicroMeterPnt const umPnt(ScrReadMicroMeterPnt(script));
	script.ScrReadSpecial(MicroMeterPosDir::SEPARATOR);
	Radian const rad(Cast2Float(script.ScrReadFloat()));
	script.ScrReadSpecial(MicroMeterPosDir::CLOSE_BRACKET);
	return MicroMeterPosDir(umPnt, rad);
}

MicroMeterPntVector const ScrReadMicroMeterPntVector(Script& script)
{
	MicroMeterPntVector umPntVector;
	script.ScrReadSpecial(MicroMeterPntVector::OPEN_BRACKET);
	int const iNrOfElements { script.ScrReadInt() };
	script.ScrReadSpecial(L':');
	for (int i = 0;;)
	{
		umPntVector.Add(ScrReadMicroMeterPosDir(script));
		if (++i == iNrOfElements)
			break;
		script.ScrReadSpecial(MicroMeterPntVector::SEPARATOR);
	}
	script.ScrReadSpecial(MicroMeterPntVector::CLOSE_BRACKET);
	return umPntVector;
}

NobId const ScrReadNobId(Script& script)
{
	return NobId(script.ScrReadLong());
}

NobType const ScrReadNobType(Script& script)
{
	unsigned int uiVal { script.ScrReadUint() };
	if (uiVal >= NobType::NR_OF_NOB_TYPES)
		throw ScriptErrorHandler::ScriptException(998, wstring(L"Wrong nob type") );
	return static_cast<NobType::Value>(uiVal);
}

unique_ptr<NobIdList> const ScrReadNobIdList(Script& script)
{
	unique_ptr<NobIdList> upNobIds  { make_unique<NobIdList>() };
	script.ScrReadSpecial(NobIdList::OPEN_BRACKET);
	int const iNrOfElements { script.ScrReadInt() };
	script.ScrReadSpecial(L':');
	for (int i = 0;;)
	{
		upNobIds->Push(ScrReadNobId(script));
		if (++i == iNrOfElements)
			break;
		script.ScrReadSpecial(NobIdList::SEPARATOR);
	}
	script.ScrReadSpecial(NobIdList::CLOSE_BRACKET);
	return move(upNobIds);
}

ParamType::Value const ScrReadParamType(Script& script)
{
	unsigned long const ulParamType { script.ScrReadUlong() };
	if (ulParamType > static_cast<unsigned long>(ParamType::Value::tParameterLastGlobal))
		throw ScriptErrorHandler::ScriptException(999, wstring(L"unknown parameter type"));
	ParamType::Value param { static_cast<ParamType::Value>(ulParamType) };
	return param;
}