// NNetWrappers.ixx : wrapper functions for unit tests and scripting of NNet
//
// NNetWindows

module;

#include "NNetModelReaderInterface.h"
#include "NNetModelCommands.h"
#include "NNetModelIO.h"

export module NNetWrappers;

export void NNetWrappersSetModelInterface(NNetModelReaderInterface * const);

export void InitializeNNetWrappers
(
    NNetModelCommands * const,
    NNetModelIO       * const
);