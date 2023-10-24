// HistorySystem.cpp
//


#include <cassert>
#include "HistorySystem.h"
#include "HistorySystemImpl.h"

HistorySystem * HistorySystem::CreateHistorySystem()
{
	return new HistorySystemImpl();
}

BYTES HistorySystem::GetSlotWrapperSize()
{ 
	return BYTES(sizeof(HistCacheItem));  
}
