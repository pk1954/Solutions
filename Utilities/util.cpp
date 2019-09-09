// util.cpp
//

#include "stdafx.h"
#include <sstream> 
#include "util.h"

using std::wostringstream;

extern void UpperCase( std::wstring & str )
{
	for ( auto & c: str ) 
		c = toupper(c);
}

std::wstring DecFraction( unsigned long ulValue )
{
    wostringstream wBuffer;
    unsigned long const intPlaces = ulValue / 1000;
    unsigned long const decPlaces = ulValue - 1000 * intPlaces;
    wBuffer << intPlaces << L"." << decPlaces / 100;
	return wBuffer.str();
}

bool ApplyAutoCriterion
( 
	tOnOffAuto const onOffAuto,
	std::function<bool()> crit
)
{
	return ( onOffAuto == tOnOffAuto::on )
		? true
		: ( onOffAuto == tOnOffAuto::off )
		? false
		: crit();
}
