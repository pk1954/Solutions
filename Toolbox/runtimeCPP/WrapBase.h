// WrapBase.h
//
// Toolbox\runtimeCPP

#pragma once

#include <string>
#include "SYMTAB.H"

using std::wostream;
using std::wstring;
using std::endl;

class WrapBase : public ScriptFunctor
{
public:
    explicit WrapBase(wstring const & wstrName)
    {
        SymbolTable::ScrDefConst(wstrName, this);
    }

    virtual void Write(wostream& out) const = 0;

    void WriteCmdName(wostream & out) const
    {
        out << SymbolTable::GetSymbolName(Symbol(this)) << L" ";
    }

    void Write2(wostream & out) const
    {
        WriteCmdName(out);
        Write(out);
        out << endl;
    }
};