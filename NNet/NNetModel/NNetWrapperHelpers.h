// NNetWrapperHelpers.h : 
//
// NNetModel

#pragma once

#include "Script.h"
#include "SignalId.h"
#include "NobType.h"
#include "NobIdList.h"
#include "SigGenStaticData.h"
#include "ParameterType.h"
#include "MicroMeterPntVector.h"

import MoreTypes;
import VoltageType;

extern MicroMeter            ScrReadMicroMeter         (Script &);
extern MicroMeterPnt         ScrReadMicroMeterPnt      (Script &);
extern MicroMeterCircle      ScrReadMicroMeterCircle   (Script &);
extern MicroMeterPntVector   ScrReadMicroMeterPntVector(Script &);
extern unique_ptr<NobIdList> ScrReadNobIdList          (Script &);
extern NobId                 ScrReadNobId              (Script &);
extern SignalId              ScrReadSignalId           (Script &);
extern NobType               ScrReadNobType            (Script &);
extern ParamType::Value      ScrReadParamType          (Script &);
extern mV                    ScrReadVoltage            (Script &);
extern SigGenStaticData      ScrReadSigGenStaticData   (Script &);
