// NNetModelExporter.h 
//
// NNetModel

#pragma once

#include <vector>
#include <fstream>
#include "MoreTypes.h"
#include "NobId.h"
#include "NobIdList.h"

using std::wostream;
using std::wstring;
using std::vector;

class Param;
class Script;
class Pipe;
class Nob;
class IoConnector;
class UPNobList;
class Observable;
class ModelDescription;
class NNetModelReaderInterface;

class NNetModelExporter
{
public:
	void Initialize(NNetModelReaderInterface * const);
	void WriteModel();

private:
	NNetModelReaderInterface * m_pNMRI                    { nullptr };
	Observable               * m_unsavedChangesObservable { nullptr };

	NobIdList m_CompactIds;

	size_t getCompactIdVal(NobId const);

	void writeNob             (wostream &, Nob         const &);
	void writePipe            (wostream &, Pipe        const &);
	void writeIoConnector     (wostream &, IoConnector const &);
	void writeDescription     (wostream &) const;
	void writeMonitorData     (wostream &) const;
	void writeTriggerSounds   (wostream &);
	void writeNobParameters   (wostream &);
	void writeGlobalParameters(wostream &) const;
	void writeNobs            (wostream &);
	void writeHeader          (wostream &) const;
	void write                (wostream &);
};
