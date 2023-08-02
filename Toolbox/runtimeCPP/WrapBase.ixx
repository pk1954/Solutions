// WrapBase.ixx
//
// Toolbox\runtimeCPP

module;

#include <string>
#include <iostream>
#include <type_traits>

export module WrapBase;

import BoolType;
import Script;
import Symtab;

using std::wostream;
using std::wstring;
using std::is_base_of;
using std::remove_pointer_t;

export class WrapBase : public ScriptFunctor
{
public:
    explicit WrapBase(wstring const& wstrName)
    {
        SymbolTable::ScrDefConst(wstrName, this);
    }

    virtual ~WrapBase() = default;

    virtual void Write(wostream& out) const = 0;

    wstring const& GetName() const
    {
        return SymbolTable::GetSymbolName(Symbol(this));
    }

    void WriteCmdName(wostream& out) const
    {
        out << GetName() << L' ';
    }
};

export template <typename T>
concept Wrap_t = is_base_of<WrapBase, remove_pointer_t<T>>::value;
