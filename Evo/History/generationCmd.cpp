// generationCmd.cpp
//
// HistoryInterface


#include <unordered_map>
#include "GenerationCmd.h"

wchar_t const * const GetGenerationCmdNameShort( GenerationCmd::Id const cmd )
{
    static std::unordered_map < GenerationCmd::Id, wchar_t const * const > mapNames =
    {
		{ GenerationCmd::Id::CACHED,    L"CACHE" },
		{ GenerationCmd::Id::NEXT_GEN,  L"NEXT"  },
        { GenerationCmd::Id::UNDEFINED, L"UNDEF" },
	    { GenerationCmd::Id::RESET,     L"RESET" }
	};

    return mapNames.at( cmd );
}

wchar_t const * const GetGenerationCmdName( GenerationCmd::Id const cmd )
{
    static std::unordered_map < GenerationCmd::Id, wchar_t const * const > mapNames =
    {
	    { GenerationCmd::Id::CACHED,    L"GenerationCmd::Id::cached"    },
		{ GenerationCmd::Id::NEXT_GEN,  L"GenerationCmd::Id::nextGen"   },
		{ GenerationCmd::Id::UNDEFINED, L"GenerationCmd::Id::undefined" },
		{ GenerationCmd::Id::RESET,     L"GenerationCmd::Id::reset"     }
	};

    return mapNames.at( cmd );
}

std::wostream & operator << ( std::wostream & out, GenerationCmd const & genCmd )
{
    GenerationCmd::Id cmd = genCmd.GetCommand( );
    out << GetGenerationCmdNameShort( cmd );
    return out;
};
