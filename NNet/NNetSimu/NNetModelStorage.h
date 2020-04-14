// NNetModelStorage.h 
//
// NNetSimu

#pragma once

#include <vector>
#include <fstream>
#include "MoreTypes.h"
#include "ShapeId.h"

using std::wostream;
using std::wstring;
using std::vector;

class Param;
class NNetModel;
class Shape;

class NNetModelStorage
{
public:
	NNetModelStorage( NNetModel * const, Param * const );

	void Write( wostream & );
	bool Read( wstring const = L"" );

	wstring const GetModelPath  ( ) { return m_wstrPathOfOpenModel; };
	void          ResetModelPath( );

	int  AskSave( );
	bool AskAndSave( );
	bool AskModelFile( );
	bool SaveModel  ( );
	bool SaveModelAs( );

private:
	NNetModel     * m_pModel { nullptr };
	Param         * m_pParam { nullptr };
	bool            m_bPreparedForReading { false };
	wstring         m_wstrPathOfOpenModel { L"" };
	vector<ShapeId> m_CompactIds;

	long getCompactIdVal( ShapeId const id ) { return m_CompactIds[ id.GetValue() ].GetValue();	}

	void prepareForReading( );
	void writeModel( );
	void WriteShape( wostream &, Shape & );
	void WriteMicroMeterPoint( wostream &, MicroMeterPoint const & );
	void WritePipe( wostream &, Shape const & );
};
