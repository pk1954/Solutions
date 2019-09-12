// EvoGenerationCmd.cpp
//
// EvoHistGlue

#include "stdafx.h"
#include <unordered_map>
#include <fstream>
#include "EvoGenerationCmd.h"

bool IsEditorCommand( tEvoCmd const cmd )
{
	static_assert( (int)tEvoCmd::LAST_APP_CMD < GenerationCmd::MAX_APP_CMD, "Too many tEvoCmd values" );

	return ( tEvoCmd::FIRST_EDIT_CMD <= cmd ) && ( cmd <= tEvoCmd::LAST_APP_CMD );
}

wchar_t const * const GetEvoCommandNameShort( tEvoCmd const cmd )
{
    static std::unordered_map < tEvoCmd, wchar_t const * const > mapNames =
    {
        { tEvoCmd::editSetBrushMode,        L"MODE"   },
        { tEvoCmd::editSetBrushShape,       L"SHAPE"  },
        { tEvoCmd::editSetBrushManipulator, L"MANIP"  },
        { tEvoCmd::editSetBrushRadius,      L"RADIUS" },
        { tEvoCmd::editSetBrushIntensity,   L"INTENS" },
        { tEvoCmd::editDoEdit,              L"EDIT"   },
        { tEvoCmd::editSetPOI,              L"SETPOI" }
    };

    return mapNames.at( cmd );
}

wchar_t const * const GetEvoCommandName( tEvoCmd const cmd )
{
    static std::unordered_map < tEvoCmd, wchar_t const * const > mapNames =
    {
        { tEvoCmd::editSetBrushMode,        L"tEvoCmd::editSetBrushMode" },
        { tEvoCmd::editSetBrushShape,       L"tEvoCmd::editSetBrushShape" },
        { tEvoCmd::editSetBrushManipulator, L"tEvoCmd::editSetBrushManipulator" },
        { tEvoCmd::editSetBrushRadius,      L"tEvoCmd::editSetBrushRadius" },
        { tEvoCmd::editSetBrushIntensity,   L"tEvoCmd::editSetBrushIntensity" },
        { tEvoCmd::editDoEdit,              L"tEvoCmd::editDoEdit" },
        { tEvoCmd::editSetPOI,              L"tEvoCmd::editSetPOI" }
    };

    return mapNames.at( cmd );
}

std::wostream & operator << ( std::wostream & out, tEvoCmd const & cmd )
{
	if ( GenerationCmd::IsAppCmd( static_cast<tGenCmd>(cmd) ) )
		out << GetEvoCommandNameShort( cmd  );
	else 
		out << GetGenerationCmdNameShort( static_cast<tGenCmd>(cmd) );
    return out;
};
