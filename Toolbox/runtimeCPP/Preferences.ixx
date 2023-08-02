// Preferences.ixx
//
// Toolbox\runtimeCPP

module;

#include <compare>
#include <string>
#include <iostream>
#include <vector>
#include <memory>

export module Preferences;

export import WrapBaseBool;

import SoundInterface;

using std::wostream;
using std::wstring;
using std::vector;
using std::unique_ptr;
using std::make_unique;

export class Preferences
{
public:
	static void Initialize(wstring const&);

	static bool ReadPreferences();
	static bool WritePreferences();
	
	static void AddWrapper(unique_ptr<WrapBase> upWrapper)
	{
		m_prefVector.push_back(move(upWrapper));
	}

	static void AddBoolWrapper(wstring const& name, BoolType& boolType)
	{
		AddWrapper(make_unique<WrapBaseBool>(name, boolType));
	}

	inline static BoolType m_bColorMenu { false };
	inline static BoolType m_bAutoOpen  { true };
	inline static BoolType m_bSound     { false };

private:

	inline static vector<unique_ptr<WrapBase>> m_prefVector;
	inline static wstring                      m_wstrPreferencesFile;
};
