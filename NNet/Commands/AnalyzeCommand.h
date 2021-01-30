// AnalyzeLoopsCommand.h
//
// Commands

#pragma once

#include "NNetModelWriterInterface.h"
#include "Analyzer.h"
#include "SelectionCommand.h"

class AnalyzeCommand : public SelectionCommand
{
public:
	AnalyzeCommand( ShapeStack const stack )
		: m_result ( stack )
	{}

	virtual void Do( NNetModelWriterInterface & nmwi ) 
	{ 
		SelectionCommand::Do( nmwi );
		if ( ! m_result.empty() )
		{
			nmwi.GetShapes().SelectAllShapes( tBoolOp::opFalse );
			for ( auto it : m_result )
				nmwi.SelectShape( it->GetId(), tBoolOp::opTrue );
		}
	}

protected:
	ShapeStack m_result;
};