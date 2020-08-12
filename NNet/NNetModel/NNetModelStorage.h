// NNetModelStorage.h 
//
// NNetModel

#pragma once

#include <vector>
#include <fstream>
#include "ObserverInterface.h"
#include "MoreTypes.h"
#include "ShapeId.h"

using std::wostream;
using std::wstring;
using std::vector;

class Param;
class NNetModel;
class Script;
class Shape;
class Observable;

class ReadModelResult
{
public:
	enum class tResult
	{
		ok,
		fileNotFound,
		errorInFile
	};

	virtual void Reaction( tResult const, wstring const = wstring() ) = 0;
};

class NNetModelStorage : public ObserverInterface
{
public:
	void Initialize
	( 
		NNetModel       * const, 
		Param           * const, 
		Observable      * const,
		Script          * const,       
		ReadModelResult * const
	);

	virtual void Notify( bool const bImmediate ) { setUnsavedChanges( true ); }

	void Write( wostream & );
	bool Read( bool const, wstring const = L"" );
	void ReadAsync( wstring const = L"" );

	bool    const UnsavedChanges( ) const { return m_bUnsavedChanges; };
	wstring const GetModelPath  ( ) const 
	{ 
		return m_wstrPathOfOpenModel; 
	};
	void          ResetModelPath( );

	bool AskAndSave  ( );
	bool AskModelFile( );
	bool SaveModel   ( );
	bool SaveModelAs ( );

private:

	mutable bool m_bUnsavedChanges { false };  // can be changed in const functions

	HWND              m_hwndApp                  { nullptr };
	NNetModel       * m_pModel                   { nullptr };
	Param           * m_pParam                   { nullptr };
	Observable      * m_unsavedChangesObservable { nullptr };
	Script          * m_pScript                  { nullptr };
	ReadModelResult * m_pResult                  { nullptr };

	bool            m_bPreparedForReading { false };
	wstring         m_wstrPathOfOpenModel { L"" };
	wstring         m_wstrPathOfNewModel  { L"" };
	vector<ShapeId> m_CompactIds;

	long getCompactIdVal( ShapeId const id ) { return m_CompactIds[ id.GetValue() ].GetValue();	}

	void prepareForReading( );
	bool readModel( );
	void writeModel( );
	void WriteShape( wostream &, Shape & );
	void WriteMicroMeterPoint( wostream &, MicroMeterPoint const & );
	void WritePipe( wostream &, Shape const & );
	void setUnsavedChanges( bool const );

	friend static unsigned int __stdcall readModelThreadProc( void * );
};
