// AnimationChainCommand.h
//
// Commands

#pragma once

#include "MoreTypes.h"
#include "NNetModelWriterInterface.h"
#include "UPShapeList.h"
#include "ShapePtrList.h"
#include "Command.h"
#include "Connector.h"
#include "ConnectionNeuron.h"

using std::vector;

class AnimationChainCommand
{
public:
	AnimationChainCommand(vector<Command> const list)
	{
	}

	virtual void Do( NNetModelWriterInterface & nmwi ) 
	{ 
	}

	virtual void Undo( NNetModelWriterInterface & nmwi )
	{
	}

private:

};
