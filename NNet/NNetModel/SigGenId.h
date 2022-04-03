// SigGenId.h
//
// NNetModel

#pragma once

#include "NamedType.h"

using SigGenId = NamedType<int, struct SigGenIdParam>;

inline SigGenId const NO_SIGGEN(-1);

static bool IsDefined(SigGenId const id) 
{ 
	return id != NO_SIGGEN; 
}

static bool IsUndefined(SigGenId const id) 
{ 
	return id == NO_SIGGEN; 
}
