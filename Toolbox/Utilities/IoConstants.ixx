// IoConstants.ixx
//
// ToolBox/Utilities

module;

#include <string>

export module IoConstants;

using std::wstring;

export inline wstring const FROM_TO            { L"->" };
export inline wchar_t const SEPARATOR          { L'|' };
export inline wchar_t const NR_SEPARATOR       { L':' };
export inline wchar_t const ID_SEPARATOR       { L',' };
export inline wchar_t const OPEN_BRACKET       { L'(' };
export inline wchar_t const CLOSE_BRACKET      { L')' };
export inline wchar_t const LIST_OPEN_BRACKET  { L'{' };
export inline wchar_t const LIST_CLOSE_BRACKET { L'}' };