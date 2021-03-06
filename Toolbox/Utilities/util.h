// util.h : 
//

#pragma once

#include <functional>
#include <assert.h>
#include <string>
#include <limits>
#include <streambuf>
#include "NamedType.h"
#include "debug.h"

using std::function;
using std::abs;

extern void UpperCase( std::wstring & );

enum class tOnOffAuto : long
{
	on,
	off,
	automatic
};

extern bool ApplyAutoCriterion( tOnOffAuto const, function<bool()> );

using BYTES = NamedType< unsigned long long, struct BYTES_Parameter >;

// not needed, makes strange problems 
//template <typename SOURCE_TYPE> int sgn(SOURCE_TYPE val) 
//{
//    return (SOURCE_TYPE(0) < val) - (val < SOURCE_TYPE(0));
//}

inline bool const IsCloseToZero( float const f )
{
	return abs( f ) < 1.e-10f;
}

template<typename SOURCE_TYPE>
SOURCE_TYPE ClipToMinMax(SOURCE_TYPE const val, SOURCE_TYPE const min, SOURCE_TYPE const max)
{
	return ( val > max ) 
		   ? max 
		   : (val < min) 
		     ? min 
		     : val;
}

template<typename SOURCE_TYPE>
constexpr const float Cast2Float( SOURCE_TYPE const value ) 
{
	AssertFloat( value );
	return static_cast<float>( value );
}

template<typename SOURCE_TYPE>
constexpr const short Cast2Short( SOURCE_TYPE const value ) 
{
	AssertShort( value );
	return static_cast<short>( value );
}

template<typename SOURCE_TYPE>
constexpr const unsigned short Cast2UnsignedShort( SOURCE_TYPE const value ) 
{
	AssertUnsignedShort( value );
	return static_cast<unsigned short>( value );
}

template<typename SOURCE_TYPE>
constexpr const int Cast2Int( SOURCE_TYPE const value ) 
{
	AssertInt( value );
	return static_cast<int>( value );
}

template<typename SOURCE_TYPE>
constexpr const unsigned int Cast2UnsignedInt( SOURCE_TYPE const value ) 
{
	AssertUnsignedInt( value );
	return static_cast<unsigned int>( value );
}

template<typename SOURCE_TYPE>
constexpr const long Cast2Long( SOURCE_TYPE const value ) 
{
	AssertLong( value );
	return static_cast<long>( value );
}

template<typename SOURCE_TYPE>
constexpr const unsigned long Cast2UnsignedLong( SOURCE_TYPE const value ) 
{
	AssertUnsignedLong( value );
	return static_cast<unsigned long>( value );
}
