// EvoGenerationCmd.cpp
//

#include "stdafx.h"
#include <unordered_map>
#include <fstream>
#include "EvoGenerationCmd.h"

using namespace std;

bool IsEditorCommand( tEvoCmd const cmd )
{
	return ( tEvoCmd::editSetBrushMode <= cmd ) && ( cmd <= tEvoCmd::editSetPOI );
}

wchar_t const * const GetEvoCommandNameShort( tEvoCmd const cmd )
{
    static unordered_map < tEvoCmd, wchar_t const * const > mapNames =
    {
        { tEvoCmd::nextGen,               L"NEXT "  },
        { tEvoCmd::reset,                 L"RESET"  },
        { tEvoCmd::editSetBrushMode,      L"MODE"   },
        { tEvoCmd::editSetBrushShape,     L"SHAPE"  },
        { tEvoCmd::editSetBrushSize,      L"SIZE"   },
        { tEvoCmd::editSetBrushIntensity, L"INTENS" },
        { tEvoCmd::editDoEdit,            L"EDIT"   },
        { tEvoCmd::editSetPOI,            L"SETPOI" }
    };

    return mapNames.at( cmd );
}

wchar_t const * const GetEvoCommandName( tEvoCmd const cmd )
{
    static unordered_map < tEvoCmd, wchar_t const * const > mapNames =
    {
        { tEvoCmd::nextGen,               L"tEvoCmd::nextGen" },
        { tEvoCmd::reset,                 L"tGenCmd::reset" },
        { tEvoCmd::editSetBrushMode,      L"tEvoCmd::editSetBrushMode" },
        { tEvoCmd::editSetBrushShape,     L"tEvoCmd::editSetBrushShape" },
        { tEvoCmd::editSetBrushSize,      L"tEvoCmd::editSetBrushSize" },
        { tEvoCmd::editSetBrushIntensity, L"tEvoCmd::editSetBrushIntensity" },
        { tEvoCmd::editDoEdit,            L"tEvoCmd::editDoEdit" },
        { tEvoCmd::editSetPOI,            L"tEvoCmd::editSetPOI" }
    };

    return mapNames.at( cmd );
}

wostream & operator << ( wostream & out, tEvoCmd const & cmd )
{
	if ( GenerationCmd::IsAppCmd( static_cast<GenerationCmd::tGenCmd>(cmd) ) )
		out << GetEvoCommandNameShort( cmd  );
	else 
		out << GetGenerationCmdNameShort( static_cast<GenerationCmd::tGenCmd>(cmd) );
    return out;
};
