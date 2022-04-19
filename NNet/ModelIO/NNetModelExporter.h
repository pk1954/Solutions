// NNetModelExporter.h 
//
// ModelIO

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
	void WriteModel(NNetModelReaderInterface const &);

private:
	NNetModelReaderInterface const * m_pNMRI                    { nullptr };
	Observable                     * m_unsavedChangesObservable { nullptr };

	NobIdList m_CompactIds;

	int getCompactIdVal(NobId const) const;

	void writeNob             (wostream &, Nob  const &) const;
	void writePipe            (wostream &, Pipe const &) const;
	void writePipeVoltage     (wostream &, Pipe const &) const;
	void writeDescription     (wostream &) const;
	void writeMonitorData     (wostream &) const;
	void writeTriggerSounds   (wostream &) const;
	void writeSigGenData      (wostream &) const;
	void writeGlobalParameters(wostream &) const;
	void writeNobs            (wostream &);
	void writeHeader          (wostream &) const;
	void write                (wostream &);
};