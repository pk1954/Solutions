// NNetModelIO.h 
//
// ModelIO

#pragma once

#include <string>
#include "NobId.h"
#include "NobIdList.h"
#include "InputOutputUI.h"
#include "NNetWrapperBase.h"
#include "NNetModelWriterInterface.h"

using std::unique_ptr;
using std::wstring;

class Script;
class NNetModel;
class WrapBase;
class InputOutputUI;
class MonitorWindow;

class NNetModelIO
{
public:
	~NNetModelIO();

	void Initialize
	(
//		MonitorWindow const *
	);

	/// import ///

	bool Import(wstring const &, unique_ptr<InputOutputUI>);

	static void CheckImportedNobId(Script &, UPNobList const &, NobId const);

	unique_ptr<NNetModel> GetImportedModel();
	wstring const &       GetModelFileName() const { return m_wstrFile2Read; }

	NNetModelWriterInterface & GetImportNMWI() { return * m_upImportedNMWI; }

	/// export ///

	void Export(NNetModelReaderInterface const &, unique_ptr<InputOutputUI>);

	int    GetCompactIdVal(NobId const) const;
	size_t NrOfCompactIds()             const;

	NNetModelReaderInterface const & GetExportNMRI() const { return * m_pExportNMRI; }

private:

	unique_ptr<InputOutputUI>            m_upImportUI;   
	vector<unique_ptr<NNetWrapperBase>>  m_wrapVector;
	TP_TIMER                           * m_pTpTimer { nullptr };

	/// import ///

	unique_ptr<NNetModelWriterInterface> m_upImportedNMWI;  // valid only during import
	unique_ptr<NNetModel>                m_upImportedModel; // valid only during import
	wstring                              m_wstrFile2Read;

	void importModel();

	friend static unsigned int __stdcall importModelThreadProc(void *);

	/// export ///

	NobIdList                        m_CompactIds;
	NNetModelReaderInterface const * m_pExportNMRI { nullptr };  // valid only during export

	void compress(NNetModelReaderInterface const &);
	void writeHeader(wostream &) const;
};
