// NNetWrapperHelpers.h : 
//
// NNetWindows

#pragma once

#include "script.h"
#include "moretypes.h"
#include "ShapeIdList.h"
#include "MicroMeterPointVector.h"

extern MicroMeter              ScrReadMicroMeter           (Script &);
extern MicroMeterPoint         ScrReadMicroMeterPoint      (Script &);
extern MicroMeterCircle        ScrReadMicroMeterCircle     (Script &);
extern MicroMeterPointVector   ScrReadMicroMeterPointVector(Script &);
extern unique_ptr<ShapeIdList> ScrReadShapeIdList          (Script &);
